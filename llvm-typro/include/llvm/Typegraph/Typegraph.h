#ifndef LLVM_TYPEGRAPHS_TYPEGRAPH_H
#define LLVM_TYPEGRAPHS_TYPEGRAPH_H

#include "basicdefs.h"
#include "basegraph.h"
#include "llvm/Typegraph/StringContainer.h"
#include <memory>
#include <string>


namespace llvm {
class Function;
class GlobalValue;
} // namespace llvm

namespace typegraph {

class TGCallGraph;

struct TGNode;
struct TGEdge;

typedef basegraph::BaseGraph<TGNode, TGEdge> TG_Proto;
typedef TG_Proto::Vertex Vertex;
typedef TG_Proto::Edge Edge;

struct FunctionUsage {
  const std::string *ContextName;
  const std::string *SymbolName;
  llvm::Function *Function;
  int NumArgs = 0;
  bool IsVarArg = true;
  bool IsDynamicFunction = false;

  FunctionUsage(const std::string *ContextName, const std::string *SymbolName)
      : ContextName(ContextName), SymbolName(SymbolName), Function(nullptr) {}

  FunctionUsage(const std::string *ContextName, const std::string *SymbolName, llvm::Function *Function)
      : ContextName(ContextName), SymbolName(SymbolName), Function(Function) {
#ifndef WITHOUT_LLVM
    if (Function && reinterpret_cast<uintptr_t>(Function) < static_cast<uintptr_t>(-64)) {
      NumArgs = Function->arg_size();
      IsVarArg = Function->isVarArg();
    }
#endif
  }
};

inline bool operator==(const FunctionUsage &Lhs, const FunctionUsage &Rhs) {
  return Lhs.ContextName == Rhs.ContextName && Lhs.SymbolName == Rhs.SymbolName;
}

inline bool operator<(const FunctionUsage &Lhs, const FunctionUsage &Rhs) {
  if (Lhs.ContextName == Rhs.ContextName)
    return Lhs.SymbolName < Rhs.SymbolName;
  return Lhs.ContextName < Rhs.ContextName;
}


/**
 * Infos about an indirect call
 */
struct CallInfo {
  Vertex V;
  VectorCls<Vertex> AllVertices;  // Including copies/layers of V
  int NumArgs;
  bool IsResolvePoint = false;

  CallInfo(Vertex V, int NumArgs, bool ResolvePoint = false) : V(V), NumArgs(NumArgs), IsResolvePoint(ResolvePoint) {
    AllVertices.push_back(V);
  }
  CallInfo() : V(-1), NumArgs(-1) {}

  void addVertex(long NewV) {
    if (NewV != V) AllVertices.push_back(NewV);
  }
};

inline bool operator==(const CallInfo &Lhs, const CallInfo &Rhs) {
  return Lhs.V == Rhs.V && Lhs.NumArgs == Rhs.NumArgs;
}

inline bool operator<(const CallInfo &Lhs, const CallInfo &Rhs) {
  if (Lhs.V == Rhs.V)
    return Lhs.NumArgs < Rhs.NumArgs;
  return Lhs.V < Rhs.V;
}

/**
 * Information about any public declaration in a translation unit (either exported or imported).
 * Once for any visible symbol.
 */
struct InterfaceDesc {
  const std::string *SymbolName;
  llvm::GlobalValue *Symbol = nullptr;
  const std::string *ContextName;
  bool IsFunction = false;
  bool IsVarArg = false;
  bool DoNotMinimize = false;
  bool IsExternal = false;
  bool IsDefined = false;
  VectorCls<Vertex> Types;

  void serialize(std::ostream &OS) const;
};

struct TypeContextPair {
  const std::string *Type;
  const std::string *Context;
  bool ContextIsGlobal = false;
  bool VoidMergedWithVoid = false;
  int Layer = 0;

  TypeContextPair(const std::string *Type, const std::string *Context, int Layer = 0, bool ContextIsGlobal = false)
      : Type(Type), Context(Context), ContextIsGlobal(ContextIsGlobal), Layer(Layer) {}
};

inline bool operator==(const TypeContextPair &Lhs, const TypeContextPair &Rhs) {
  return Lhs.Type == Rhs.Type && Lhs.Context == Rhs.Context && Lhs.Layer == Rhs.Layer;
}

inline bool operator<(const TypeContextPair &Lhs, const TypeContextPair &Rhs) {
  if (Lhs.Type == Rhs.Type) {
    if (Lhs.Context == Rhs.Context) {
      return Lhs.Layer < Rhs.Layer;
    }
    return Lhs.Context < Rhs.Context;
  }
  return Lhs.Type < Rhs.Type;
}

} // namespace typegraph

#ifndef WITHOUT_LLVM
namespace llvm {
template <> struct DenseMapInfo<typegraph::TypeContextPair> {
  static inline typegraph::TypeContextPair getEmptyKey() { return typegraph::TypeContextPair(nullptr, nullptr); }

  static inline typegraph::TypeContextPair getTombstoneKey() {
    uintptr_t Val = static_cast<uintptr_t>(-2);
    Val <<= llvm::PointerLikeTypeTraits<const std::string *>::NumLowBitsAvailable;
    const auto *Ptr = reinterpret_cast<const std::string *>(Val);
    return typegraph::TypeContextPair(Ptr, Ptr);
  }

  static unsigned getHashValue(const typegraph::TypeContextPair &PtrVal) {
    return (unsigned((uintptr_t)PtrVal.Type) >> 4) ^ (unsigned((uintptr_t)PtrVal.Type) >> 9) ^
           (unsigned((uintptr_t)PtrVal.Context) >> 5) ^ (unsigned((uintptr_t)PtrVal.Context) >> 10) ^ (unsigned(PtrVal.Layer << 16));
  }

  static bool isEqual(const typegraph::TypeContextPair &LHS, const typegraph::TypeContextPair &RHS) {
    return LHS == RHS;
  }
};

template <> struct DenseMapInfo<typegraph::FunctionUsage> {
  static inline typegraph::FunctionUsage getEmptyKey() { return typegraph::FunctionUsage(nullptr, nullptr, nullptr); }

  static inline typegraph::FunctionUsage getTombstoneKey() {
    uintptr_t Val = static_cast<uintptr_t>(-2);
    Val <<= llvm::PointerLikeTypeTraits<const std::string *>::NumLowBitsAvailable;
    const auto *Ptr = reinterpret_cast<const std::string *>(Val);
    return typegraph::FunctionUsage(Ptr, Ptr, reinterpret_cast<llvm::Function *>(Val));
  }

  static unsigned getHashValue(const typegraph::FunctionUsage &PtrVal) {
    return (unsigned((uintptr_t)PtrVal.ContextName) >> 4) ^ (unsigned((uintptr_t)PtrVal.ContextName) >> 9) ^
           (unsigned((uintptr_t)PtrVal.SymbolName) >> 5) ^ (unsigned((uintptr_t)PtrVal.SymbolName) >> 10);
  }

  static bool isEqual(const typegraph::FunctionUsage &LHS, const typegraph::FunctionUsage &RHS) { return LHS == RHS; }
};
} // namespace llvm
#endif

namespace typegraph {
struct TGNode : public TypeContextPair {
  VectorCls<TypeContextPair> AdditionalNames;

  // all functions that have been used with this type.
  // will also be filled with indirect function uses during computation
  SetCls<FunctionUsage> FunctionUses;
  bool HasIndirectCalls = false;

  // External/internal
  bool External = false;
  bool NameIsImportant = false; // name should be preserved across merges, but not across serialization. Used for "call#" nodes and interfaces of not-external functions.

  TGNode() : TypeContextPair(nullptr, nullptr) {}
  TGNode(const std::string *Type, const std::string *Context, bool ContextIsGlobal, int Layer = 0) : TypeContextPair(Type, Context, Layer, ContextIsGlobal), FunctionUses() {}
};

enum EdgeType { CAST_SIMPLE = 0, POINTS_TO = 1, STRUCT_MEMBER = 2, REACHABILITY = 3, UNION_MEMBER = 4 };

struct TGEdge {
  EdgeType Type;
  union {
    int StructOffset;
    const std::string *UnionType;
  };

  TGEdge(EdgeType Type) : Type(Type) {}
};

inline bool operator==(const TGEdge &Lhs, const TGEdge &Rhs) {
  if (Lhs.Type != Rhs.Type)
    return false;
  if (Lhs.Type == STRUCT_MEMBER)
    return Lhs.StructOffset == Rhs.StructOffset;
  if (Lhs.Type == UNION_MEMBER)
    return Lhs.UnionType == Rhs.UnionType;
  return true;
}

class TypeGraph : public TG_Proto {
  MapCls<TypeContextPair, Vertex> TypeContextToVertex;

public:
  static constexpr Vertex NO_VERTEX = -1;
  std::shared_ptr<StringContainer> SymbolContainer;
  MapCls<const std::string *, CallInfo> CallInfos;
  MapCls<const std::string *, std::vector<InterfaceDesc>> Interfaces; // Symbol name => all interfaces for this symbol
  MapCls<const std::string *, const std::string *> ContextDefiningUnits;
  MapCls<const std::string *, SetCls<Vertex>> PotentialExternalInterfaces; // Function context name => all interface nodes
  MapCls<const std::string *, SetCls<const std::string *>> DynamicSymbolTargets; // dlsym call => possible arguments

  std::shared_ptr<TGCallGraph> CallGraph;

  int NextLayer = 1;

  TypeGraph();
  TypeGraph(std::shared_ptr<StringContainer> SymbolContainer);

  Vertex createVertex(const std::string *Type, const std::string *Context, bool ContextIsGlobal, int Layer);

  Vertex createVertex(const TGNode &Node);

  void joinVertex(Vertex V, TGNode &Node, bool KeepOnlyExternalNames = false);

  inline Vertex getVertex(const std::string *Type, const std::string *Context, bool ContextIsGlobal, int Layer = 0) {
    auto it = TypeContextToVertex.find(TypeContextPair(Type, Context, Layer, ContextIsGlobal));
    if (it != TypeContextToVertex.end()) {
      return it->second;
    }
    return createVertex(Type, Context, ContextIsGlobal, Layer);
  }

  inline Vertex getVertexOpt(const std::string *Type, const std::string *Context, int Layer = 0) {
    auto it = TypeContextToVertex.find(TypeContextPair(Type, Context, Layer));
    if (it != TypeContextToVertex.end()) {
      return it->second;
    }
    return NO_VERTEX;
  }

  inline Vertex getVertexForCall(const std::string *Callname) {
    auto it = CallInfos.find(Callname);
    if (it == CallInfos.end())
      return NO_VERTEX;
    return it->second.V;
  }

  inline void addIndirectCall(Vertex V, const std::string *Callname, int NumArgs, bool IsResolvePoint = false) {
    CallInfos[Callname] = CallInfo(V, NumArgs, IsResolvePoint);
    (*this)[V].HasIndirectCalls = true;
  }

  inline const std::string *getDefiningUnit(Vertex V) const {
    auto It = ContextDefiningUnits.find(vertices[V].property.Context);
    if (It == ContextDefiningUnits.end())
      return nullptr;
    return It->second;
  }

  inline MapCls<TypeContextPair, Vertex> &getTypeContextToVertex() {
    return TypeContextToVertex;
  }

  long getNumExternalVertices() const;

  void loadFromFile(const std::string &Filename);
  void saveToFile(const std::string &Filename);

  // computation
  /**
   * Fix interfaces with varying definitions.
   */
  void computeInterfaceRelations();

  void computeFunctionUsesDebug(const std::string &Basedir);

  void computeReachability(bool StorePossibleFunctions, bool StoreReachability, int NumThreads = 0);

  void computeReachabilityInline(bool StorePossibleFunctions, bool StoreReachability, bool RespectDefiningUnits);

  std::unique_ptr<TypeGraph> computeEquivalenceClasses(bool KeepOnlyExternalNames, int NumThreads = 0);

  void combineEquivalencesInline(bool KeepOnlyExternalNames, bool RespectDefiningUnits, bool RespectTypes = false);

  /**
   * Before, the interface should be marked external and reachability should have been computed.
   * After, all nodes necessary for an external summary should be marked external.
   */
  void markExternallyReachableCallsAndUses();

  /**
   * Get a minified version of this graph - only "external" nodes and the edges between them are contained.
   * @return
   */
  std::unique_ptr<TypeGraph> getMinifiedGraph(bool RemoveInternalContexts = false);

  // parser
  void parserReset();
  void parserNodesStart();
  Vertex parserAddNode(int Idx, const std::string &Repr);
  void parserNodesFlush();
  void parserNodesFinished();
  void parserAddEdge(int Src, int Dst, const std::string &Repr);
  Vertex parserAddFunctionUse(int Idx, const std::string &Context, const std::string &SymbolName,
                              llvm::Function *Function);
  void parserAddIndirectCall(int Idx, const std::string &Repr);
  void parserAddInterface(const std::string &Repr, llvm::GlobalValue *Value);

  void assertIntegrity(bool CheckTypeContextMap = false);

private:
  MapCls<int, Vertex> ParserMap;
  std::vector<Vertex> ParserJoinMap;
};

} // namespace typegraph

#endif // LLVM_TYPEGRAPHS_TYPEGRAPH_H
