#ifndef LLVM_TYPRO_LIBRARYFIXES_PASS_H
#define LLVM_TYPRO_LIBRARYFIXES_PASS_H

#include <llvm/IR/PassManager.h>
#include <utility>

namespace llvm {

    struct TyproLibraryFixesPass : public PassInfoMixin<TyproLibraryFixesPass> {
        PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
    };

    struct TyproLibraryFixesLegacyPass : public ModulePass {
        static char ID;
        bool isAfter;

        TyproLibraryFixesLegacyPass();
        TyproLibraryFixesLegacyPass(bool isAfter);

        bool runOnModule(Module &M) override;

        void getAnalysisUsage(AnalysisUsage &usage) const override;

    };

} // namespace llvm

#endif // LLVM_TYPRO_LIBRARYFIXES_PASS_H