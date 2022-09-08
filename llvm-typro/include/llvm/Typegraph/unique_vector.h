#ifndef LLVM_TYPEGRAPHS_UNIQUE_VECTOR_H
#define LLVM_TYPEGRAPHS_UNIQUE_VECTOR_H

#include "basicdefs.h"
#include <cstddef>
#include <vector>

namespace basegraph {

// Unique vector - a set implementation based on vectors (more space-efficient, fast enough for small sets)
template <typename Tp> class unique_vector {
public:
  typedef typename VectorCls<Tp>::size_type size_type;
  typedef typename VectorCls<Tp>::iterator iterator;
  typedef typename VectorCls<Tp>::const_iterator const_iterator;
  VectorCls<Tp> vec;

  void push_back(const Tp &x) { insert(x); }

  void push_back(Tp &&x) { insert(x); }

  /**
   *
   * @param x
   * @return true iff the edge did not exist before
   */
  bool insert(const Tp &x) {
    for (auto it = vec.begin(); it != vec.end(); it++)
      if (x == *it)
        return false;
    vec.push_back(x);
    return true;
  }

  bool insert(Tp &&x) {
    for (auto it = vec.begin(); it != vec.end(); it++)
      if (x == *it)
        return false;
    vec.push_back(x);
    return true;
  }

  bool insert_unique(const Tp &x) {
    vec.push_back(x);
    return true;
  }

  size_t size() const { return vec.size(); }

  iterator begin() { return vec.begin(); }

  iterator end() { return vec.end(); }

  const_iterator begin() const { return vec.begin(); }

  const_iterator end() const { return vec.end(); }

  bool remove(const Tp &x) {
    for (auto it = vec.begin(); it != vec.end(); ++it) {
      if (*it == x) {
        vec.erase(it);
        return true;
      }
    }
    return false;
  }

  void clear() { vec.clear(); }

  Tp &back() { return vec.back(); }

  inline Tp &operator[](size_t index) { return vec[index]; }

  bool replace(const Tp &x, const Tp &y) {
    bool foundX = false;
    bool foundY = false;
    for (auto it = vec.begin(); it != vec.end(); it++) {
      if (*it == x) {
        if (foundY){
          vec.erase(it);
          return true;
        }
        foundX = true;
        *it = y;
      } else if (*it == y) {
        if (foundX) {
          vec.erase(it);
          return true;
        }
        foundY = true;
      }
    }
    return foundX;
  }

  bool contains(const Tp& x) {
    for (auto it = vec.begin(); it != vec.end(); it++) {
      if (*it == x)
        return true;
    }
    return false;
  }
};

} // namespace basegraph

#endif // LLVM_TYPEGRAPHS_UNIQUE_VECTOR_H
