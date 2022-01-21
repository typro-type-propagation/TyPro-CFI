#include "../CodeGen/TypegraphBuilder.h"

namespace clang {

__attribute__((weak))
void TypeGraphBuilder::addImplicitTypeCast(GlobalDecl &GD, const Expr *E, QualType DestTy) {
  assert(false && "Linking broken, link clangCodeGen.so");
}

__attribute__((weak))
thread_local TypeGraphBuilder *TypeGraphBuilder::CurrentInstance = nullptr;

} // namespace clang
