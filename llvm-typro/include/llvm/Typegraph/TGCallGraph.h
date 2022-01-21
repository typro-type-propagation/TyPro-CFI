#ifndef LLVM_TYPEGRAPHS_TGCALLGRAPH_H
#define LLVM_TYPEGRAPHS_TGCALLGRAPH_H
#include "StringContainer.h"
#include "basegraph.h"

#ifdef WITHOUT_LLVM
#include <map>
#include <set>
#define MapCls std::map
#define SetCls std::set
#else
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/IR/Function.h"
#define MapCls llvm::DenseMap
#define SetCls llvm::DenseSet
#endif

namespace typegraph {

template<class T>
struct TGSCCNode;
struct TGSCCEdge;

typedef basegraph::BaseGraph<TGSCCNode<const std::string *>, TGSCCEdge>::Vertex CGSCC;
typedef basegraph::BaseGraph<TGSCCNode<const std::string *>, TGSCCEdge>::Edge CGSCCEdge;

template<class T>
struct TGSCCNode {
  SetCls<T> Contexts;
  int Depth = 0;

  TGSCCNode() {}
  TGSCCNode(T Context) { Contexts.insert(Context); }
};

struct TGSCCEdge {};

inline bool operator==(const TGSCCEdge &Lhs, const TGSCCEdge &Rhs) { return true; }

template<class T>
class TGCallGraphBase : public basegraph::BaseGraph<TGSCCNode<T>, TGSCCEdge> {
protected:
  MapCls<T, CGSCC> ContextToSCC;

public:
  static constexpr CGSCC NO_VERTEX = -1;
  std::shared_ptr<StringContainer> SymbolContainer;

  TGCallGraphBase(std::shared_ptr<StringContainer> SymbolContainer) : SymbolContainer(SymbolContainer) {}

  CGSCC createVertex(T Context) {
    return ContextToSCC[Context] = TGCallGraphBase<T>::add_vertex(TGSCCNode<T>(Context));
  }

  inline CGSCC getVertex(T Context) {
    auto It = ContextToSCC.find(Context);
    if (It != ContextToSCC.end()) {
      return It->second;
    }
    return createVertex(Context);
  }
  inline CGSCC getVertexOpt(T Context) {
    auto It = ContextToSCC.find(Context);
    if (It != ContextToSCC.end()) {
      return It->second;
    }
    return NO_VERTEX;
  }
};

/**
 * A call graph representation.
 * Functions are represented by their context string, and we collect the direct calls between them.
 * Later we build SCCs (strongly connected components) out of that graph, where each SCC contains at least one function,
 * and the resulting graph is cycle-free.
 */
class TGCallGraph : public TGCallGraphBase<const std::string *> {

public:
  TGCallGraph();
  TGCallGraph(std::shared_ptr<StringContainer> SymbolContainer);

  void parserAddSCC(int V, const std::string &Repr);

  void parserAddEdge(int S, int T);

  void parserReset();

  void joinSCCs(CGSCC S1, CGSCC S2);

  std::shared_ptr<TGCallGraph> buildStronglyConnectedComponents();

private:
  MapCls<int, CGSCC> ParserMap;
};


/**
 * A CallGraph that respects layers
 */
class TGLayeredCallGraph : public TGCallGraphBase<std::pair<const std::string *, int>> {

public:
  TGLayeredCallGraph(std::shared_ptr<StringContainer> SymbolContainer) : TGCallGraphBase(SymbolContainer) {}

  void joinSCCs(CGSCC S1, CGSCC S2);

  std::shared_ptr<TGLayeredCallGraph> buildStronglyConnectedComponents();

  void importUnlayeredGraph(TGCallGraph& Graph);

  void optimizeGraph();

  void computeDepth();

  std::vector<CGSCC> getOrderCalleeBeforeCaller();

  std::vector<CGSCC> getOrderCallerBeforeCallee();

  bool hasEdge(CGSCC Source, CGSCC Target);
  CGSCC getLayeredVertex(CGSCC V, int Layer);
};

} // namespace typegraph

#endif // LLVM_TYPEGRAPHS_TGCALLGRAPH_H
