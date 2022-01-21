#include "TypegraphPass.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/Typegraph/Typegraph.h"
#include <llvm/Support/JSON.h>
#include <llvm/Typegraph/TypegraphSettings.h>

using namespace typegraph;

namespace llvm {

class RelatedWorkPassInternal {
  Module &M;
  TypeGraph Graph;

public:
  RelatedWorkPassInternal(Module &M) : M(M) {
    // Fix paths
    if (Settings.ifcc_output && std::string(Settings.ifcc_output) == "auto") {
      Settings.ifcc_output = (new std::string(Settings.output_filename + ".ifcc.json"))->c_str();
    }
    if (Settings.icfi_output && std::string(Settings.icfi_output) == "auto") {
      Settings.icfi_output = (new std::string(Settings.output_filename + ".icfi.json"))->c_str();
      setenv("TG_ICFI_OUTPUT", Settings.icfi_output, 1);
    }
    if (Settings.output_filename.substr(0, 9) == "conftest-") {
      Settings.icfi_output = nullptr;
      Settings.ifcc_output = nullptr;
    }
  }

  std::vector<Function *> getAddressTakenFunctions() {
    std::vector<Function *> Functions;
    for (auto &F : M.functions()) {
      if (!F.isIntrinsic() && F.hasAddressTaken())
        Functions.push_back(&F);
    }
    return Functions;
  }

  void writeIFCCOutput() {
    auto AddressTakenFunctions = getAddressTakenFunctions();
    llvm::json::Object J;
    J["ifcc_targets"] = llvm::json::Object();
    J["ifcc_targets_vararg"] = llvm::json::Object();
    auto *CallMap = J.getObject("ifcc_targets");
    auto *CallMapVararg = J.getObject("ifcc_targets_vararg");

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
            (*CallMapVararg)[CallName] = llvm::json::Array();
            auto *ArrVararg = CallMapVararg->getArray(CallName);

            auto CallSize = Call->getNumArgOperands();
            for (auto *F2 : AddressTakenFunctions) {
              if (F2->arg_size() == CallSize) {
                Arr->push_back(F2->getName());
                ArrVararg->push_back(F2->getName());
              } else if (F2->arg_size() < CallSize && F2->isVarArg()) {
                ArrVararg->push_back(F2->getName());
              }
            }
          }
        }
      }
    }

    std::error_code EC;
    llvm::raw_fd_ostream File(Settings.ifcc_output, EC);
    File << llvm::json::Value(std::move(J)) << "\n";
  }

  bool run() {
    auto &C = M.getContext();

    auto *Node = M.getOrInsertNamedMetadata("cfi-icall-data");

    if (Settings.icfi_output) {
      for (auto &F : M.functions()) {
        if (F.hasAddressTaken()) {
          F.setMetadata("address_taken", MDNode::get(C, {}));
          F.setLinkage(GlobalValue::ExternalLinkage);
        }

        for (auto &Bb : F) {
          for (auto &Ins : Bb) {
            if (auto *Call = dyn_cast<CallBase>(&Ins)) {
              if (!Call->isIndirectCall())
                continue;

              // icfi analysis
              if (Ins.hasMetadata(LLVMContext::MD_icfi_call_type)) {
                auto *TypegraphNode = Ins.getMetadata(LLVMContext::MD_typegraph_node);
                assert(TypegraphNode);
                Node->addOperand(llvm::MDNode::get(
                    C, {TypegraphNode->getOperand(0), Ins.getMetadata(LLVMContext::MD_icfi_call_type)->getOperand(0),
                        Ins.getMetadata(LLVMContext::MD_icfi_call_type)->getOperand(1),
                        Ins.getMetadata(LLVMContext::MD_icfi_call_type)->getOperand(2)}));
              }
            }
          }
        }
      }
    }

    if (Settings.ifcc_output) {
      writeIFCCOutput();
    }

    return false;
  }
};

PreservedAnalyses RelatedWorkPass::run(Module &M, ModuleAnalysisManager &MAM) {
  return RelatedWorkPassInternal(M).run() ? PreservedAnalyses::none() : PreservedAnalyses::all();
}

bool RelatedWorkLegacyPass::runOnModule(Module &M) { return RelatedWorkPassInternal(M).run(); }

char RelatedWorkLegacyPass::ID = 0;

RelatedWorkLegacyPass::RelatedWorkLegacyPass() : ModulePass(ID) {}

void RelatedWorkLegacyPass::getAnalysisUsage(AnalysisUsage &AU) const {}

static RegisterPass<RelatedWorkLegacyPass> Registration("RelatedWork", "RelatedWorkLegacyPass", false, false);

} // namespace llvm
