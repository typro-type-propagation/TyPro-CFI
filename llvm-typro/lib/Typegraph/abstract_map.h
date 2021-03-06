#ifndef TYPEGRAPH_ABSTRACT_MAP_H
#define TYPEGRAPH_ABSTRACT_MAP_H

#include <vector>

namespace basegraph {

template <class ValueType> class AbstractMap {
  typedef long unsigned int Vertex;
  std::vector<ValueType> values;

public:
  template <class Graph> explicit AbstractMap(const Graph &graph) : values(graph.capacity() + 1) {}

  AbstractMap(const AbstractMap &) = delete;

  inline void init(const ValueType t) { std::fill(values.begin(), values.end(), t); }

  inline void put(Vertex v, ValueType t) {
    assert(v < values.size());
    values[v] = t;
  }

  inline ValueType &get(Vertex v) {
#ifdef NDEBUG
    return values[v];
#else
    return values.at(v);
#endif
  }

  ValueType &operator[](Vertex v) {
    return values[v];
  }

  const ValueType &operator[](Vertex v) const {
    return values[v];
  }
};

} // namespace basegraph

#endif // TYPEGRAPH_ABSTRACT_MAP_H
