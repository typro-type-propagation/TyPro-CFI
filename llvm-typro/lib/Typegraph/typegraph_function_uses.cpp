#include "llvm/Typegraph/Typegraph.h"
#ifndef WITHOUT_LLVM
#include <llvm/Support/raw_ostream.h>
#endif
#include <llvm/Typegraph/timeclock.h>
#include <queue>
#include <algorithm>


namespace typegraph {

#define EdgeTypeNewCast EdgeType::CAST_SIMPLE
// #define EdgeTypeNewCast EdgeType::REACHABILITY

namespace {

struct VertexInfos {
  bool IsDirty = true;
  bool Enqueued = false;
  Vertex NewVertex = TypeGraph::NO_VERTEX;
  // Propagating stuff, collected from incoming edges. Any change to these fields makes the node dirty
  SetCls<Vertex> FunctionUsesFrom;
  SetCls<std::pair<int, Vertex>> IncomingStructMembers;
  SetCls<std::pair<const std::string *, Vertex>> IncomingUnionMembers;
  SetCls<Vertex> IncomingPointsTo;
  SetCls<Vertex> IncomingExternal;
  SetCls<Vertex> IncomingExternalPointsTo;
};

struct ReachabilityWorker {
  static constexpr Vertex NO_VERTEX = TypeGraph::NO_VERTEX;

  TypeGraph &Graph;
  bool StorePossibleFunctions;
  bool StoreReachability;
  bool KeepOnlyExternalNames = true;
  bool RespectDefiningUnits;

  std::queue<Vertex> Queue;
  std::vector<VertexInfos> Info;

  // record results
  long JoinedNodes = 0;

  // UTILITIES

  inline void enqueue(Vertex V, bool MakeDirty) {
    if (MakeDirty)
      Info[V].IsDirty = true;
    if (Info[V].IsDirty && !Info[V].Enqueued) {
      Queue.push(V);
      Info[V].Enqueued = true;
    }
  }

  inline bool addEdge(Vertex V1, Vertex V2, const TGEdge &E, bool Enqueue = true) {
    if (Graph.add_edge(V1, V2, E)) {
      if (Enqueue)
        enqueue(V1, true);
      return true;
    }
    return false;
  }

  /**
   * Get the (potential) new identity of a vertex. If V has been joined in the meantime, this function determines the
   * new vertex with this name.
   * @param V
   * @return
   */
  inline Vertex resolve(Vertex V) {
    if (V == NO_VERTEX)
      return NO_VERTEX;
    while (Info[V].NewVertex != NO_VERTEX) {
      V = Info[V].NewVertex;
    }
    return V;
  }

  ReachabilityWorker(TypeGraph &G, bool StorePossibleFunctions, bool StoreReachability, bool RespectDefiningUnits)
  : Graph(G), StorePossibleFunctions(StorePossibleFunctions), StoreReachability(StoreReachability), RespectDefiningUnits(RespectDefiningUnits) {
    // Initialize "Info"
    Info.resize(G.vertices.size(), VertexInfos());
    for (auto V : Graph.vertex_set()) {
      if (!Graph[V].FunctionUses.empty()) {
        Info[V].FunctionUsesFrom.insert(V);
        enqueue(V, true);

        // If this node also has indirect calls - handle them and insert edges! No need to actually process these edges.
        if (Graph[V].HasIndirectCalls) {
          for (const auto &FU : Graph[V].FunctionUses) {
            handleNewFunctionUse(V, FU, false);
          }
        }
      }
    }

    // Enqueue external nodes if necessary
    for (auto V : Graph.vertex_set()) {
      if (Graph[V].External) {
        newExternalNode(V);
        if (StoreReachability)
          enqueue(V, true);
      }
    }
  }

  // How to make nodes external
  void makeExternal(Vertex V) {
    assert(V != NO_VERTEX);
    assert(!Graph.vertices[V].empty && "Can't make missing vertex external!");
    if (Graph[V].External)
      return;

    Graph[V].External = true;
    // External(y) :- External(x), PointsTo(x, y).
    // External(y) :- External(x), StructMember(x, y, _).
    // External(y) :- External(x), UnionMember(x, y, _).
    for (const auto &E : Graph.out_edges(V)) {
      switch (Graph[E].Type) {
      case POINTS_TO:
      case STRUCT_MEMBER:
      case UNION_MEMBER:
        makeExternal(E.target);
        break;
      default:
        break;
      }
    }

    newExternalNode(V);

    if (StoreReachability) enqueue(V, true);
  }

  void newExternalNode(Vertex V) {
    // external -> calls: call context must be external
    if (Graph[V].HasIndirectCalls) {
      for (auto &CI : Graph.CallInfos) {
        // check: all calls that use this vertex
        if (std::find(CI.second.AllVertices.begin(), CI.second.AllVertices.end(), V) != CI.second.AllVertices.end()) {
          const auto *CallContext = CI.first;
          for (const auto &Pair : Graph.getTypeContextToVertex()) {
            // check: all nodes that have a call context that must be external
            if (Pair.first.Context == CallContext) {
              makeExternal(Pair.second);
            }
          }
        }
      }
    }

    // external function uses: function interface must be external
    for (auto &FU: Graph[V].FunctionUses) {
      auto It = Graph.PotentialExternalInterfaces.find(FU.ContextName);
      if (It != Graph.PotentialExternalInterfaces.end()) {
        for (auto V2: It->second) {
          makeExternal(V2);
        }
      }
    }
  }

  inline bool isJoinable(Vertex V) { return true; }

  inline bool isJoinable(Vertex V1, Vertex V2) {
    if (Graph[V1].ContextIsGlobal != Graph[V2].ContextIsGlobal)
      return false;
    /*if (RespectTypes) {
      if (Graph[V1].Type != Graph[V2].Type)
        return false;
    }*/
    if (RespectDefiningUnits) {
      const auto *U1 = Graph.getDefiningUnit(V1);
      const auto *U2 = Graph.getDefiningUnit(V2);
      return U1 && U2 && U1 == U2;
    }
    return true;
  }

  bool joinOrConnect(Vertex V1, Vertex V2, Vertex VMain) {
    if (V1 == V2)
      return false;
    assert(V2 != VMain && "Can't join a vertex that is currently being processed");
    if (Graph.vertices[V1].empty || Graph.vertices[V2].empty) {
      enqueue(VMain, true);
      return false;
    }

    if (!isJoinable(V1) || !isJoinable(V2) || !isJoinable(V1, V2)) {
      addEdge(V1, V2, TGEdge(EdgeTypeNewCast));
      addEdge(V2, V1, TGEdge(EdgeTypeNewCast));
      return false;
    }
    // llvm::errs() << "JOIN " << V1 << " <-- " << V2 << "\n";
    bool WasIndirectCallBefore = Graph[V1].HasIndirectCalls;
    bool WasExternalBefore = Graph[V1].External;
    Graph.joinVertex(V1, Graph[V2], KeepOnlyExternalNames);
    Graph.replace_vertex_uses(V2, V1);
    Graph.remove_vertex(V2);
    Info[V2].NewVertex = V1;
    // move other Info stuff
    Info[V1].IncomingStructMembers.insert(Info[V2].IncomingStructMembers.begin(), Info[V2].IncomingStructMembers.end());
    Info[V1].IncomingUnionMembers.insert(Info[V2].IncomingUnionMembers.begin(), Info[V2].IncomingUnionMembers.end());
    Info[V1].IncomingPointsTo.insert(Info[V2].IncomingPointsTo.begin(), Info[V2].IncomingPointsTo.end());
    Info[V1].IncomingExternal.insert(Info[V2].IncomingExternal.begin(), Info[V2].IncomingExternal.end());
    Info[V1].IncomingExternalPointsTo.insert(Info[V2].IncomingExternalPointsTo.begin(), Info[V2].IncomingExternalPointsTo.end());
    // Info[V1].FunctionUsesFrom.insert(Info[V2].FunctionUsesFrom.begin(), Info[V2].FunctionUsesFrom.end());
    if (Graph[V1].HasIndirectCalls && !WasIndirectCallBefore) {
      for (auto &FU: Info[V1].FunctionUsesFrom) {
        handleNewFunctionUses(V1, FU);
      }
    }
    for (const auto &FU : Info[V2].FunctionUsesFrom) {
      if (Info[V1].FunctionUsesFrom.insert(FU).second) {
        handleNewFunctionUses(V1, FU);
      }
    }
    if (Graph[V1].External && !WasExternalBefore)
      newExternalNode(V1);
    JoinedNodes++;
    enqueue(V1, true);
    return true;
  }

  void progressVertex(Vertex V) {
    Info[V].IsDirty = false;

    Vertex PointsTo = NO_VERTEX;
    MapCls<int, Vertex> StructMembers;
    MapCls<const std::string *, Vertex> UnionMembers;
    basegraph::unique_vector<Vertex> CastsTo;

    auto &OutEdges = Graph.out_edges(V);
    for (size_t I = 0; I < OutEdges.size(); I++) {
      const auto &E = OutEdges[I];
      switch (Graph[E].Type) {
      case CAST_SIMPLE:
      case REACHABILITY:
        if (E.target != V)
          CastsTo.insert(E.target);
        break;
      case POINTS_TO:
        if (PointsTo != NO_VERTEX) {
          if (E.target != V) {
            if (joinOrConnect(PointsTo, E.target, V))
              I--;
          } else {
            if (joinOrConnect(E.target, PointsTo, V))
              I--;
            PointsTo = V;
          }
        } else {
          PointsTo = E.target;
        }
        break;
      case STRUCT_MEMBER: {
        Vertex V2 = StructMembers.try_emplace(Graph[E].StructOffset, E.target).first->second;
        if (V2 != E.target) {
          if (E.target != V) {
            if (joinOrConnect(V2, E.target, V))
              I--;
          } else {
            if (joinOrConnect(E.target, V2, V)) {
              I--;
              StructMembers[Graph[E].StructOffset] = V2;
            }
          }
        }
        break;
      }
      case UNION_MEMBER: {
        Vertex V2 = UnionMembers.try_emplace(Graph[E].UnionType, E.target).first->second;
        if (V2 != E.target) {
          if (E.target != V) {
            if (joinOrConnect(V2, E.target, V))
              I--;
          } else {
            if (joinOrConnect(E.target, V2, V)) {
              I--;
              UnionMembers[Graph[E].UnionType] = V2;
            }
          }
        }
        break;
      }
      }
    }

    /*
    llvm::errs() << "Processing " << V << ": PointsTo=" << PointsTo << "  struct members=";
    for (auto It: StructMembers) {
      llvm::errs() << " " << It.first << ":" << It.second;
    }
    llvm::errs() << "\n";
    // */

    // Handle IncomingStructMembers etc
    SetCls<std::pair<int, Vertex>> OutgoingStructMembers;
    OutgoingStructMembers.insert(StructMembers.begin(), StructMembers.end());
    for (auto &Pair : Info[V].IncomingStructMembers) {
      auto V2 = resolve(Pair.second);
      auto It = StructMembers.find(Pair.first);
      if (It == StructMembers.end()) {
        OutgoingStructMembers.insert(Pair);
      } else {
        It->second = resolve(It->second);
        addEdge(V2, It->second, TGEdge(EdgeTypeNewCast));
      }
    }

    // Handle incoming union members
    SetCls<std::pair<const std::string *, Vertex>> OutgoingUnionMembers;
    OutgoingUnionMembers.insert(UnionMembers.begin(), UnionMembers.end());
    for (auto &Pair : Info[V].IncomingUnionMembers) {
      auto V2 = resolve(Pair.second);
      auto It = UnionMembers.find(Pair.first);
      if (It == UnionMembers.end()) {
        OutgoingUnionMembers.insert(Pair);
      } else {
        It->second = resolve(It->second);
        addEdge(V2, It->second, TGEdge(EdgeTypeNewCast));
      }
    }

    // Handle incoming pointers
    if (PointsTo != NO_VERTEX) {
      PointsTo = resolve(PointsTo);

      for (auto V2 : Info[V].IncomingPointsTo) {
        V2 = resolve(V2);
        if (V != V2) {
          if (joinOrConnect(PointsTo, V2, V) && PointsTo == V) break;
        } else {
          bool Joined = joinOrConnect(V2, PointsTo, V);
          PointsTo = V2;
          if (Joined) break;
        }
      }

      // Handle incoming externals that might need a pointer
      if (StoreReachability) {
        for (auto V2 : Info[V].IncomingExternalPointsTo) {
          V2 = resolve(V2);
          addEdge(V2, PointsTo, TGEdge(EdgeType::POINTS_TO));
          makeExternal(PointsTo);
        }
      }
    }

    // Handle incoming external reachability
    if (StoreReachability) {
      if (Graph[V].External) {
        for (auto V2 : Info[V].IncomingExternal) {
          V2 = resolve(V2);
          addEdge(V2, V, TGEdge(EdgeTypeNewCast));
        }

        // Handle special cases (new pointsto etc)
        // External(z)       :- External(x), !PointsTo(x, _), (SimpleCast2(x,y) ; SimpleCast2(y,x)), PointsTo(y, z).
        // NewPointsTo(x, z) :- External(x), !PointsTo(x, _), (SimpleCast2(x,y) ; SimpleCast2(y,x)), PointsTo(y, z).
        if (PointsTo == NO_VERTEX) {
          for (auto V2 : Info[V].IncomingPointsTo) {
            V2 = resolve(V2);
            addEdge(V, V2, TGEdge(EdgeType::POINTS_TO));
            makeExternal(V2);
          }
        }

      } else if (Graph[V].HasIndirectCalls && !Info[V].IncomingExternal.empty()) {
        makeExternal(V);
      }
    }

    // Handle incoming function uses (to propagate external). Usual function use handled by callback: handleNewFunctionUses
    // External(x) :- FunctionUses(x, _, _, _), External(y), (SimpleCast2(x,y) ; PointerCast(x,y); FunctionCast(x,y); FunctionCast2(x,y)).
    if (Graph[V].External) {
      for (Vertex V2 : Info[V].FunctionUsesFrom) {
        V2 = resolve(V2);
        if (!Graph[V2].External)
          makeExternal(V2);
      }
    }

    // Handle outgoing casts
    for (auto V2 : CastsTo) {
      V2 = resolve(V2);
      if (V == V2) continue;

      // propagate struct members
      auto Size = Info[V2].IncomingStructMembers.size();
      Info[V2].IncomingStructMembers.insert(OutgoingStructMembers.begin(), OutgoingStructMembers.end());
      if (Info[V2].IncomingStructMembers.size() > Size)
        enqueue(V2, true);

      // propagate union members
      Size = Info[V2].IncomingUnionMembers.size();
      Info[V2].IncomingUnionMembers.insert(OutgoingUnionMembers.begin(), OutgoingUnionMembers.end());
      if (Info[V2].IncomingUnionMembers.size() > Size)
        enqueue(V2, true);

      // propagate pointers
      if (PointsTo != NO_VERTEX) {
        if (Info[V2].IncomingPointsTo.insert(PointsTo).second) {
          enqueue(V2, true);
        }
      } else {
        Size = Info[V2].IncomingPointsTo.size();
        Info[V2].IncomingPointsTo.insert(Info[V].IncomingPointsTo.begin(), Info[V].IncomingPointsTo.end());
        if (Info[V2].IncomingPointsTo.size() > Size)
          enqueue(V2, true);
      }

      // propagate FunctionUses
      for (auto FU : Info[V].FunctionUsesFrom) {
        FU = resolve(FU);
        if (Info[V2].FunctionUsesFrom.insert(FU).second) {
          handleNewFunctionUses(V2, FU);
          enqueue(V2, true);
        }
      }

      // propagate externals
      if (StoreReachability) {
        if (Graph[V].External) {
          if (Info[V2].IncomingExternal.insert(V).second)
            enqueue(V2, true);
          if (PointsTo == NO_VERTEX) {
            if (Info[V2].IncomingExternalPointsTo.insert(V).second)
              enqueue(V2, true);
          }
        } else {
          Size = Info[V2].IncomingExternal.size();
          Info[V2].IncomingExternal.insert(Info[V].IncomingExternal.begin(), Info[V].IncomingExternal.end());
          if (Info[V2].IncomingExternal.size() > Size)
            enqueue(V2, true);
          if (PointsTo == NO_VERTEX) {
            Size = Info[V2].IncomingExternalPointsTo.size();
            Info[V2].IncomingExternalPointsTo.insert(Info[V].IncomingExternalPointsTo.begin(), Info[V].IncomingExternalPointsTo.end());
            if (Info[V2].IncomingExternalPointsTo.size() > Size)
              enqueue(V2, true);
          }
        }
      }

      assert(!Graph.vertices[V2].empty && "V2 got removed during processing of CastsTo");
    }

    // compute backward edges (if they are dirty)
    auto &InEdges = Graph.in_edges(V);
    for (size_t I = 0; I < InEdges.size(); I++) {
      const auto &E = InEdges[I];
      switch (Graph[E].Type) {
      case CAST_SIMPLE:
      case REACHABILITY:
        // if in CastsTo: merge
        if (CastsTo.contains(E.source)) {
          joinOrConnect(V, E.source, V);
        } else {
          enqueue(E.source, false);
        }
        break;
      case POINTS_TO:
      case STRUCT_MEMBER:
      case UNION_MEMBER:
        enqueue(E.source, false);
        break;
      }
    }
  }

  void handleNewFunctionUse(Vertex V, const FunctionUsage &FU, bool Enqueue = true) {
    /*
    CallCanTargetFunction(c,f) :- Call(x,c,ac), FunctionUses(x,f, af,0), ac=af.
    CallCanTargetFunction(c,f) :- Call(x,c,ac), FunctionUses(x,f, af,1), ac>=af.
    FunctionCast(x,z) :- CallCanTargetFunction(c,f), VertexTypeContext(y,t,c), VertexTypeContext(z,t,f),
    SimpleCast2(x,y). FunctionCast(z,x) :- CallCanTargetFunction(c,f), VertexTypeContext(y,t,c),
    VertexTypeContext(z,t,f), SimpleCast2(y,x). x = V here
     */
    for (auto &CI : Graph.CallInfos) {
      // argument number filter
      if (!CI.second.IsResolvePoint) {
        if (FU.IsVarArg && CI.second.NumArgs < FU.NumArgs)
          continue;
        if (!FU.IsVarArg && CI.second.NumArgs != FU.NumArgs)
          continue;
      }
      // if this call is triggered by functions from V
      if (std::find(CI.second.AllVertices.begin(), CI.second.AllVertices.end(), V) != CI.second.AllVertices.end()) {
        const auto *CallContext = CI.first;
        for (const auto &Pair : Graph.getTypeContextToVertex()) {
          if (Pair.first.Context == CallContext) {
            // pair.second is y
            auto Y = resolve(Pair.second);
            auto Z = resolve(Graph.getVertexOpt(Pair.first.Type, FU.ContextName));
            if (Z != NO_VERTEX) {
              // SimpleCast2(y,x)
              for (auto &E : Graph.out_edges(Y)) {
                if (Graph[E].Type == EdgeType::CAST_SIMPLE || Graph[E].Type == EdgeType::REACHABILITY) {
                  addEdge(Z, E.target, E.property, Enqueue);
                }
              }
              // SimpleCast2(x,y)
              for (auto &E : Graph.in_edges(Y)) {
                if (Graph[E].Type == EdgeType::CAST_SIMPLE || Graph[E].Type == EdgeType::REACHABILITY) {
                  addEdge(E.source, Z, E.property, Enqueue);
                }
              }
            }
          }
        }
      }
    }
  }

  void handleNewFunctionUses(Vertex V, Vertex FU) {
    if (Graph[V].HasIndirectCalls) {
      // Store possible functions if that's an indirect call
      if (StorePossibleFunctions) {
        // Graph[V].FunctionUses.insert(Graph[FU].FunctionUses.begin(), Graph[FU].FunctionUses.end());
        for (auto &FunctionUse : Graph[FU].FunctionUses) {
          if (Graph[V].FunctionUses.insert(FunctionUse).second) {
            handleNewFunctionUse(V, FunctionUse);
          }
        }
      } else {
        for (auto &FunctionUse : Graph[FU].FunctionUses) {
          handleNewFunctionUse(V, FunctionUse);
        }
      }
    }
  }

  void run() {
    // Progress queue
    while (!Queue.empty()) {
      auto V = Queue.front();
      Queue.pop();
      Info[V].Enqueued = false;
      if (!Graph.vertices[V].empty) {
        progressVertex(V);
      }
    }

    adjustAdditionalFields();
  }

  void adjustAdditionalFields() {
    for (auto &CallIt : Graph.CallInfos) {
      CallIt.second.V = resolve(CallIt.second.V);
      for (auto &V : CallIt.second.AllVertices) {
        V = resolve(V);
      }
    }
    for (auto &InterfaceList : Graph.Interfaces) {
      for (auto &Interface : InterfaceList.second) {
        for (auto &InterfaceVertex : Interface.Types) {
          InterfaceVertex = resolve(InterfaceVertex);
        }
      }
    }
    for (auto &It : Graph.getTypeContextToVertex()) {
      It.second = resolve(It.second);
    }
    for (auto &It: Graph.PotentialExternalInterfaces) {
      SetCls<Vertex> NewNodes;
      for (auto V: It.second) {
        auto V2 = resolve(V);
        if (V != TypeGraph::NO_VERTEX && V2 != TypeGraph::NO_VERTEX) {
          NewNodes.insert(V2);
        }
      }
      It.second = std::move(NewNodes);
    }
  }
};
} // namespace

void TypeGraph::computeReachabilityInline(bool StorePossibleFunctions, bool StoreReachability, bool RespectDefiningUnits) {
  long Nv = num_vertices();
  long Ne = num_edges();
  TimeClock C;
  ReachabilityWorker Worker(*this, StorePossibleFunctions, StoreReachability, RespectDefiningUnits);
  C.report("computeReachabilityInline prepare");
  Worker.run();
  C.report("computeReachabilityInline run");
  std::cerr << "                | Joined " << Worker.JoinedNodes << " nodes: " << Nv << " => " << num_vertices()
            << " vertices, " << Ne << " => " << num_edges() << " edges.\n";

  assertIntegrity(true);
}

} // namespace typegraph
