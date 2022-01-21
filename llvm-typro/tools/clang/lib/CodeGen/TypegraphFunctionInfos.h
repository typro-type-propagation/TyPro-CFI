#ifndef LLVM_TYPEGRAPHS_TYPEGRAPHFUNCTIONINFOS_H
#define LLVM_TYPEGRAPHS_TYPEGRAPHFUNCTIONINFOS_H

#include <vector>
#include <llvm/ADT/Triple.h>

namespace clang {

enum LibraryFunctionHandling {
  DEFAULT = 0,
  PER_CALL_CONTEXT,  // each call gets its own function context
  IGNORE,            // do not consider call at all
  RESOLVE,           // Arguments need FunctionID => FunctionPtr conversion
  RESOLVE_DEEP,      // Arguments need FunctionID => FunctionPtr conversion in struct/union parameters
  RESOLVE_WITH_DATA, // Arguments need FunctionID => FunctionPtr conversion, and further arguments transfer data to the indirect call
  DLSYM,             // Resolve dynamic symbols
  MEMCPY,            // memcpy-style functions: arg1--*-> [---->] <--*-- arg0
};

LibraryFunctionHandling GetHandlingForFunction(const char *Name);
const std::vector<int> &GetHandlingIndicesForFunction(const char *Name);
void TestHandlingForFunction();
void TypegraphFunctionsSetArch(llvm::Triple::ArchType A);

} // namespace clang

#endif // LLVM_TYPEGRAPHS_TYPEGRAPHFUNCTIONINFOS_H
