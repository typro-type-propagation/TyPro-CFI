#ifndef LLVM_CLEANUPPASS_H
#define LLVM_CLEANUPPASS_H

#include <llvm/IR/PassManager.h>
#include <utility>

namespace llvm {

struct CScanCleanupPass : public PassInfoMixin<CScanCleanupPass> {
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

struct CScanCleanupLegacyPass : public ModulePass {
  static char ID;

  CScanCleanupLegacyPass();

  bool runOnModule(Module &M) override;

  void getAnalysisUsage(AnalysisUsage &usage) const override;

};

}

#endif // LLVM_CLEANUPPASS_H
