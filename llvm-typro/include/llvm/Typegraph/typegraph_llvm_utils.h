#ifndef LLVM_TYPEGRAPH_LLVM_IMPORT_H
#define LLVM_TYPEGRAPH_LLVM_IMPORT_H

#include "Typegraph.h"
#include "llvm/IR/Module.h"

namespace typegraph {

class LLVMDispatcherBuilder;

void ParseTypegraphFromMetadata(TypeGraph &graph, llvm::Module &M);

void WriteTypegraphToMetadata(TypeGraph &Graph, llvm::Module &M,
                              const std::string &unique_identifier);

int CheckGraphForIntegrity(TypeGraph &graph, llvm::Module &M);

void markExternalInterfaceNodes(TypeGraph &Graph, bool AllCallsAreExternal, bool AllFunctionUsesAreExternal, bool isWholeDSO);

void markImportantInterfaceNodes(TypeGraph &Graph);

std::vector<llvm::Value*> WriteTypegraphToConstant(TypeGraph &Graph, llvm::Module &M, LLVMDispatcherBuilder &DBuilder);

llvm::raw_ostream &operator<<(llvm::raw_ostream &OS, const TGNode &Node);

} // namespace typegraph

#endif // LLVM_TYPEGRAPH_LLVM_IMPORT_H
