#ifndef LLVM_TYPEGRAPHS_BASEGRAPH_H
#define LLVM_TYPEGRAPHS_BASEGRAPH_H

#include "basicdefs.h"
#include <stack>
#include <cassert>
#include "unique_vector.h"

namespace basegraph {

template<typename Vertex, class EdgeProperty>
struct EdgeImplBase {
  Vertex source;
  Vertex target;
  EdgeProperty property;
};

template<typename Vertex, class EdgeProperty>
inline bool operator==(const EdgeImplBase<Vertex, EdgeProperty> &lhs, const EdgeImplBase<Vertex, EdgeProperty> &rhs) {
  return lhs.source == rhs.source && lhs.target == rhs.target && lhs.property == rhs.property;
}

template<typename Vertex, class VertexProperty, class EdgeProperty>
struct VertexImplBase {
  unique_vector<EdgeImplBase<Vertex, EdgeProperty>> out_edges;
  unique_vector<EdgeImplBase<Vertex, EdgeProperty>> in_edges;
  VertexProperty property;
  bool empty = false;
};

template<class VertexProperty, class EdgeProperty>
class BaseGraph {
public:
  typedef long int Vertex;
  typedef VertexImplBase<Vertex, VertexProperty, EdgeProperty> VertexImpl;
  typedef EdgeImplBase<Vertex, EdgeProperty> Edge;
  typedef typename unique_vector<Edge>::iterator EdgeIterator;

private:
  size_t size = 0;
  size_t edge_size = 0;
  size_t max_capacity = 0;
  std::stack<Vertex> free_vertices;

public:
  inline Vertex null_vertex() const {
    return -1;
  }

  class VertexIterator {
    const BaseGraph *graph;
    Vertex v;
  public:
    explicit VertexIterator(const BaseGraph *graph, Vertex v) : graph(graph), v(v) {
      skip_empty();
    }

    inline void skip_empty() {
      while (v < (Vertex) graph->vertices.size() && graph->vertices[v].empty) ++v;
    }

    const VertexIterator operator++(int) { return v++; }

    VertexIterator &operator++() {
      ++v;
      // skip empty/removed vertices
      skip_empty();
      return *this;
    }

    Vertex operator*() const { return v; }

    // pointer   operator->() const                    { return pos_; }
    // VertexIterator  operator+ (int v)   const { return pos_ + v; }
    bool operator==(const VertexIterator &rhs) const { return v == rhs.v; }

    bool operator!=(const VertexIterator &rhs) const { return v != rhs.v; }
  };

  class VertexSet {
    const BaseGraph *graph;
    VertexIterator _end;
  public:
    inline VertexSet(const BaseGraph *graph) : graph(graph), _end(VertexIterator(graph, graph->vertices.size())) {}

    inline auto begin() {
      return VertexIterator(graph, 0);
    }

    inline auto end() {
      return _end;
    }
  };

  // Storage: Vertex with adjacency lists
  VectorCls<VertexImpl> vertices;

  inline VertexSet vertex_set() const {
    return VertexSet(this);
  }

  // Compatibility - [] operators for property access
  inline VertexProperty &operator[](Vertex v) {
    return vertices[v].property;
  }

  inline EdgeProperty &operator[](Edge &edge) {
    return edge.property;
  }

  inline const EdgeProperty &operator[](const Edge &edge) const {
    return edge.property;
  }

  // Iterate edges
  inline auto &out_edges(Vertex v) {
    return vertices[v].out_edges;
  }

  inline const auto &out_edges(Vertex v) const {
    return vertices[v].out_edges;
  }

  inline auto &in_edges(Vertex v) {
    return vertices[v].in_edges;
  }

  // sizes
  inline size_t num_vertices() const {
    return size;
  }

  inline size_t num_edges() const {
    return edge_size;
  }

  /**
   * Capacity = number of elements in the vertices vector = highest vertex id -1.
   * @return
   */
  inline size_t capacity() const {
    return vertices.size();
  }

  inline size_t max_vertex_id() {
    return max_capacity;
  }

  // Add / remove vertices and edges

  inline Vertex add_vertex(VertexProperty prop) {
    size++;
    while (!free_vertices.empty()) {
      auto v = free_vertices.top();
      free_vertices.pop();
      if ((size_t) v <= vertices.size()) {
        vertices[v].empty = false;
        vertices[v].property = prop;
        return v;
      }
    }

    auto v = vertices.size();
    vertices.emplace_back();
    vertices[v].property = prop;
    return v;
  }

  inline bool add_edge(Vertex v1, Vertex v2, EdgeProperty edge) {
    assert(v1 >= 0 && v1 < (Vertex) vertices.size() && "v1 out of range");
    assert(!vertices[v1].empty && "v1 already deleted");
    assert(v2 >= 0 && v2 < (Vertex) vertices.size() && "v2 out of range");
    assert(!vertices[v2].empty && "v2 already deleted");
    Edge e{v1, v2, edge};
    if (vertices[v1].out_edges.insert(e) && vertices[v2].in_edges.insert(e)) {
      edge_size++;
      return true;
    } else {
      return false;
    }
  }

  inline void add_edge_unique(Vertex v1, Vertex v2, EdgeProperty edge) {
    Edge e{v1, v2, edge};
    if (vertices[v1].out_edges.insert_unique(e) && vertices[v2].in_edges.insert_unique(e))
      edge_size++;
  }

  inline void remove_vertex(Vertex v) {
    assert(v >= 0 && v < (Vertex) vertices.size() && "v out of range");
    assert(!vertices[v].empty && "v already deleted");
    edge_size -= vertices[v].in_edges.size();
    edge_size -= vertices[v].out_edges.size();
    for (const auto &e: vertices[v].in_edges) {
      if (e.source == v) {
        edge_size++; // self-loop: only one edge
      }      else {
        vertices[e.source].out_edges.remove(e); // TODO can be more efficient
      }
    }
    for (const auto &e: vertices[v].out_edges) {
      if (e.target != v)
        vertices[e.target].in_edges.remove(e); //TODO can be more efficient
    }
    vertices[v].in_edges.clear();
    vertices[v].out_edges.clear();
    vertices[v].empty = true;
    size--;
    free_vertices.push(v);
  }

  inline void remove_edge(Edge e) {
    if (vertices[e.source].out_edges.remove(e) & vertices[e.target].in_edges.remove(e))
      edge_size--;
  }

  /**
   * Edit all edges from/to vOld, so that they point to vNew instead.
   * @param vOld
   * @param vNew
   */
  inline void replace_vertex_uses(Vertex vOld, Vertex vNew) {
    assert(vOld >= 0 && vOld < (Vertex) vertices.size() && "vOld is out of range");
    assert(vNew >= 0 && vNew < (Vertex) vertices.size() && "vNew is out of range");
    assert(!vertices[vOld].empty && "vOld is deleted");
    assert(!vertices[vNew].empty && "vNew is deleted");
    edge_size -= vertices[vOld].in_edges.size();
    edge_size -= vertices[vOld].out_edges.size();

    for (const auto &e: vertices[vOld].in_edges) {
      if (e.source == vOld) {
        // self-loop
        add_edge(vNew, vNew, e.property);
        edge_size++;
      } else {
        Edge e2 = e;
        e2.target = vNew;
        auto x = vertices[e.source].out_edges.replace(e, e2);
        assert(x);
        assert(vertices[e.source].out_edges.contains(e2) && "replace failed!");
        if (vertices[vNew].in_edges.insert(e2))
          edge_size++;
      }
    }
    vertices[vOld].in_edges.clear();

    for (const auto &e: vertices[vOld].out_edges) {
      if (e.target != vOld) {
        Edge e2 = e;
        e2.source = vNew;
        auto x = vertices[e.target].in_edges.replace(e, e2);
        assert(x);
        if (vertices[vNew].out_edges.insert(e2)) {
          edge_size++;
        }
      }
    }
    vertices[vOld].out_edges.clear();
  }

  inline void assert_integrity() {
    auto limit = (Vertex) vertices.size();
    size_t count_vertices = 0;
    size_t count_edges = 0;
    for (auto V: vertex_set()) {
      count_vertices++;
      for (const auto &e: vertices[V].in_edges) {
        assert(e.target == V && "Integrity: in_edges contains wrong edge");
        assert(e.source >= 0 && e.source < limit && "Integrity: in_edges contains out-of-bounds vertex");
        assert(vertices[e.source].out_edges.contains(e));
      }
      for (const auto &e: vertices[V].out_edges) {
        assert(e.source == V && "Integrity: out_edges contains wrong edge");
        assert(e.target >= 0 && e.target < limit && "Integrity: out_edges contains out-of-bounds vertex");
        assert(vertices[e.target].in_edges.contains(e));
        count_edges++;
      }
    }
    assert(count_vertices == num_vertices() && "Wrong vertex size");
    assert(count_edges == num_edges() && "Wrong edge size");
  }
};

} // namespace basegraph

#endif // LLVM_TYPEGRAPHS_BASEGRAPH_H
