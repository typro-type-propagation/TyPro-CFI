#include "./TyproLibraryFixes.h"
#include "llvm/Typegraph/TypegraphSettings.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

using namespace typegraph;

namespace llvm {

    class TyproLibraryFixesPassInternal {
        Module &M;
        bool IsAfter;
        bool Changed = false;

    public:
        TyproLibraryFixesPassInternal(Module &M, bool IsAfter = false) : M(M), IsAfter(IsAfter) {}

        void GenerateDebugValue(StringRef S, Instruction* Inst) {
          auto *PtrType = Type::getInt8PtrTy(M.getContext());
          auto *Stderr = M.getOrInsertGlobal("stderr", PtrType);
          auto Fprintf = M.getOrInsertFunction("fprintf",
                                               FunctionType::get(Type::getInt32Ty(M.getContext()), {PtrType, PtrType}, true));
          llvm::IRBuilder<> Builder(Inst->getNextNode());
          llvm::errs() << *Fprintf.getCallee() << "\n";
          Builder.CreateCall(Fprintf, {Builder.CreateLoad(Stderr), Builder.CreateGlobalStringPtr(S), Inst});
          llvm::errs() << "[DEBUG] Generated debug value: '" << S << "' for " << *Inst << "\n";
          Changed = true;
        }

        bool run() {
          if (!IsAfter && !Settings.lld_is_shared) {
            // Hard-wire musl libc's call to main(), which is not required to be valid-typed
            // This is not required in general (Typro could infer this relationship automatically), but makes things faster:
            // many programs don't need any runtime computation now, because they don't exchange function pointers with libc except main.
            auto *F = M.getFunction("libc_start_main_stage2");
            if (F) {
              for (auto &BB: *F) {
                for (auto &Ins: BB) {
                  if (auto *C = dyn_cast<CallInst>(&Ins)) {
                    if (C->isIndirectCall()) {
                      auto *Main = M.getFunction("main");
                      if (Main) {
                        auto *CallType = C->getCalledValue()->getType();
                        if (Main->getType() == CallType) {
                          C->setCalledFunction(Main);
                        } else {
                          C->setCalledOperand(ConstantExpr::getBitCast(Main, CallType));
                        }
                        Changed = true;
                      }
                    }
                  }
                }
              }
            }
          }

          if (IsAfter && Settings.lld_is_shared) {
            Changed |= muslAddInternalForkHandlerForOmp();
          }

          return Changed;
        }

        /**
         * Our dynamic version of musl includes (unprotected!) openmp runtime, which calls pthread_atfork.
         * To trigger these atfork handlers, we have to call _internal___fork_handler from __fork_handler.
         * @return
         */
        bool muslAddInternalForkHandlerForOmp() {
          if (auto *F = M.getFunction("__fork_handler")) {
            auto F2 = M.getOrInsertFunction("_internal___fork_handler", F->getFunctionType());
            IRBuilder<> Builder(&*F->getEntryBlock().begin());
            std::vector<Value *> Arguments;
            Arguments.push_back(F->arg_begin());
            Builder.CreateCall(F2, Arguments);
            return true;
          }
          return false;
        }
    };

    PreservedAnalyses TyproLibraryFixesPass::run(Module &M, ModuleAnalysisManager &MAM) {
      return TyproLibraryFixesPassInternal(M).run() ? PreservedAnalyses::none() : PreservedAnalyses::all();
    }

    bool TyproLibraryFixesLegacyPass::runOnModule(Module &M) { return TyproLibraryFixesPassInternal(M, isAfter).run(); }

    char TyproLibraryFixesLegacyPass::ID = 0;

    TyproLibraryFixesLegacyPass::TyproLibraryFixesLegacyPass() : ModulePass(ID), isAfter(false) {}
    TyproLibraryFixesLegacyPass::TyproLibraryFixesLegacyPass(bool IsAfter) : ModulePass(ID), isAfter(IsAfter) {}

    void TyproLibraryFixesLegacyPass::getAnalysisUsage(AnalysisUsage &AU) const {}

    static RegisterPass<TyproLibraryFixesLegacyPass> Registration("TyproLibraryFixes", "TyproLibraryFixesLegacyPass", false, false);

} // namespace llvm