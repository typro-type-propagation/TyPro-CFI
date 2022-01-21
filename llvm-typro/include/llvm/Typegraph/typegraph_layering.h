#ifndef LLVM_TYPEGRAPHS_TYPEGRAPH_LAYERING_H
#define LLVM_TYPEGRAPHS_TYPEGRAPH_LAYERING_H

#include "Typegraph.h"

namespace typegraph {
void applySimpleLayering(TypeGraph &Graph);
void applyLinktimeLayering(TypeGraph &Graph);
}

#endif // LLVM_TYPEGRAPHS_TYPEGRAPH_LAYERING_H
