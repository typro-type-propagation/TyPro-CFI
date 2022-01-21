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

namespace typegraph {
namespace rt {

static inline void *pageOf(void *Addr) {
  uintptr_t I = (uintptr_t) Addr;
  I = I ^ (I & 0xfff);
  return (void*) I;
}

class DynamicSymbolInfo {
public:
  const std::string *DynamicSymbolCallName;
  // List of all dispatchers that receive targets from this dynamic symbol resolver. Indices in Runtime::Dispatchers
  std::vector<size_t> DispatcherIndices;
  // Set of all functions this dispatcher already resolved so far
  std::set<FunctionInfos> TargetFunctions;

  DynamicSymbolInfo(const std::string *DynamicSymbolCallName) : DynamicSymbolCallName(DynamicSymbolCallName) {}
};

struct TypegraphRuntime {
  std::mutex Mutex;

  TypeGraph Graph;
  std::vector<std::tuple<const char *, void **, long>> LoadableGraphData;
  std::vector<long> Modules;
  bool MainModuleSeen = false;

  std::vector<std::unique_ptr<FunctionInfos>> Functions;
  std::vector<DispatcherInfos> Dispatchers;
  std::map<void *, long> DynamicSymbolToId;
  std::map<const std::string *, std::unique_ptr<DynamicSymbolInfo>> DynamicSymbolInfos;
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
  }

  bool loadAllGraphData(bool InvalidateHandlers) {
    if (LoadableGraphData.empty())
      return false;
    // we don't need graph data if we have only one module
    if (LoadableGraphData.size() < 2 && Graph.num_vertices() == 0)
      return false;

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

    if (getenv("TG_RT_GRAPH_OUTPUT")) {
      Graph.saveToFile(getenv("TG_RT_GRAPH_OUTPUT"));
      dprintf("Saved graph to '%s'\n", getenv("TG_RT_GRAPH_OUTPUT"));
    }

    // invalidate dispatchers if necessary (if target set has changed)
    if (InvalidateHandlers) {
      invalidateDispatchersIfNecessary();
    }
    return true;
  }

  void invalidateDispatchersIfNecessary() {
    void *rwpage = nullptr;

    assertLazyDispatchersExist(Dispatchers.size());
    for (size_t Index = 0; Index < Dispatchers.size(); Index++) {
      if (Dispatchers[Index].Targets.size() != Dispatchers[Index].CurrentImplHandlesTargets) {
        // if (Index < (sizeof(LazyDispatchers) / sizeof(void *))) {
          if (pageOf(Dispatchers[Index].DispatcherAddr) != rwpage) {
            if (*Dispatchers[Index].DispatcherAddr == LazyDispatchers.at(Index))
              continue;
            makeRO(rwpage);
            rwpage = pageOf(Dispatchers[Index].DispatcherAddr);
            makeRW(rwpage);
          }
          // dprintf("Writing lazy to %ld = %p\n", Index, (void *)Dispatchers[Index].DispatcherAddr);
          *Dispatchers[Index].DispatcherAddr = LazyDispatchers.at(Index);
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
      }
      makeRO((void *) Dispatchers[Index].DispatcherAddr);
    }

    if (GraphHasChanged) {
      invalidateDispatchersIfNecessary();
    }

    return *Dispatchers[Index].DispatcherAddr;
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
};

TypegraphRuntime Runtime;

} // namespace rt
} // namespace typegraph

[[maybe_unused]] void __tg_register_graph(const char *GraphData, void **References, long ModuleID) {
  typegraph::rt::Runtime.registerGraph(GraphData, References, ModuleID);
}

void __tg_dynamic_error(size_t Index, long ID) {
  typegraph::rt::Runtime.reportError(Index, ID);
}

long __tg_dlsym_to_id(typegraph::rt::DynamicSymbolInfo *DynamicSymbolCall, void *Symbol) {
  return typegraph::rt::Runtime.resolveDynamicSymbol(DynamicSymbolCall, Symbol);
}

void *generateDispatcher(int Index, uintptr_t CurrentID) { return typegraph::rt::Runtime.generateDispatcher(Index, CurrentID); }
