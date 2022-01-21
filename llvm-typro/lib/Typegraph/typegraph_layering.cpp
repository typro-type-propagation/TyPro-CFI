#include "llvm/Typegraph/typegraph_layering.h"
#include "TypegraphFormatters.h"
#include <llvm/Support/raw_ostream.h>
#include <llvm/Typegraph/TGCallGraph.h>
#include <llvm/Typegraph/TypegraphSettings.h>
#include <queue>

namespace typegraph {

struct QueueEntry {
  const Vertex oldV;
  const Vertex newV;

  QueueEntry(const Vertex oldV, const Vertex newV) : oldV(oldV), newV(newV) {}
};

struct LayeringEngine {
  TypeGraph &Graph;
  std::queue<QueueEntry> Q;
  std::vector<Edge> EdgesToRemove;
  long CountDuplicated = 0;

  LayeringEngine(TypeGraph &Graph) : Graph(Graph) {}

  virtual bool isAContextToDuplicate(Vertex V) = 0;

  virtual bool isAnInterfaceToDuplicate(const std::string *InterfaceName) = 0;

  virtual int getLayerForReferencingContext(Vertex V) = 0;

  Vertex getDuplicatedVertex(Vertex V, int Layer) {
    assert(!Graph[V].ContextIsGlobal && "Context must not be global!");
    assert(Layer > 0);

    auto NewVertex = Graph.getVertexOpt(Graph[V].Type, Graph[V].Context, Layer);
    if (NewVertex != Graph.NO_VERTEX) return NewVertex;
    NewVertex = Graph.createVertex(Graph[V].Type, Graph[V].Context, Graph[V].ContextIsGlobal, Layer);
    for (auto &TCP: Graph[V].AdditionalNames) {
      Graph[NewVertex].AdditionalNames.push_back(TypeContextPair(TCP.Type, TCP.Context, Layer, TCP.ContextIsGlobal));
    }
    Graph[NewVertex].External = Graph[V].External;
    Graph[NewVertex].HasIndirectCalls = Graph[V].HasIndirectCalls;
    Graph[NewVertex].FunctionUses.insert(Graph[V].FunctionUses.begin(), Graph[V].FunctionUses.end());

    if (Graph[V].HasIndirectCalls) {
      for (auto &It: Graph.CallInfos) {
        for (auto CallVertex: It.second.AllVertices) {
          if (CallVertex == V) {
            It.second.addVertex(NewVertex);
            break;
          }
        }
      }
    }

    Q.emplace(V, NewVertex);
    CountDuplicated++;

    return NewVertex;
  }

  void handleEdge(Edge& E, Vertex VOld, Vertex VNew, Vertex VOther, bool IsInEdge) {
    // Edges from/to globals must be retained in all layers (=> copied)
    if (Graph[VOther].ContextIsGlobal) {
      if (IsInEdge) Graph.add_edge(VOther, VNew, Graph[E]);
      else Graph.add_edge(VNew, VOther, Graph[E]);
    } else if (isAContextToDuplicate(VOther)) {
      // Edges from/to another vertex within the same group is cloned to the new layer
      Vertex VOtherNew = getDuplicatedVertex(VOther, Graph[VNew].Layer);
      if (IsInEdge) Graph.add_edge(VOtherNew, VNew, Graph[E]);
      else Graph.add_edge(VNew, VOtherNew, Graph[E]);
    } else {
      // Edges from/to another group can be:
      int Layer = getLayerForReferencingContext(VOther);
      if (Layer == Graph[VNew].Layer) {
        // - moved over to the layer currently created
        if (IsInEdge) {
          Graph.add_edge(VOther, VNew, Graph[E]);
        } else {
          Graph.add_edge(VNew, VOther, Graph[E]);
        }
        EdgesToRemove.push_back(E);
      } else if (Layer == 0) {
        // - copied to also include the new layer
        if (IsInEdge) Graph.add_edge(VOther, VNew, Graph[E]);
        else Graph.add_edge(VNew, VOther, Graph[E]);
      } else {
        // ignored (Layer < 0).
      }
    }
  }

  void progressQueue() {
    while (!Q.empty()) {
      auto Entry = Q.front();
      Q.pop();

      for (auto &E: Graph.in_edges(Entry.oldV)) {
        handleEdge(E, Entry.oldV, Entry.newV, E.source, true);
      }
      for (auto &E: Graph.out_edges(Entry.oldV)) {
        handleEdge(E, Entry.oldV, Entry.newV, E.target, false);
      }
      for (auto &E: EdgesToRemove) {
        Graph.remove_edge(E);
      }
      EdgesToRemove.clear();

    }
  }

  /**
   * For each interface included in the to-duplicate set, we duplicate the interface vertices, without touching any
   * edge (for now). Afterwards, referenced interface vertices are in the queue.
   */
  void duplicateInterfaces() {
    for (auto &It: Graph.Interfaces) {
      if (isAnInterfaceToDuplicate(It.first)) {
        for (auto &Desc: It.second) {
          if (!Desc.IsFunction) continue;
          for (auto &V: Desc.Types) {
            if (V == Graph.NO_VERTEX) continue;
            for (auto &E: Graph.in_edges(V)) {
              if (!isAContextToDuplicate(E.source)) {
                int Layer = getLayerForReferencingContext(E.source);
                if (Layer > 0) {
                  getDuplicatedVertex(V, Layer);
                }
              }
            }
            for (auto &E: Graph.out_edges(V)) {
              if (!isAContextToDuplicate(E.target)) {
                int Layer = getLayerForReferencingContext(E.target);
                if (Layer > 0) {
                  getDuplicatedVertex(V, Layer);
                }
              }
            }
          }
        }
      }
    }
  }

  void runLayering() {
    duplicateInterfaces();
    progressQueue();
  }

};


/**
 * A layering engine that duplicates a set of functions - once for any related function
 */
struct SimpleLayeringEngine : public LayeringEngine {
  llvm::DenseSet<const std::string *> ToDuplicate;
  llvm::DenseMap<const std::string *, int> RefToLayer;

  SimpleLayeringEngine(TypeGraph &Graph) : LayeringEngine(Graph) {}

  bool isAContextToDuplicate(Vertex V) override {
    if (Graph[V].Context->substr(0, 5) == "call#")
      return true;
    return ToDuplicate.find(Graph[V].Context) != ToDuplicate.end();
  }

  int getLayerForReferencingContext(Vertex V) override {
    auto It = RefToLayer.find(Graph[V].Context);
    if (It != RefToLayer.end())
      return It->second;
    return RefToLayer[Graph[V].Context] = Graph.NextLayer++;
  }

  bool isAnInterfaceToDuplicate(const std::string *InterfaceName) override {
    return ToDuplicate.find(InterfaceName) != ToDuplicate.end();
  }

  /*
  void duplicateInterfaces() {
    for (const std::string *Name: UnitsToDuplicate) {
      auto It = Graph.Interfaces.find(Name);
      if (It != Graph.Interfaces.end()) {
        for (auto &Desc: It->second) {
          for (auto &V: Desc.Types) {
            if (V == Graph.NO_VERTEX) continue;
            for (auto &E: Graph.in_edges(V)) {
              if (!isAContextToDuplicate(E.source)) {
                getDuplicatedVertex(V, getLayerForReferencingContext(E.source));
              }
            }
            for (auto &E: Graph.out_edges(V)) {
              if (!isAContextToDuplicate(E.target)) {
                getDuplicatedVertex(V, getLayerForReferencingContext(E.target));
              }
            }
          }
        }
      }
    }
  }
   */

  void run() {
    llvm::errs() << "BEFORE LAYERING: " << Graph.num_vertices() << "v " << Graph.num_edges() << "e\n";
    runLayering();
    llvm::errs() << "AFTER LAYERING:  " << Graph.num_vertices() << "v " << Graph.num_edges() << "e  (" << CountDuplicated << " vertices duplicated)\n";
  }
};




/**
 * A layering engine that duplicates a set of functions - once for any related function
 */
struct LinkUnitLayeringEngine : public LayeringEngine {
  llvm::DenseSet<const std::string *> UnitsToDuplicate;
  llvm::DenseMap<const std::string *, int> RefToLayer;

  LinkUnitLayeringEngine(TypeGraph &Graph) : LayeringEngine(Graph) {}

  bool isAContextToDuplicate(Vertex V) override {
    if (Graph[V].Context->substr(0, 5) == "call#")
      return true;
    auto It = Graph.ContextDefiningUnits.find(Graph[V].Context);
    if (It == Graph.ContextDefiningUnits.end())
      return false;
    return UnitsToDuplicate.find(It->second) != UnitsToDuplicate.end();
  }

  int getLayerForReferencingContext(Vertex V) override {
    auto It = Graph.ContextDefiningUnits.find(Graph[V].Context);
    if (It == Graph.ContextDefiningUnits.end())
      return 0;
    auto It2 = RefToLayer.find(It->first);
    if (It2 != RefToLayer.end())
      return It2->second;
    return RefToLayer[It->first] = Graph.NextLayer++;
  }

  bool isAnInterfaceToDuplicate(const std::string *InterfaceName) override {
    auto It = Graph.ContextDefiningUnits.find(InterfaceName);
    if (It == Graph.ContextDefiningUnits.end())
      return false;
    return UnitsToDuplicate.find(It->first) != UnitsToDuplicate.end();
  }

  void run() {
    llvm::errs() << "BEFORE LAYERING: " << Graph.num_vertices() << "v " << Graph.num_edges() << "e\n";
    runLayering();
    llvm::errs() << "AFTER LAYERING:  " << Graph.num_vertices() << "v " << Graph.num_edges() << "e  (" << CountDuplicated << " vertices duplicated)\n";
  }
};


/**
 * Callgraph-based layering.
 * Pay attention: Defined and declared functions must never be merged into one SCC!
 */
struct CGLayeringEngine : public LayeringEngine {
  int NumberOfLayersToDuplicate = 0;

  TGLayeredCallGraph CG;

  CGLayeringEngine(TypeGraph &Graph) : LayeringEngine(Graph), CG(Graph.SymbolContainer) {}

  virtual void init() {
    CG.importUnlayeredGraph(*Graph.CallGraph);
    CG.optimizeGraph();
    CG.computeDepth();
  }

  CGSCC CurrentSCC;
  llvm::DenseMap<CGSCC, int> RefToLayer; // old CG vertex => layer

  bool isAnSCCToDuplicate(CGSCC SCC) {
    return CG[SCC].Depth < NumberOfLayersToDuplicate;
    switch (CG[SCC].Depth) {
    case 0: return true;
    // case 1: return CG.out_edges(SCC).size() < 15;
    // case 2: return CG.out_edges(SCC).size() < 3;
    case 1:
    case 2:
      //return true;
    //case 3:
      return CG.out_edges(SCC).size() < 15;
    default:
      return false;
    }
  }

  bool isAContextToDuplicate(Vertex V) override {
    return CG.getVertexOpt({Graph[V].Context, Graph[V].Layer}) == CurrentSCC;
  }
  bool isAnInterfaceToDuplicate(const std::string *InterfaceName) override {
    for (auto &Interface: Graph.Interfaces[InterfaceName]) {
      // if (Interface.IsDefined && Interface.IsFunction) { llvm::errs() << " (defined " << *InterfaceName << ")\n"; }
      if (Interface.IsDefined && Interface.IsFunction && CG.getVertexOpt({Interface.ContextName, 0}) == CurrentSCC) {
        return true;
      }
    }
    return false;
  }
  /**
   * For edges with callers: return the new layer of the respective caller.
   * For edges with callees: return 0 - they will be duplicated later
   * For edges without caller/callee relation: return -1 - we ignore them for now, shouldn't really happen.
   * @param V
   * @return
   */
  int getLayerForReferencingContext(Vertex V) override {
    auto CGVertex = CG.getVertexOpt({Graph[V].Context, Graph[V].Layer});
    auto It = RefToLayer.find(CGVertex);
    if (It != RefToLayer.end())
      return It->second;

    if (CG.hasEdge(CGVertex, CurrentSCC)) {
      return RefToLayer[CGVertex] = Graph.NextLayer++;
    }
    if (CG.hasEdge(CurrentSCC, CGVertex)) {
      return RefToLayer[CGVertex] = 0;
    }
    return RefToLayer[CGVertex] = -1;
  }

  void adjustSCCGraph() {
    // duplicate vertices for each created layer, and move incoming edges
    SetCls<CGSCC> NewVertices;
    for (auto &RelatedVertexWithLayer : RefToLayer) {
      if (RelatedVertexWithLayer.second > 0) {
        // llvm::errs() << "- Duplicated " << RelatedVertexWithLayer.first << " => " << CurrentSCC << " (" << CG[RelatedVertexWithLayer.first].Contexts.size() << "\"" << *CG[RelatedVertexWithLayer.first].Contexts.begin()->first << "\" -> " << CG[CurrentSCC].Contexts.size() << "\"" << *CG[CurrentSCC].Contexts.begin()->first << "\")\n";
        // incoming edge (.first => current) to new layer
        auto LayeredV = CG.getLayeredVertex(CurrentSCC, RelatedVertexWithLayer.second);
        NewVertices.insert(LayeredV);
        // move incoming edges
        CG.remove_edge({RelatedVertexWithLayer.first, CurrentSCC, {}});
        CG.add_edge(RelatedVertexWithLayer.first, LayeredV, {});
      }
    }
    // For all these new vertices, create outgoing edges again
    for (auto &RelatedVertexWithLayer : RefToLayer) {
      if (RelatedVertexWithLayer.second == 0) {
        // outgoing edge (current => .first)
        for (auto NewV: NewVertices) {
          CG.add_edge(NewV, RelatedVertexWithLayer.first, {});
        }
      }
    }
    // clear history
    RefToLayer.clear();
  }

  void run() {
    if (CG.num_vertices() == 0) {
      init();
    }
    auto SCCs = CG.getOrderCallerBeforeCallee();
    for (CGSCC SCC: SCCs) {
      if (CG.in_edges(SCC).size() <= 1) continue;
      if (!isAnSCCToDuplicate(SCC)) continue;
      // llvm::errs() << "SCC: " << SCC << "\n";
      // for (auto C: CG[SCC].Contexts) llvm::errs() << " - " << *C.first << " " << C.second << "\n";
      // run only if all functions in SCC are defined!
      CurrentSCC = SCC;
      runLayering();
      // handle SCC-Graph adjustments
      if (CountDuplicated > 0) {
        adjustSCCGraph();
        CountDuplicated = 0;
      }
    }
  }
};


struct LinktimeCGLayeringEngine : public CGLayeringEngine {
  LinktimeCGLayeringEngine(TypeGraph &Graph) : CGLayeringEngine(Graph) {}

  void init() override {
    TGCallGraph TempCG = *Graph.CallGraph;

    if (Settings.linktime_layering_debug) {
      basegraph::dot_debug_graph(TempCG, CGNodeWriter(), CGEdgeWriter(),
                                 basegraph::default_node_filter<TGCallGraph, CGSCC>());
      TempCG.assert_integrity();
    }

    // all connected vertices from one compilation unit should be in one SCC,
    // so that things that have already been splitted are not splitted up again.
    // At this point, each SCC contains only functions from one compilation unit
    // (because SCC generation did not run after linking graphs together yet)
    std::queue<CGSCC> Q;
    for (auto V: TempCG.vertex_set()) {
      Q.push(V);
    }
    while (!Q.empty()) {
      CGSCC V = Q.front();
      Q.pop();
      if (TempCG.vertices[V].empty)
        continue;
      auto It1 = Graph.ContextDefiningUnits.find(*TempCG[V].Contexts.begin());
      if (It1 == Graph.ContextDefiningUnits.end())
        continue;

      std::vector<CGSCC> JoinVertices;
      for (auto &E: TempCG.out_edges(V)) {
        if (E.source == E.target) continue;
        auto It2 = Graph.ContextDefiningUnits.find(*TempCG[E.target].Contexts.begin());
        if (It2 != Graph.ContextDefiningUnits.end() && It1->second == It2->second) {
          JoinVertices.push_back(E.target);
        }
      }
      if (!JoinVertices.empty()) {
        for (auto V2: JoinVertices) {
          TempCG.joinSCCs(V, V2);
        }
        Q.push(V);
      }
    }

    if (Settings.linktime_layering_debug) {
      basegraph::dot_debug_graph(TempCG, CGNodeWriter(), CGEdgeWriter(),
                                 basegraph::default_node_filter<TGCallGraph, CGSCC>());
      TempCG.assert_integrity();
    }

    auto TempCG2 = TempCG.buildStronglyConnectedComponents();
    if (Settings.linktime_layering_debug) {
      basegraph::dot_debug_graph(*TempCG2, CGNodeWriter(), CGEdgeWriter(),
                                 basegraph::default_node_filter<TGCallGraph, CGSCC>());
      TempCG2->assert_integrity();
    }

    llvm::errs() << "LINK SCC SIZES: " << Graph.CallGraph->num_vertices() << " -> " << TempCG.num_vertices() << " -> " << TempCG2->num_vertices() << "\n";
    CG.importUnlayeredGraph(*TempCG2);
    CG.optimizeGraph();
    CG.computeDepth();
  }
};




void applySimpleLayering(TypeGraph &Graph) {
  /*
  SimpleLayeringEngine ObjectFileEngine(Graph);
  ObjectFileEngine.ToDuplicate.insert(Graph.SymbolContainer->get("my_memcpy"));
  // ObjectFileEngine.ToDuplicate.insert(Graph.SymbolContainer->get("leak"));
  ObjectFileEngine.run();
   */
  llvm::errs() << "BEFORE LAYERING " << Graph.num_vertices() << " vertices  " << Graph.num_edges() << " edges\n";
  CGLayeringEngine ObjectFileEngine(Graph);
  ObjectFileEngine.NumberOfLayersToDuplicate = 0;
  ObjectFileEngine.run();
  llvm::errs() << "AFTER LAYERING  " << Graph.num_vertices() << " vertices  " << Graph.num_edges() << " edges  (with config " << ObjectFileEngine.NumberOfLayersToDuplicate << ")\n";
}


/**
 * Do NOT run SCC-build on combined callgraph before applying linktime layering!
 * @param Graph
 */
void applyLinktimeLayering(TypeGraph &Graph) {
  llvm::errs() << "BEFORE LINK LAYERING " << Graph.num_vertices() << " vertices  " << Graph.num_edges() << " edges\n";
  LinktimeCGLayeringEngine LinktimeEngine(Graph);
  LinktimeEngine.NumberOfLayersToDuplicate = 0;
  LinktimeEngine.run();
  if (Settings.linktime_layering_debug) {
    basegraph::dot_debug_graph(LinktimeEngine.CG, CGLNodeWriter(), CGLEdgeWriter(),
                               basegraph::default_node_filter<TGLayeredCallGraph, CGSCC>());
  }
  llvm::errs() << "AFTER LINK LAYERING  " << Graph.num_vertices() << " vertices  " << Graph.num_edges() << " edges  (with config " << LinktimeEngine.NumberOfLayersToDuplicate <<")\n";
}

} // namespace typegraph