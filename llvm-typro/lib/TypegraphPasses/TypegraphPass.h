#ifndef LLVM_TYPEGRAPHPASS_H
#define LLVM_TYPEGRAPHPASS_H

#include <llvm/IR/PassManager.h>
#include <utility>

namespace llvm {

struct TypeGraphPass : public PassInfoMixin<TypeGraphPass> {
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

struct TypeGraphLegacyPass : public ModulePass {
  static char ID;

  TypeGraphLegacyPass();

  bool runOnModule(Module &M) override;

  void getAnalysisUsage(AnalysisUsage &usage) const override;

};

struct RelatedWorkPass : public PassInfoMixin<RelatedWorkPass> {
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

struct RelatedWorkLegacyPass : public ModulePass {
  static char ID;

  RelatedWorkLegacyPass();

  bool runOnModule(Module &M) override;

  void getAnalysisUsage(AnalysisUsage &usage) const override;

};

}

#endif // LLVM_TYPEGRAPHPASS_H
