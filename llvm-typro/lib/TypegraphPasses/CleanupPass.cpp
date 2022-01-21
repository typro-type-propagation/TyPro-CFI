#include "CleanupPass.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include <llvm/IR/Operator.h>

namespace llvm {

class CScanCleanupPassInternal {
  Module &M;

public:
  CScanCleanupPassInternal(Module &M) : M(M) {}

  bool isFunctionLike(Value *Op) {
    if (isa<Function>(Op)) return true;
    if (auto *A = dyn_cast<GlobalAlias>(Op)) {
      return isFunctionLike(A->getAliasee());
    }
    if (auto *COp = dyn_cast<BitCastOperator>(Op)) {
      return isFunctionLike(COp->getOperand(0));
    }
    return false;
  }

  bool run() {
    // llvm::errs() << "CleanupPass started.\n";
    bool Modified = false;

    // Cleanup cscan-instrumentation if necessary
    std::vector<Instruction*> ToRemove;
    for (auto &F: M.functions()) {
      for (auto &BB: F) {
        for (auto &Ins: BB) {
          if (auto *C = dyn_cast<CallInst>(&Ins)) {
            if (C->isInlineAsm() && C->getNumArgOperands() == 1) {
              if (auto *Asm = dyn_cast<InlineAsm>(C->getCalledValue())) {
                // llvm::errs() << "InlineAsm: " << *C << "   " << (Asm->getAsmString() == "movq $0, $0; int3;") << (isFunctionLike(C->getArgOperand(0))) << "   arg0=" << *C->getArgOperand(0) << "\n";
                // llvm::errs().flush();
                if (Asm->getAsmString() == "movq $0, $0; int3;" && isFunctionLike(C->getArgOperand(0))) {
                  llvm::errs() << "Removing CSCAN instrumentation: " << *C << "\n";
                  ToRemove.push_back(C);
                }
              }
            }
          }
        }
      }
    }
    Modified |= !ToRemove.empty();
    for (auto *I: ToRemove) {
      I->eraseFromParent();
    }

    return Modified;
  }
};

PreservedAnalyses CScanCleanupPass::run(Module &M, ModuleAnalysisManager &MAM) {
  return CScanCleanupPassInternal(M).run() ? PreservedAnalyses::none() : PreservedAnalyses::all();
}

bool CScanCleanupLegacyPass::runOnModule(Module &M) { return CScanCleanupPassInternal(M).run(); }

char CScanCleanupLegacyPass::ID = 0;

CScanCleanupLegacyPass::CScanCleanupLegacyPass() : ModulePass(ID) {}

void CScanCleanupLegacyPass::getAnalysisUsage(AnalysisUsage &AU) const {}

static RegisterPass<CScanCleanupLegacyPass> Registration("CScanCleanup", "CScanCleanupLegacyPass", false, false);

} // namespace llvm
