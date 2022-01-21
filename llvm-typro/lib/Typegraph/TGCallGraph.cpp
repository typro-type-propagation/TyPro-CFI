#include "llvm/Typegraph/TGCallGraph.h"
#include "scc_algorithms.h"
#include <sstream>
#ifndef WITHOUT_LLVM
#include <llvm/Support/raw_ostream.h>
#endif

namespace typegraph {

TGCallGraph::TGCallGraph() : TGCallGraphBase(std::make_shared<StringContainer>()) {}
TGCallGraph::TGCallGraph(std::shared_ptr<StringContainer> SymbolContainer) : TGCallGraphBase(SymbolContainer) {}

void TGCallGraph::joinSCCs(CGSCC S1, CGSCC S2) {
  auto &N1 = (*this)[S1];
  for (const std::string *C: (*this)[S2].Contexts) {
    N1.Contexts.insert(C);
    ContextToSCC[C] = S1;
  }
  (*this)[S2].Contexts.clear();
  replace_vertex_uses(S2, S1);
  remove_vertex(S2);
  // remove self edges S1->S1
  remove_edge({S1, S1, {}});
}

std::shared_ptr<TGCallGraph> TGCallGraph::buildStronglyConnectedComponents() {
  basegraph::ComponentMap Map(*this);
  long NumComponents = basegraph::strong_components(*this, Map);
  auto NewGraph = std::make_shared<TGCallGraph>(SymbolContainer);
  for (int I = 0; I < NumComponents; I++) {
    NewGraph->add_vertex({});
  }
  for (auto V: vertex_set()) {
    auto V2 = Map.get(V);
    assert(V2 >= 0 && V2 < NumComponents);
    (*NewGraph)[V2].Contexts.insert((*this)[V].Contexts.begin(), (*this)[V].Contexts.end());
    for (const auto *C: (*this)[V].Contexts) {
      NewGraph->ContextToSCC[C] = V2;
    }
    for (auto E: out_edges(V)) {
      auto V3 = Map.get(E.target);
      if (V2 != V3) NewGraph->add_edge(V2, V3, E.property);
    }
  }
  return NewGraph;
}

void TGCallGraph::parserAddSCC(int V, const std::string &Repr) {
  CGSCC NewV = add_vertex({});
  std::stringstream Ss(Repr);
  std::string S;
  while (getline(Ss, S, '\t')) {
    const auto *C = SymbolContainer->get(S);
    auto V2 = getVertexOpt(C);
    if (V2 == NO_VERTEX) {
      (*this)[NewV].Contexts.insert(C);
      ContextToSCC[C] = NewV;
    } else if (V2 != NewV) {
      joinSCCs(V2, NewV);
      NewV = V2;
    }
  }
  ParserMap[V] = NewV;
}

void TGCallGraph::parserAddEdge(int S, int T) {
  add_edge(ParserMap[S], ParserMap[T], {});
}

void TGCallGraph::parserReset() {
  ParserMap.clear();
}

void TGLayeredCallGraph::joinSCCs(CGSCC S1, CGSCC S2) {
  auto &N1 = (*this)[S1];
  for (const auto &C: (*this)[S2].Contexts) {
    N1.Contexts.insert(C);
    ContextToSCC[C] = S1;
  }
  (*this)[S2].Contexts.clear();
  remove_vertex(S2);
  // remove self edges S1->S1
  remove_edge({S1, S1, {}});
}

std::shared_ptr<TGLayeredCallGraph> TGLayeredCallGraph::buildStronglyConnectedComponents() {
  basegraph::ComponentMap Map(*this);
  long NumComponents = basegraph::strong_components(*this, Map);
  auto NewGraph = std::make_shared<TGLayeredCallGraph>(SymbolContainer);
  for (int I = 0; I < NumComponents; I++) {
    NewGraph->add_vertex({});
  }
  for (auto V: vertex_set()) {
    auto V2 = Map.get(V);
    assert(V2 >= 0 && V2 < NumComponents);
    (*NewGraph)[V2].Contexts.insert((*this)[V].Contexts.begin(), (*this)[V].Contexts.end());
    for (const auto &C: (*this)[V].Contexts) {
      NewGraph->ContextToSCC[C] = V2;
    }
    for (auto E: out_edges(V)) {
      auto V3 = Map.get(E.target);
      if (V2 != V3) NewGraph->add_edge(V2, V3, E.property);
    }
  }
  return NewGraph;
}

void TGLayeredCallGraph::importUnlayeredGraph(TGCallGraph &Graph) {
  basegraph::AbstractMap<CGSCC> OldToNew(Graph);
  for (auto V: Graph.vertex_set()) {
    auto V2 = OldToNew[V] = add_vertex({});
    for (const auto *C: Graph[V].Contexts) {
      (*this)[V2].Contexts.insert({C, 0});
      ContextToSCC[{C, 0}] = V2;
    }
  }
  for (auto V: Graph.vertex_set()) {
    for (auto E: Graph.out_edges(V)) {
      add_edge(OldToNew[E.source], OldToNew[E.target], E.property);
    }
  }
}

std::vector<CGSCC> TGLayeredCallGraph::getOrderCalleeBeforeCaller() {
  std::vector<CGSCC> SCCs;
  // Store SCCs (callee before caller)
  bottom_up_traversal(*this, [&SCCs](CGSCC V, TGLayeredCallGraph &Graph) {
    SCCs.push_back(V);
  });
  return SCCs;
}

std::vector<CGSCC> TGLayeredCallGraph::getOrderCallerBeforeCallee() {
  std::vector<CGSCC> SCCs;
  // Store SCCs (caller before callee)
  top_down_traversal(*this, [&SCCs](CGSCC V, TGLayeredCallGraph &Graph) {
    SCCs.push_back(V);
  });
  return SCCs;
}

bool TGLayeredCallGraph::hasEdge(CGSCC Source, CGSCC Target) {
  if (Source == NO_VERTEX || Target == NO_VERTEX) return false;
  return out_edges(Source).contains({Source, Target, {}});
}

CGSCC TGLayeredCallGraph::getLayeredVertex(CGSCC V, int Layer) {
  const std::string *FirstContext = (*this)[V].Contexts.begin()->first;
  auto V2 = getVertexOpt({FirstContext, Layer});
  if (V2 != NO_VERTEX)
    return V2;
  V2 = add_vertex({});
  for (auto &C: (*this)[V].Contexts) {
    (*this)[V2].Contexts.insert({C.first, Layer});
    ContextToSCC[{C.first, Layer}] = V2;
  }
  return V2;
}

void TGLayeredCallGraph::optimizeGraph() {
  // Detect 1:1 relations (and merge)
  for (auto V: vertex_set()) {
    if (out_edges(V).size() == 1) {
      auto E = *out_edges(V).begin();
      if (in_edges(E.target).size() == 1) {
        joinSCCs(V, E.target);
      }
    }
  }
}

void TGLayeredCallGraph::computeDepth() {
  bottom_up_traversal(*this, [](CGSCC V, TGLayeredCallGraph &Graph) {
    int Depth = 0;
    for (auto &E: Graph.out_edges(V)) {
      Depth = std::max(Depth, Graph[E.target].Depth + 1);
    }
    Graph[V].Depth = Depth;
  });
}

} // namespace typegraph