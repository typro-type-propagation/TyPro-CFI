#include "TypegraphPass.h"
#include "../Typegraph/TGDispatcherBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/Typegraph/TGCallGraph.h"
#include "llvm/Typegraph/Typegraph.h"
#include "llvm/Typegraph/TypegraphSettings.h"
#include "llvm/Typegraph/timeclock.h"
#include "llvm/Typegraph/typegraph_llvm_utils.h"

#include <chrono>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/JSON.h>
#include <llvm/Typegraph/typegraph_layering.h>

using namespace typegraph;

namespace llvm {

namespace {
Value *castTo(Value *V, Type *T, IRBuilder<> &B) {
  if (V->getType() != T) {
    return B.CreateBitCast(V, T);
  }
  return V;
}
} // namespace

class TypeGraphPassInternal {
  TimeClock Clock;
  Module &M;
  TypeGraph Graph;

public:
  TypeGraphPassInternal(Module &M) : M(M) {
    if (Settings.enabled) {
      // Fix paths
      if (Settings.graph_output && std::string(Settings.graph_output) == "auto") {
        Settings.graph_output = Settings.output_filename.c_str();
      }
      if (Settings.tgcfi_output && std::string(Settings.tgcfi_output) == "auto") {
        Settings.tgcfi_output = (new std::string(Settings.output_filename + ".tgcfi.json"))->c_str();
      }
      if (Settings.output_filename.substr(0, 9) == "conftest-") {
        Settings.graph_output = nullptr;
        Settings.tgcfi_output = nullptr;
      }
      // Load graph from modules
      ParseTypegraphFromMetadata(Graph, M);
      Clock.report("graph load");
      if (auto *Path = Settings.graph_output) {
        Graph.saveToFile(std::string(Path) + ".before.typegraph");
        std::error_code EC;
        llvm::raw_fd_ostream OS(std::string(Path) + ".before.bc", EC, llvm::sys::fs::F_None);
        WriteBitcodeToFile(M, OS);
        OS.flush();
      }
    }
  }

  ~TypeGraphPassInternal() {
    if (Settings.enabled) {
      Clock.report("Typegraph combined");
    }
  }

  void addGlobalCtor(llvm::Function *F) {
    // if we have a statically linked musl libc, we don't use the init array, because it messes with initialization order.
    // we add a direct call in libc_start_main_stage2, directly after the init array has been processed instead.
    auto *Stage2 = F->getParent()->getFunction("libc_start_main_stage2");
    if (Stage2) {
      for (auto &BB: *Stage2) {
        for (auto &Ins: BB) {
          if (auto *C = dyn_cast<CallInst>(&Ins)) {
            auto *A = dyn_cast<GlobalAlias>(C->getCalledOperand());
            if ((A && A->getName() == "__libc_start_init") || (C->getCalledFunction() && C->getCalledFunction()->getName() == "__libc_start_init")) {
              IRBuilder<> B(Ins.getNextNode());
              B.CreateCall(F);
              return;
            }
          }
        }
      }
    }

    auto *Void = Type::getVoidTy(M.getContext());
    auto *CharPtr = Type::getInt8PtrTy(M.getContext());
    auto *Int32Ty = Type::getInt32Ty(M.getContext());
    auto *FPT = PointerType::get(FunctionType::get(Void, false), 0);
    llvm::StructType *CtorStructTy = llvm::StructType::get(Int32Ty, FPT, CharPtr);

    auto *Entry =
        ConstantStruct::get(CtorStructTy, {ConstantInt::get(Int32Ty, 0), F, ConstantPointerNull::get(CharPtr)});
    auto *GV = M.getGlobalVariable("llvm.global_ctors", true);
    if (!GV) {
      auto *AT = ArrayType::get(CtorStructTy, 1);
      GV = cast<GlobalVariable>(M.getOrInsertGlobal("llvm.global_ctors", AT));
      GV->setLinkage(GlobalVariable::AppendingLinkage);
      GV->setInitializer(ConstantArray::get(AT, {Entry}));
    } else {
      // append to existing global_ctors
      auto *OldTy = GV->getType()->getPointerElementType();
      auto *AT = ArrayType::get(CtorStructTy, OldTy->getArrayNumElements() + 1);
      std::vector<Constant *> Init2;
      if (OldTy->getArrayNumElements() > 0) {
        auto *Init = cast<ConstantArray>(GV->getInitializer());
        for (auto &Op : Init->operands()) {
          Init2.push_back(cast<Constant>(Op.get()));
        }
      }
      Init2.push_back(Entry);
      GV->setInitializer(nullptr);
      GV->removeFromParent();
      GV = cast<GlobalVariable>(M.getOrInsertGlobal("llvm.global_ctors", AT));
      GV->setLinkage(GlobalVariable::AppendingLinkage);
      GV->setInitializer(ConstantArray::get(AT, Init2));
      llvm::errs() << "CTORS AFTER: " << *GV << "\n";
    }
  }

  void instrumentCollectCalltargets() {
    // Define functions from library
    // void __calltargets_initialize(const char *CallName, size_t CallCount, void **Ident);
    // void __calltargets_add_function(const char *FunctionName, void *FunctionAddress);
    // void __calltargets_count(void **Ident, size_t CallNumber, void *Function);
    auto *Void = Type::getVoidTy(M.getContext());
    auto *CharPtr = Type::getInt8PtrTy(M.getContext());
    auto *IdentTy = CharPtr->getPointerTo();
    auto *SizeT = Type::getInt64Ty(M.getContext());
    auto *FuncInit = cast<Function>(
        M.getOrInsertFunction("__calltargets_initialize", FunctionType::get(Void, {CharPtr, SizeT, IdentTy}, false))
            .getCallee());
    auto *FuncAdd = cast<Function>(
        M.getOrInsertFunction("__calltargets_add_function", FunctionType::get(Void, {CharPtr, CharPtr}, false))
            .getCallee());
    auto *FuncCount = cast<Function>(
        M.getOrInsertFunction("__calltargets_count", FunctionType::get(Void, {IdentTy, SizeT, CharPtr}, false)).getCallee());

    // Define unique identifier for this module/library
    auto *IdentVar = new GlobalVariable(
        M, CharPtr, false, GlobalValue::PrivateLinkage, ConstantPointerNull::getNullValue(CharPtr),
        "__calltargets_global_identifier_" + std::to_string(std::hash<std::string>{}(Settings.output_filename)));

    // Iterate all calls, instrument, and collect names+ids
    std::string CollectedCallNames;
    size_t NextCallId = 0;
    for (auto &F : M.functions()) {
      if (!considerFunctionForEnforcement(F) || F.getName() == "__funcs_on_exit")
        continue;
      for (auto &Bb : F) {
        for (auto &Ins : Bb) {
          if (auto *Call = dyn_cast<CallBase>(&Ins)) {
            if (!Call->isIndirectCall())
              continue;
            // check metadata
            auto *MetaNode = Call->getMetadata("typegraph_node");
            if (MetaNode) {
              auto Callname = cast<MDString>(MetaNode->getOperand(0))->getString();
              // TODO we could assert here that all Callnames have the same type.
              CollectedCallNames.append(Callname.str());
              CollectedCallNames.push_back('\0');
              IRBuilder<> Builder(Call);
              auto *Ptr = Builder.CreateBitCast(Call->getCalledValue(), CharPtr);
              Builder.CreateCall(FuncCount, {IdentVar, ConstantInt::get(SizeT, NextCallId), Ptr});
              NextCallId++;
            }
          }
        }
      }
    }

    // Create "init" function and add to global_ctors
    auto *FT = FunctionType::get(Void, false);
    auto *F = cast<Function>(M.getOrInsertFunction("__collect_calltargets_init", FT).getCallee());
    F->setLinkage(GlobalValue::PrivateLinkage);
    addGlobalCtor(F);

    // Function to run on init
    auto *Bb = BasicBlock::Create(M.getContext(), "entry", F);
    IRBuilder<> Builder(Bb);
    // init calls
    auto *CollectedCallNamesStr = Builder.CreateGlobalStringPtr(CollectedCallNames);
    Builder.CreateCall(FuncInit, {CollectedCallNamesStr, ConstantInt::get(SizeT, NextCallId), IdentVar});
    // init functions
    for (auto &UsedFunction : M.getFunctionList()) {
      if (UsedFunction.hasAddressTaken() && UsedFunction.hasName()) {
        auto *Symbol = Builder.CreateGlobalStringPtr(UsedFunction.getName());
        Builder.CreateCall(FuncAdd, {Symbol, Builder.CreateBitCast(&UsedFunction, CharPtr)});
      }
    }
    Builder.CreateRetVoid();
  }

  void warnAboutExternalReachableCalls() {
    long CountIntern = 0;
    long CountExtern = 0;
    for (auto &F : M.functions()) {
      for (auto &Bb : F) {
        for (auto &Ins : Bb) {
          if (auto *Call = dyn_cast<CallBase>(&Ins)) {
            if (!Call->isIndirectCall())
              continue;
            // check metadata
            auto *MetaNode = Call->getMetadata("typegraph_node");
            if (MetaNode) {
              const auto *Callname = Graph.SymbolContainer->get(cast<MDString>(MetaNode->getOperand(0))->getString());
              auto It = Graph.CallInfos.find(Callname);
              if (It == Graph.CallInfos.end()) {
                llvm::errs() << "[WARN] Indirect call named \"" << *Callname << "\" has no vertex in graph!\n";
              } else {
                // Check vertex with function type
                if (Graph[It->second.V].External) {
                  llvm::errs() << "[INFO] Indirect call named \"" << *Callname << "\" has type externally reachable!\n";
                  CountExtern++;
                } else {
                  // TODO we could resolve this later without dynamic linking
                  // llvm::errs() << "[+] Indirect call named \"" << *Callname << "\" is internalized, "
                  //              << Graph[It->second].FunctionUses.size() << " possible targets!\n";
                  CountIntern++;
                }
              }
            }
          }
        }
      }
    }
    fprintf(stderr, "[=] %ld / %ld calls have been internalized (%.1f%%)\n", CountIntern, CountIntern + CountExtern,
            CountIntern ? CountIntern * 100.0 / (CountIntern + CountExtern) : 0.0);
  }

  const std::string *getCallNameForCall(llvm::CallBase *Call) {
    auto *MetaNode = Call->getMetadata("typegraph_node");
    if (!MetaNode) {
      return nullptr;
    }
    return Graph.SymbolContainer->get(cast<MDString>(MetaNode->getOperand(0))->getString());
  }

  std::vector<Vertex> getVertexForIndirectCall(llvm::CallBase *Call) {
    const auto *Callname = getCallNameForCall(Call);
    if (!Callname) return {};
    auto It = Graph.CallInfos.find(Callname);
    if (It == Graph.CallInfos.end()) {
      llvm::errs() << "[WARN] Indirect call named \"" << *Callname << "\" has no vertex in graph!\n";
      return {};
    }
    assert(It->getSecond().AllVertices.at(0) == It->getSecond().V);
    return It->getSecond().AllVertices;
  }

  SetCls<FunctionUsage> getFunctionUsesForIndirectCall(llvm::CallBase *Call) {
    SetCls<FunctionUsage> Result;
    for (auto V: getVertexForIndirectCall(Call)) {
      Result.insert(Graph[V].FunctionUses.begin(), Graph[V].FunctionUses.end());
    }
    return Result;
  }

  bool isArgnumCompatible(const llvm::CallBase *Call, const llvm::Function *Function) {
    auto CallSize = Call->getNumArgOperands();
    auto IsVarArg = Function->isVarArg() || (Call->getCalledValue()->getType()->isFunctionTy() && Call->getCalledValue()->getType()->isFunctionVarArg());
    if (!(Function->arg_size() == CallSize || (IsVarArg && Function->arg_size() < CallSize)))
      return false;
    for (size_t I = 0; I < Call->arg_size() && I < Function->arg_size(); I++) {
      auto *T1 = Call->getArgOperand(I)->getType();
      auto *T2 = Function->getArg(I)->getType();
      if (T1->isPointerTy() && T2->isPointerTy()) continue;
      if (T1->isIntegerTy() && T2->isIntegerTy()) continue;
      if (T1->isFloatingPointTy() && T2->isFloatingPointTy()) continue;
      if ((T1->isFloatingPointTy() && !T2->isFloatingPointTy()) ||
          (!T1->isFloatingPointTy() && T2->isFloatingPointTy()))
        return false;
      /*llvm::errs() << "BITSIZE " << T1->getPrimitiveSizeInBits() << " vs " << T2->getPrimitiveSizeInBits() << " => " << (T1->getPrimitiveSizeInBits() == T2->getPrimitiveSizeInBits()) << "\n";
      llvm::errs() << *T1 << " vs " << *T2 << "\n";
      if (T1->getPrimitiveSizeInBits() == T2->getPrimitiveSizeInBits()) continue;*/
      auto S1 = M.getDataLayout().getTypeSizeInBits(T1);
      auto S2 = M.getDataLayout().getTypeSizeInBits(T2);
      // llvm::errs() << "Check: " << S1 <<" " << S2 <<"   | " << *T1 << " vs " << *T2 << "\n";
      if (S1 == S2) continue;
      // llvm::errs() << "Incompatible types: " << *T1 << " vs " << *T2 << "\n";
      return false;
    }

    // Check return types
    if (!Call->getType()->isVoidTy()) {
      auto *T1 = Call->getType();
      auto *T2 = Function->getFunctionType()->getReturnType();
      if (T2->isVoidTy()) return false;
      if (T1->isIntegerTy() && T2->isIntegerTy()) return true;
      if (T1->isFloatingPointTy() && T2->isFloatingPointTy()) return true;
      if ((T1->isFloatingPointTy() && !T2->isFloatingPointTy()) ||
          (!T1->isFloatingPointTy() && T2->isFloatingPointTy()))
        return false;
      if (T1->isPointerTy() && T2->isPointerTy()) return true;
      auto S1 = M.getDataLayout().getTypeSizeInBits(T1);
      auto S2 = M.getDataLayout().getTypeSizeInBits(T2);
      if (S1 != S2) return false;
    } else if (Settings.consider_return_type) {
      if (!Function->getFunctionType()->getReturnType()->isVoidTy())
        return false;
    }

    return true;
  }

  void writeCallTargetOutput() {
    llvm::json::Object J;
    J["tg_targets"] = llvm::json::Object();
    J["tg_targets_hash"] = llvm::json::Object();
    J["tg_targets_argnum"] = llvm::json::Object();
    auto *CallMap = J.getObject("tg_targets");
    auto *CallMapHash = J.getObject("tg_targets_hash");
    auto *CallMapArgnum = J.getObject("tg_targets_argnum");

    for (auto &F : M.functions()) {
      for (auto &Bb : F) {
        for (auto &Ins : Bb) {
          if (auto *Call = dyn_cast<CallBase>(&Ins)) {
            if (!Call->isIndirectCall())
              continue;
            auto *TypegraphNode = Ins.getMetadata(LLVMContext::MD_typegraph_node);
            assert(TypegraphNode);
            auto CallName = cast<MDString>(TypegraphNode->getOperand(0))->getString();
            (*CallMap)[CallName] = llvm::json::Array();
            auto *Arr = CallMap->getArray(CallName);
            (*CallMapHash)[CallName] = llvm::json::Array();
            auto *HashArr = CallMapHash->getArray(CallName);
            (*CallMapArgnum)[CallName] = llvm::json::Array();
            auto *ArrArgNum = CallMapArgnum->getArray(CallName);
            for (auto &Ins: Bb) {
              HashArr->push_back(Ins.getOpcodeName());
            }
            for (auto &Use : getFunctionUsesForIndirectCall(Call)) {
              if (Use.Function) {
                Arr->push_back(Use.Function->getName());
                if (isArgnumCompatible(Call, Use.Function)) {
                  ArrArgNum->push_back(Use.Function->getName());
                }
              }
            }
            std::sort(Arr->begin(), Arr->end(), [](json::Value &V1, json::Value&V2) { return V1.getAsString().getValue() < V2.getAsString().getValue(); });
            std::sort(ArrArgNum->begin(), ArrArgNum->end(), [](json::Value &V1, json::Value&V2) { return V1.getAsString().getValue() < V2.getAsString().getValue(); });
          }
        }
      }
    }

    std::error_code EC;
    llvm::raw_fd_ostream File(Settings.tgcfi_output, EC);
    File << llvm::json::Value(std::move(J)) << "\n";
  }

  void addSimpleEnforcement() {
    std::vector<CallBase*> CallsToProtect;

    // Collect all indirect calls
    for (auto &F : M.functions()) {
      for (auto &Bb : F) {
        for (auto &Ins : Bb) {
          if (auto *Call = dyn_cast<CallBase>(&Ins)) {
            if (!Call->isIndirectCall())
              continue;
            // this call must be guarded
            CallsToProtect.push_back(Call);
          }
        }
      }
    }

    // One switch per indirect call, simply guarding the indirect call
    for (auto *Call: CallsToProtect) {
      auto *BB = Call->getParent();
      auto *BB2 = Call->getParent()->splitBasicBlock(Call);
      BB->getTerminator()->eraseFromParent();
      IRBuilder<> Builder(BB);

      auto *CV = Call->getCalledValue();
      for (auto &Use : getFunctionUsesForIndirectCall(Call)) {
        if (Use.Function) {
          if (Settings.enforce_argnum) {
            if (!isArgnumCompatible(Call, Use.Function)) {
              continue;
            }
          }
          // this function is a valid target
          auto *R = Builder.CreateICmpEQ(CV, castTo(Use.Function, CV->getType(), Builder));
          auto *NewBB = BasicBlock::Create(M.getContext(), "", BB->getParent());
          Builder.CreateCondBr(R, BB2, NewBB);
          Builder.SetInsertPoint(NewBB);
        } else {
          llvm::errs() << "Warning: Function without ref! Symbol \"" << *Use.SymbolName << "\" / context \"" << *Use.ContextName << "\"\n";
        }
      }

      Builder.CreateIntrinsic(Intrinsic::trap, {}, {});
      Builder.CreateUnreachable();
      // llvm::errs() << *Call->getFunction() << "\n\n";
    }
  }

  bool considerFunctionForEnforcement(Function &F) {
    // some functions in musl libc should go without instrumentation, they don't expect this / use inline assembly for things.
    // their function pointers come from readonly memory, so the resulting binary is still safe.
    StringRef Name = F.hasName() ? F.getName() : "";
    if (Name == "_dlstart_c" || Name == "__dls2" || Name == "__dls2b" || Name == "libc_start_init" || Name == "libc_exit_fini")
      return false;
    if (Settings.lld_is_shared && (Name == "do_init_fini" || Name == "libc_start_main_stage2" || Name == "__libc_exit_fini" || Name == "__funcs_on_exit"))
      return false;
    return true;
  }

  void addEnforcement(TypeGraph &MinifiedGraph) {
    LLVMDispatcherBuilder Builder(M.getContext(), M, Graph.SymbolContainer);

    if (Settings.dynlib_support && Settings.lld_is_shared) {
      size_t ModuleID = std::hash<std::string>{}(Settings.output_filename);
      ModuleID &= 0x7fffffffffffff00;
      ModuleID &= (1uL << Settings.enforce_id_bitwidth) - 1;
      if (!ModuleID)
        ModuleID = 0xff00;
      fprintf(stderr, "Module ID: %lu (=0x%lx)\n", ModuleID, ModuleID);
      Builder.setModuleID(ModuleID);
    }

    // Collect all indirect calls
    for (auto &F : M.functions()) {
      if (!considerFunctionForEnforcement(F))
        continue;

      for (auto &Bb : F) {
        for (auto &Ins : Bb) {
          if (auto *Call = dyn_cast<CallBase>(&Ins)) {
            if (!Call->isIndirectCall()) {
              auto *MetaNode = Call->getMetadata("typegraph_node");
              if (MetaNode && MetaNode->getNumOperands() > 1) {
                // this call has resolve points
                for (unsigned I = 1; I < MetaNode->getNumOperands(); I++) {
                  const auto *CN = Graph.SymbolContainer->get(cast<MDString>(MetaNode->getOperand(I))->getString());
                  auto RPInfos = Graph.CallInfos.find(CN);
                  if (RPInfos != Graph.CallInfos.end()) {
                    // add this resolve point & collect all its functions
                    auto &C = Builder.addResolvePoint(CN, Call, RPInfos->second.NumArgs);
                    C.VertexID = RPInfos->second.V;
                    SetCls<LLVMDispatcherBuilder::DFunction *> Functions;
                    for (auto V : RPInfos->second.AllVertices) {
                      if (Graph[V].External)
                        C.IsExternal = true;
                      for (auto &Use : Graph[V].FunctionUses) {
                        if (Use.Function == nullptr)
                          continue;
                        Functions.insert(Builder.getFunction(Use.Function));
                      }
                    }
                    for (auto *Func : Functions) {
                      C.Targets.push_back(Func);
                    }
                  }
                }
              }
              MetaNode = Call->getMetadata("typegraph_dlsym");
              if (MetaNode && MetaNode->getNumOperands() == 1) {
                // mark to add call that converts result to ID
                auto DlSymName = cast<MDString>(MetaNode->getOperand(0))->getString();
                Builder.addDynamicSymbolCall(Call, Graph.SymbolContainer->get(DlSymName));
              }
              continue;
            }

            // this call must be guarded
            const auto *CN = getCallNameForCall(Call);
            if (!CN) continue;

            auto &C = Builder.addCall(CN, Call);
            for (auto V: getVertexForIndirectCall(Call)) {
              C.VertexID = V;
              if (Graph[V].External) {
                C.IsExternal = true;
                break;
              }
            }
            for (auto &Use : getFunctionUsesForIndirectCall(Call)) {
              if (Use.IsDynamicFunction) {
                C.IsExternal = true;
                continue;
              }
              if (Use.Function == nullptr) continue;
              if (!isArgnumCompatible(Call, Use.Function)) continue;
              auto *Func = Builder.getFunction(Use.Function);
              if (std::find(C.Targets.begin(), C.Targets.end(), Func) == C.Targets.end()) {
                C.Targets.push_back(Func);
              }
            }
          }
        }
      }
    }

    // Collect all external functions (and mark them as "leaking")
    for (auto V: Graph.vertex_set()) {
      if (Graph[V].External) {
        for (auto &FU: Graph[V].FunctionUses) {
          Builder.getFunction(FU.Function)->Leaking = true;
        }
      }
    }

    // Collect resolve points for the libc
    // TODO

    // Build enforcement
    Builder.initialize();
    Builder.assignOptimalIDs();
    Builder.assignMissingIDs();
    Builder.debugIDAssignment();
    Builder.replaceFunctionsWithIDs();
    Builder.generateCodeForAll();
    if (Settings.dynlib_support) {
      Builder.generateCodeForDynamicSymbols();
      Builder.allTrampolinesGenerated();
    }

    Builder.printFunctionIDs();
    // llvm::outs() << M << "\n\n\n";

    if (Settings.dynlib_support) {
      // define the dynlib interface
      auto *Void = Type::getVoidTy(M.getContext());
      auto *PtrType = Type::getInt8PtrTy(M.getContext());
      auto *LongType = Type::getInt64Ty(M.getContext());
      auto RegisterGraph = M.getOrInsertFunction("__tg_register_graph", Void, PtrType, PtrType, LongType);

      // initialize dynlib handlers
      auto *InitFunction = cast<Function>(M.getOrInsertFunction("__tg_init", Void).getCallee());
      InitFunction->setLinkage(GlobalValue::PrivateLinkage);
      auto *BB = BasicBlock::Create(M.getContext(), "entry", InitFunction);
      IRBuilder<> IRBuilder(BB);
      std::vector<llvm::Value *> SerializedGraph = WriteTypegraphToConstant(MinifiedGraph, M, Builder);
      IRBuilder.CreateCall(RegisterGraph, SerializedGraph);
      IRBuilder.CreateRetVoid();

      // at startup
      addGlobalCtor(InitFunction);
    }

    if (Settings.llvm_output) {
      std::error_code Code;
      raw_fd_ostream Stream(Settings.llvm_output, Code);
      Stream << M;
      Stream.close();
    }
  }

  void reportSettings() {
    llvm::errs() << "[Setting] TG_PROTECTED_LIBC = " << Settings.protected_libc << "\n";
    llvm::errs() << "[Setting] TG_INSTRUMENT_COLLECTCALLTARGETS = " << Settings.instrument_collectcalltargets << "\n";
    llvm::errs() << "[Setting] TG_LINKTIME_LAYERING = " << Settings.linktime_layering << "\n";
    llvm::errs() << "[Setting] TG_ENFORCE = " << Settings.enforce << "\n";
    llvm::errs() << "[Setting] TG_DYNLIB_SUPPORT = " << Settings.dynlib_support << "\n";
    llvm::errs() << "[Setting] TG_ENFORCE_ID_BITWIDTH = " << Settings.enforce_id_bitwidth << "\n";
    llvm::errs() << "[Setting] TG_ENFORCE_DISPATCHER_LIMIT = " << Settings.enforce_dispatcher_limit << "\n";
    llvm::errs() << "[Setting] TG_ENFORCE_SIMPLE = " << Settings.enforce_simple << "\n";
    llvm::errs() << "[Setting] TG_ENFORCE_ARGNUM = " << Settings.enforce_argnum << "\n";
  }

  void dumpLLVM(StringRef fname) {
    std::error_code code;
    raw_fd_ostream stream(fname, code);
    stream << M;
    stream.close();
  }

  void autodetectSettings() {
    Settings.link_with_libc = M.getFunction("libc_start_init") != nullptr;
  }

  bool run() {
    if (!Settings.enabled) {
      llvm::errs() << "[Setting] TG_ENABLED = false\n";
      return false;
    }
    autodetectSettings();
    reportSettings();
    CheckGraphForIntegrity(Graph, M);
    TimeClock C;

    Graph.computeInterfaceRelations();

    // Layering
    if (Settings.linktime_layering) {
      applyLinktimeLayering(Graph);
    }

    // mark external nodes
    if (Settings.dynlib_support && Settings.enforce) {
      markExternalInterfaceNodes(Graph, false, false, true);
    } else {
      markImportantInterfaceNodes(Graph);
    }
    C.report("1. mark");
    
    llvm::errs() << "       Basis: " << Graph.num_vertices() << " nodes, " << Graph.num_edges() << " edges\n";
    Graph.combineEquivalencesInline(true, false);
    C.report("2. equivalences (inline)");
    llvm::errs() << "       Equiv: " << Graph.num_vertices() << " nodes, " << Graph.num_edges() << " edges\n";

    if (Settings.facts_output) {
      Graph.computeFunctionUsesDebug(Settings.facts_output);
    }
    if (auto *Path = Settings.graph_output) {
      C.stop();
      Graph.saveToFile(std::string(Path) + ".equivalent.typegraph");
      C.cont();
    }

    Graph.computeReachability(true, Settings.dynlib_support);
    // Graph.computeReachabilityInline(true, true, false);
    C.report("3. reachability");
    if (Settings.enforce && Settings.dynlib_support) {
      Graph.combineEquivalencesInline(true, false, false);
    }

    auto MinifiedGraph = Graph.getMinifiedGraph(true);
    C.report("4. minify");
    llvm::errs() << "       Min:   " << MinifiedGraph->num_vertices() << " nodes, " << MinifiedGraph->num_edges()
                 << " edges\n";
    C.report("equivalences / function uses / closure computation");
    MinifiedGraph->CallGraph = MinifiedGraph->CallGraph->buildStronglyConnectedComponents();

    // now we can use the graph
    warnAboutExternalReachableCalls();

    if (Settings.tgcfi_output) {
      writeCallTargetOutput();
    }

    if (Settings.instrument_collectcalltargets) {
      C.restart();
      instrumentCollectCalltargets();
      C.report("Collected Call Targets instrumentation");
    } else if (Settings.enforce_simple) {
      addSimpleEnforcement();
    } else if (Settings.enforce) {
      C.report("(before enforcement)");
      addEnforcement(*MinifiedGraph);
      C.report("Enforcing call targets");
    }

    if (auto *Path = Settings.graph_output) {
      C.restart();
      MinifiedGraph->saveToFile(std::string(Path) + ".min.typegraph");
      Graph.saveToFile(std::string(Path) + ".after.typegraph");
      C.report("graph export to file");
    }
    return Settings.enforce || Settings.enforce_simple || Settings.instrument_collectcalltargets;
  }
};

PreservedAnalyses TypeGraphPass::run(Module &M, ModuleAnalysisManager &MAM) {
  return TypeGraphPassInternal(M).run() ? PreservedAnalyses::none() : PreservedAnalyses::all();
}

bool TypeGraphLegacyPass::runOnModule(Module &M) { return TypeGraphPassInternal(M).run(); }

char TypeGraphLegacyPass::ID = 0;

TypeGraphLegacyPass::TypeGraphLegacyPass() : ModulePass(ID) {}

void TypeGraphLegacyPass::getAnalysisUsage(AnalysisUsage &AU) const {}

static RegisterPass<TypeGraphLegacyPass> Registration("TypeGraph", "TypeGraphLegacyPass", false, false);

} // namespace llvm
