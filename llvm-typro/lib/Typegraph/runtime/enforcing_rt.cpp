#include "enforcing_rt.h"
#include "AssemblyBuilder.h"
#include "typegraph_binary_loader.h"

#include <algorithm>
#include <llvm/Typegraph/Typegraph.h>
#include <llvm/Typegraph/timeclock.h>
#include <mutex>
#include <nlohmann/json.hpp>
#include <tuple>
#include <vector>
#include <unistd.h>
#include <sys/mman.h>

// #define RT_EVALUATE_RUNTIME

namespace typegraph {
namespace rt {

static inline void *pageOf(void *Addr) {
  uintptr_t I = (uintptr_t) Addr;
  I = I ^ (I & 0xfff);
  return (void*) I;
}

#ifdef RT_EVALUATE_RUNTIME
#include <syscall.h>
void reportRuntime(double T, const char *What) {
  char Buffer[256];
  int Size = sprintf(Buffer, "[RT BENCH] %9.6fs %s\n", T, What);
  while (Size > 0) {
    // custom printf, not depending on stdlib
    int Result = syscall(SYS_write, 2, Buffer, Size);
    if (Result <= 0)
      return;
    Size -= Result;
  }
}
#endif

class DynamicSymbolInfo {
public:
  const std::string *DynamicSymbolCallName;
  // List of all dispatchers that receive targets from this dynamic symbol resolver. Indices in Runtime::Dispatchers
  std::vector<size_t, ProtectedAllocator<size_t>> DispatcherIndices;
  // Set of all functions this dispatcher already resolved so far
  std::set<FunctionInfos, std::less<FunctionInfos>, ProtectedAllocator<FunctionInfos>> TargetFunctions;

  DynamicSymbolInfo(const std::string *DynamicSymbolCallName) : DynamicSymbolCallName(DynamicSymbolCallName) {}
};

struct TypegraphRuntime {
  std::mutex Mutex;

  TypeGraph Graph;
  std::vector<std::tuple<const char *, void **, long>, ProtectedAllocator<std::tuple<const char *, void **, long>>> LoadableGraphData;
  std::vector<long, ProtectedAllocator<long>> Modules;
  bool MainModuleSeen = false;

  std::vector<std::unique_ptr<FunctionInfos>, ProtectedAllocator<std::unique_ptr<FunctionInfos>>> Functions;
  std::vector<DispatcherInfos, ProtectedAllocator<DispatcherInfos>> Dispatchers;
  std::map<const std::string *, std::set<FunctionInfos>, std::less<const std::string *>, ProtectedAllocator<std::pair<const std::string *const, std::set<FunctionInfos>>>> ResolvePoints;
  std::map<void *, long, std::less<void *>, ProtectedAllocator<std::pair<void *const, long>>> DynamicSymbolToId;
  std::map<const std::string *, std::unique_ptr<DynamicSymbolInfo>, std::less<const std::string *>, ProtectedAllocator<std::pair<const std::string *const, std::unique_ptr<DynamicSymbolInfo>>>> DynamicSymbolInfos;
  long NextDynamicSymbolID = 0x100000;

  inline void registerGraph(const char *GraphData, void **References, long ModuleID) {
    dprintf("[RT] Register graph %p %p 0x%lx\n", (const void *)GraphData, (void *)References, ModuleID);
    loadReferences(GraphData, References, ModuleID);
    LoadableGraphData.emplace_back(GraphData, References, ModuleID);
    Modules.push_back(ModuleID);
    invalidateDispatchersIfNecessary();

    if (ModuleID == 1)
      MainModuleSeen = true;
    if (MainModuleSeen && getenv("TG_CFI_OUTPUT_RT")) {
      writeCfiResultsToJson(getenv("TG_CFI_OUTPUT_RT"));
    }
#ifdef RT_EVALUATE_RUNTIME
    if (MainModuleSeen && Modules.size() > 1) {
      evaluateRuntime();
    }
#endif
  }

  bool loadAllGraphData(bool InvalidateHandlers) {
    if (LoadableGraphData.empty())
      return false;
    // we don't need graph data if we have only one module
    if (LoadableGraphData.size() < 2 && Graph.num_vertices() == 0)
      return false;

#ifdef RT_EVALUATE_RUNTIME
    auto StartTime = std::chrono::high_resolution_clock::now();
#endif

    if (DEBUG_OUTPUT)
      std::cerr << "[RT] Loading " << LoadableGraphData.size() << " graphs ..." << std::endl;
    TimeClock Clock;
    for (auto &GD : LoadableGraphData) {
      loadTypegraphFromBinary(Graph, std::get<0>(GD), Functions, std::get<1>(GD));
    }
    if (DEBUG_OUTPUT) {
      Clock.report("[RT] Graphs loaded");
      dprintf("[RT] # vertices = %ld   # edges = %ld\n", Graph.num_vertices(), Graph.num_edges());
    }
    LoadableGraphData.clear();

#ifdef RT_EVALUATE_RUNTIME
    auto Duration = std::chrono::high_resolution_clock::now() - StartTime;
    reportRuntime(std::chrono::duration_cast<std::chrono::microseconds>(Duration).count() / 1000000.0, "loading");
    StartTime = std::chrono::high_resolution_clock::now();
#endif

    Graph.combineEquivalencesInline(false, false, false);
    Graph.computeReachability(true, false);
    if (DEBUG_OUTPUT)
      Clock.report("[RT] Compute reachability");

    // compute new target sets
    for (size_t I = 0; I < Dispatchers.size(); I++) {
      auto &Dispatcher = Dispatchers[I];
      auto &CallInfos = Graph.CallInfos[Dispatcher.CallName];
      for (auto V : CallInfos.AllVertices) {
        for (auto &FU : Graph[V].FunctionUses) {
          // check dynamic symbols - connect this dispatcher to dynamic symbol, and insert already-known targets
          if (FU.IsDynamicFunction) {
            auto It = DynamicSymbolInfos.find(FU.SymbolName);
            if (It != DynamicSymbolInfos.end()) {
              if (std::find(It->second->DispatcherIndices.begin(), It->second->DispatcherIndices.end(), I) == It->second->DispatcherIndices.end()) {
                It->second->DispatcherIndices.push_back(I);
                for (auto FI: It->second->TargetFunctions) {
                  Dispatcher.insertTarget(FI);
                }
              }
            }
            continue;
          }
          // check compatibility
          if (FU.IsVarArg ? FU.NumArgs > CallInfos.NumArgs : FU.NumArgs != CallInfos.NumArgs)
            continue;
          // check if dispatcher already handled this
          if (Dispatcher.AlreadyHandledTargets.find(FU.Function->ID) != Dispatcher.AlreadyHandledTargets.end())
            continue;
          // add ID to target set
          Dispatcher.insertTarget(*FU.Function);
        }
      }
    }

    computeResolvePointTargetSets();

    if (getenv("TG_RT_GRAPH_OUTPUT")) {
      Graph.saveToFile(getenv("TG_RT_GRAPH_OUTPUT"));
      dprintf("Saved graph to '%s'\n", getenv("TG_RT_GRAPH_OUTPUT"));
    }

    // invalidate dispatchers if necessary (if target set has changed)
    if (InvalidateHandlers) {
      invalidateDispatchersIfNecessary();
    }

#ifdef RT_EVALUATE_RUNTIME
    Duration = std::chrono::high_resolution_clock::now() - StartTime;
    reportRuntime(std::chrono::duration_cast<std::chrono::microseconds>(Duration).count() / 1000000.0, "computation");
#endif

    return true;
  }

  void computeResolvePointTargetSets() {
    for (auto &It: ResolvePoints) {
      auto &CallInfos = Graph.CallInfos[It.first];
      for (auto V: CallInfos.AllVertices) {
        for (auto &FU : Graph[V].FunctionUses) {
          It.second.insert(*FU.Function);
        }
      }
    }
  }

  void invalidateDispatchersIfNecessary() {
    void *rwpage = nullptr;

    assertLazyDispatchersExist(Dispatchers.size());
    for (size_t Index = 0; Index < Dispatchers.size(); Index++) {
      if (Dispatchers[Index].Targets.size() != Dispatchers[Index].CurrentImplHandlesTargets) {
        // if (Index < (sizeof(LazyDispatchers) / sizeof(void *))) {
          if (pageOf(Dispatchers[Index].DispatcherAddr) != rwpage) {
            if (*Dispatchers[Index].DispatcherAddr == LazyDispatchers->at(Index))
              continue;
            makeRO(rwpage);
            rwpage = pageOf(Dispatchers[Index].DispatcherAddr);
            makeRW(rwpage);
          }
          // dprintf("Writing lazy to %ld = %p\n", Index, (void *)Dispatchers[Index].DispatcherAddr);
          *Dispatchers[Index].DispatcherAddr = LazyDispatchers->at(Index);
        // } else {
        //   fprintf(stderr, "Requires %ld lazy dispatchers...\n", Dispatchers.size());
        //   assert(false && "Not implemented (too much dispatchers for now)");
        // }
      }
    }

    makeRO(rwpage);
  }

  void loadReferences(const char *&GraphData, void **&References, long ModuleID) {
    BinaryStreamReader Stream(GraphData);
    auto Num = Stream.readSize();
    dprintf("[RT] Reading %ld new dispatchers from %p\n", Num, (void *)References);
    for (; Num--;) {
      const auto *CallName = Graph.SymbolContainer->get(Stream.readString());
      void *DispatcherAddr = *(References++);
      Dispatchers.emplace_back(CallName, (void **)DispatcherAddr, ModuleID);
      auto NumAlreadyHandled = Stream.readSize();
      for (; NumAlreadyHandled--;) {
        Dispatchers.back().AlreadyHandledTargets.insert(Stream.readLong());
      }
    }

    Num = Stream.readSize();
    dprintf("[RT] Reading %ld new dynamic symbol containers\n", Num);
    for (; Num--;) {
      const auto *DynSymbolCallName = Graph.SymbolContainer->get(Stream.readString());
      auto *InfoStorage = (DynamicSymbolInfo**) *(References++);
      DynamicSymbolInfos[DynSymbolCallName] = std::make_unique<DynamicSymbolInfo>(DynSymbolCallName);
      *InfoStorage = DynamicSymbolInfos[DynSymbolCallName].get();
    }

    Num = Stream.readSize();
    dprintf("[RT] Reading %ld new dynamic symbol target sets\n", Num);
    for (; Num--;) {
      const auto *DynSymbolCallName = Graph.SymbolContainer->get(Stream.readString());
      auto &It = Graph.DynamicSymbolTargets[DynSymbolCallName];
      auto Num2 = Stream.readSize();
      for (; Num2--;) {
        It.insert(Graph.SymbolContainer->get(Stream.readString()));
      }
    }

    GraphData = Stream.getBuffer();
  }

  void debugGraph() {
    loadAllGraphData(true);
    Graph.saveToFile("/tmp/dynlib.typegraph");
    system("show_typegraph.py --numbers --all /tmp/dynlib.typegraph");
  }

  void *generateDispatcher(int Index, uintptr_t CurrentID) {
    // concurrency might affect us here!
    std::lock_guard Guard(Mutex);
    dprintf("[RT] generateDispatcher (index=%d, r11=%lu =%p)\n", Index, CurrentID, (void *)CurrentID);

    bool GraphHasChanged = loadAllGraphData(false);
    // debugGraph();

    // build handler here
    if (Dispatchers[Index].CurrentImplHandlesTargets != Dispatchers[Index].Targets.size()) {
      makeRW((void *) Dispatchers[Index].DispatcherAddr);
      {
        TimeClock Clock;
        AssemblyBuilder AsmBuilder;
        AsmBuilder.generateDispatcher(Index, Dispatchers[Index].Targets, Dispatchers[Index].DispatcherAddr, CurrentID);
        Dispatchers[Index].CurrentImplHandlesTargets = Dispatchers[Index].Targets.size();
        if (DEBUG_OUTPUT)
          Clock.report("[RT] Building dispatcher");
#ifdef ASM_BUILDER_CHECKING
        AsmBuilder.startChecking();
        AsmBuilder.generateDispatcher(Index, Dispatchers[Index].Targets, Dispatchers[Index].DispatcherAddr, CurrentID);
#endif
      }
      makeRO((void *) Dispatchers[Index].DispatcherAddr);
    }

    if (GraphHasChanged) {
      invalidateDispatchersIfNecessary();
    }

    dprintf("[RT] generateDispatcher finished (index=%d, r11=%lu =%p)\n", Index, CurrentID, (void *)CurrentID);
    return *Dispatchers[Index].DispatcherAddr;
  }

  void generateAllDispatcher() {
    // concurrency might affect us here!
    std::lock_guard Guard(Mutex);

    bool GraphHasChanged = loadAllGraphData(false);

    // build handler here
    void *CurrentRW = nullptr;
    for (size_t Index = 0; Index < Dispatchers.size(); Index++) {
      if (Dispatchers[Index].CurrentImplHandlesTargets != Dispatchers[Index].Targets.size()) {
        if (CurrentRW != (void *)Dispatchers[Index].DispatcherAddr) {
          if (CurrentRW)
            makeRO(CurrentRW);
          CurrentRW = (void *)Dispatchers[Index].DispatcherAddr;
          makeRW(CurrentRW);
        }
        {
          AssemblyBuilder AsmBuilder;
          AsmBuilder.generateDispatcher(Index, Dispatchers[Index].Targets, Dispatchers[Index].DispatcherAddr,-1);
          Dispatchers[Index].CurrentImplHandlesTargets = Dispatchers[Index].Targets.size();
#ifdef ASM_BUILDER_CHECKING
          AsmBuilder.startChecking();
          AsmBuilder.generateDispatcher(Index, Dispatchers[Index].Targets, Dispatchers[Index].DispatcherAddr,-1);
#endif
        }
      }
    }
    if (CurrentRW) {
      makeRO(CurrentRW);
    }

    if (GraphHasChanged) {
      invalidateDispatchersIfNecessary();
    }
  }

  std::map<long, const std::string *> getFunctionIDToSymbol() {
    std::map<long, const std::string *> Result;
    for (auto V: Graph.vertex_set()) {
      for (auto &FU: Graph[V].FunctionUses) {
        Result[FU.Function->ID] = FU.SymbolName;
      }
    }
    return Result;
  }

  void writeCfiResultsToJson(std::string Filename) {
    if (Filename == "auto") {
      std::string Basename(program_invocation_name);
      // if (Basename.find("redis-server") == std::string::npos) return;
      if (getenv("TG_CFI_OUTPUT_RT_BASE"))
        Basename = getenv("TG_CFI_OUTPUT_RT_BASE");
      int I = 0;
      do {
        Filename = Basename + ".dynamic-tgcfi" + std::to_string(I++) + ".json";
      } while (access(Filename.c_str(), F_OK) == 0);
    }

    if (loadAllGraphData(false)) {
      invalidateDispatchersIfNecessary();
    }

    nlohmann::json J;
    J["binary"] = program_invocation_name;
    // list of loaded graphs / modules in order of load
    J["modules"] = nlohmann::json::array();
    for (auto ModuleID: Modules) {
      J["modules"].push_back(ModuleID);
    }
    auto IDToName = getFunctionIDToSymbol();
    auto AdditionalTargets = nlohmann::json::object();
    for (auto &Dispatcher: Dispatchers) {
      auto Arr = nlohmann::json::array();
      for (auto &Target: Dispatcher.Targets) {
        auto It = IDToName.find(Target.ID);
        if (It != IDToName.end()) {
          Arr.push_back(*It->second);
        } else {
          Arr.push_back(Target.ID);
        }
      }
      AdditionalTargets[*Dispatcher.CallName] = Arr;
    }
    J["tg_additional_targets"] = AdditionalTargets;

    // write file
    auto *F = fopen(Filename.c_str(), "w");
    if (!F) {
      fprintf(stderr, "ERROR: Can't write \"%s\".\n", Filename.c_str());
      return;
    }
    auto S = J.dump(2);
    fputs(S.c_str(), F);
    fclose(F);
  }

  void reportError(size_t DispatcherIndex, long ID) {
    fprintf(
        stderr, "[RT] DYNAMIC ERROR in dispatcher %ld for call \"%s\": ID %ld = %p, %ld valid targets: ", DispatcherIndex,
        Dispatchers[DispatcherIndex].CallName->c_str(), ID, (void *)ID, Dispatchers[DispatcherIndex].Targets.size());
    for (auto &F: Dispatchers[DispatcherIndex].Targets) {
      fprintf(stderr, "%ld,", F.ID);
    }
    fprintf(stderr, "\n");
  }

  long resolveDynamicSymbol(DynamicSymbolInfo *DynamicSymbolCall, void *Symbol) {
    dprintf("[RT] Call \"%s\" resolves symbol %p ...\n", DynamicSymbolCall->DynamicSymbolCallName->c_str(), Symbol);
    // Get an ID
    auto It = DynamicSymbolToId.find(Symbol);
    if (It == DynamicSymbolToId.end()) {
      // new symbol - assign ID and save
      long ID = NextDynamicSymbolID++;
      DynamicSymbolToId[Symbol] = ID;
      FunctionInfos Info(ID, Symbol);
      DynamicSymbolCall->TargetFunctions.insert(Info);
      for (auto I: DynamicSymbolCall->DispatcherIndices) {
        Dispatchers[I].Targets.push_back(Info);
        // make dirty
        invalidateDispatchersIfNecessary();
      }
      dprintf("[RT] => ID %ld, available in %ld dispatchers\n", ID, DynamicSymbolCall->DispatcherIndices.size());
      return ID;
    }
    FunctionInfos Info(It->second, Symbol);
    if (DynamicSymbolCall->TargetFunctions.insert(Info).second) {
      for (auto I : DynamicSymbolCall->DispatcherIndices) {
        // check if already existing
        if (Dispatchers[I].insertTarget(Info)) {
          // make dirty
          invalidateDispatchersIfNecessary();
        }
      }
      dprintf("[RT] => ID %ld, available in %ld dispatchers\n", It->second,
              DynamicSymbolCall->DispatcherIndices.size());
    } else {
      dprintf("[RT] => ID %ld, not new\n", It->second);
    }
    return It->second;
  }

  void *resolvePoint(const char *Name, long ID) {
    const auto *Symbol = Graph.SymbolContainer->get(Name);
    auto It = ResolvePoints.find(Symbol);
    if (It == ResolvePoints.end()) {
      ResolvePoints[Symbol] = {};
      if (!loadAllGraphData(true))
        computeResolvePointTargetSets();
    }
    for (auto &Target: ResolvePoints[Symbol]) {
      if (Target.ID == (unsigned long) ID)
        return Target.Address;
    }
    fprintf(stderr, "[RT] Can't resolve point %s - invalid target %lu (=0x%lx)\n", Name, ID, ID);
#if defined(__x86_64__) || defined(_M_X64)
    __asm__("ud2");
#elif defined(__aarch64__) || defined(_M_ARM64)
    __asm__("brk 1");
#elif defined(mips) || defined(__mips__) || defined(__mips)
    __asm__("break");
#else
    #error "Unsupported architecture!"
#endif
    return nullptr;
  }

  void makeRW(void *Addr) {
    // fprintf(stderr, "[mapping] %p = rw\n", Addr);
    if (Addr && mprotect(pageOf(Addr), 4096, PROT_READ | PROT_WRITE))
      perror("mprotect rw");
  }

  void makeRO(void *Addr) {
    // fprintf(stderr, "[mapping] %p = ro\n", Addr);
    if (Addr && mprotect(pageOf(Addr), 4096, PROT_READ))
      perror("mprotect ro");
  }

  void evaluateRuntime() {
    auto StartTime = std::chrono::high_resolution_clock::now();
    loadAllGraphData(false);
    auto StartTimeDispatchers = std::chrono::high_resolution_clock::now();
    generateAllDispatcher();
    auto DurationDispatchers = std::chrono::high_resolution_clock::now() - StartTimeDispatchers;
    auto Duration = std::chrono::high_resolution_clock::now() - StartTime;
#ifdef RT_EVALUATE_RUNTIME
    reportRuntime(std::chrono::duration_cast<std::chrono::microseconds>(DurationDispatchers).count() / 1000000.0, "dispatchers");
    reportRuntime(std::chrono::duration_cast<std::chrono::microseconds>(Duration).count() / 1000000.0, "total");
    const auto *MaxNumber = getenv("TYPRO_RT_MAX_GRAPHS");
    if (MaxNumber && std::stoi(MaxNumber) == Modules.size()) {
      exit(0);
    }
#endif
  }
};

TypegraphRuntime *Runtime = nullptr;

} // namespace rt
} // namespace typegraph

[[maybe_unused]] EXTERN void __tg_register_graph(const char *GraphData, void **References, long ModuleID) {
  ProtectedAllocatorWriteableScope Scope;
  if (typegraph::rt::Runtime == nullptr)
    typegraph::rt::Runtime = new (ProtectedAllocator<typegraph::rt::TypegraphRuntime>().allocate(1)) typegraph::rt::TypegraphRuntime();
  typegraph::rt::Runtime->registerGraph(GraphData, References, ModuleID);
}

void __tg_dynamic_error(size_t Index, long ID) {
  typegraph::rt::Runtime->reportError(Index, ID);
}

long __tg_dlsym_to_id(typegraph::rt::DynamicSymbolInfo *DynamicSymbolCall, void *Symbol) {
  ProtectedAllocatorWriteableScope Scope;
  return typegraph::rt::Runtime->resolveDynamicSymbol(DynamicSymbolCall, Symbol);
}

void *__tg_resolve_symbol(const char *Name, long ID) {
  ProtectedAllocatorWriteableScope Scope;
  return typegraph::rt::Runtime->resolvePoint(Name, ID);
}

void *generateDispatcher(int Index, uintptr_t CurrentID) {
  ProtectedAllocatorWriteableScope Scope;
  return typegraph::rt::Runtime->generateDispatcher(Index, CurrentID);
}
