#ifndef LLVM_TYPEGRAPHS_BASICDEFS_H
#define LLVM_TYPEGRAPHS_BASICDEFS_H

// Case 1: Protected allocator / runtime library only
#ifdef RUNTIME_CUSTOM_CLASSES
#include "runtime/ProtectedTypegraph.h"

#else
// Case 2+3: Default allocator
#define DefaultAlloc std::allocator

// Case 2: No LLVM given, use std::... containers
#ifdef WITHOUT_LLVM
#include <map>
#include <set>
#define MapCls std::map
#define SetCls std::set
#define VectorCls std::vector

// Case 3: LLVM given, use custom LLVM containers
#else
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/IR/Function.h"
#define MapCls llvm::DenseMap
#define SetCls llvm::DenseSet
#define VectorCls std::vector
#endif
#endif

#endif // LLVM_TYPEGRAPHS_BASICDEFS_H
