#ifndef LLVM_TYPEGRAPHS_PROTECTEDTYPEGRAPH_H
#define LLVM_TYPEGRAPHS_PROTECTEDTYPEGRAPH_H

#include "ProtectedAllocator.h"
#include <map>
#include <set>
#include <vector>

#define DefaultAlloc ProtectedAllocator

template <class K, class V> using MapCls = std::map<K, V, std::less<K>, ProtectedAllocator<std::pair<const K, V>>>;
template <class K> using SetCls = std::set<K, std::less<K>, ProtectedAllocator<K>>;
template <class V> using VectorCls = std::vector<V, ProtectedAllocator<V>>;


#endif // LLVM_TYPEGRAPHS_PROTECTEDTYPEGRAPH_H
