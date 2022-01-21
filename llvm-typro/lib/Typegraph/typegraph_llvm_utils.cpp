#include "llvm/Typegraph/typegraph_llvm_utils.h"
#include "algorithms.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Operator.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Typegraph/TGCallGraph.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/BinaryByteStream.h>
#include <llvm/Typegraph/TypegraphSettings.h>
#include "TGDispatcherBuilder.h"
#include <set>
#include <sstream>

namespace {

using namespace llvm;

llvm::GlobalValue *getGlobalFromMetadata(llvm::ValueAsMetadata *Meta) {
  if (!Meta || !Meta->getValue())
    return nullptr;
  auto *V = Meta->getValue();
  if (auto *V2 = llvm::dyn_cast<llvm::BitCastOperator>(V)) {
    V = V2->getOperand(0);
  }
  return llvm::dyn_cast_or_null<llvm::GlobalValue>(V);
}

} // namespace

namespace typegraph {

llvm::raw_ostream &operator<<(llvm::raw_ostream &OS, const TGNode &Node) {
  auto &X = OS << "(" << *Node.Type << " | " << *Node.Context;
  if (Node.Layer != 0) X << " | " << Node.Layer;
  return X << ")";
}

void ParseTypegraphFromMetadata(TypeGraph &Graph, llvm::Module &M) {
  for (auto &MD : M.getNamedMDList()) {
    if (!MD.getName().startswith("typegraph.nodes."))
      continue;
    auto MetaIdent = MD.getName().substr(16).str();
    Graph.parserNodesStart();
    for (unsigned I = 0; I < MD.getNumOperands(); I++) {
      auto NodeId = std::stoi(llvm::cast<llvm::MDString>(MD.getOperand(I)->getOperand(0))->getString());
      auto Repr = llvm::cast<llvm::MDString>(MD.getOperand(I)->getOperand(1))->getString();
      Graph.parserAddNode(NodeId, Repr);
    }
    Graph.parserNodesFinished();

    auto *Edges = M.getNamedMetadata("typegraph.edges." + MetaIdent);
    assert(Edges != nullptr);
    for (unsigned I = 0; I < Edges->getNumOperands(); I++) {
      auto Src = std::stoi(llvm::cast<llvm::MDString>(Edges->getOperand(I)->getOperand(0))->getString());
      auto Dst = std::stoi(llvm::cast<llvm::MDString>(Edges->getOperand(I)->getOperand(1))->getString());
      auto Repr = llvm::cast<llvm::MDString>(Edges->getOperand(I)->getOperand(2))->getString();
      Graph.parserAddEdge(Src, Dst, Repr);
    }

    auto *FunctionUses = M.getNamedMetadata("typegraph.functions." + MetaIdent);
    if (FunctionUses) {
      for (unsigned I = 0; I < FunctionUses->getNumOperands(); I++) {
        auto Node = std::stoi(llvm::cast<llvm::MDString>(FunctionUses->getOperand(I)->getOperand(0))->getString());
        auto ContextName = llvm::cast<llvm::MDString>(FunctionUses->getOperand(I)->getOperand(1))->getString();
        auto SymbolName = llvm::cast<llvm::MDString>(FunctionUses->getOperand(I)->getOperand(2))->getString();
        if (FunctionUses->getOperand(I)->getNumOperands() > 3) {
          auto *Value = llvm::dyn_cast_or_null<llvm::ValueAsMetadata>(FunctionUses->getOperand(I)->getOperand(3));
          if (Value && Value->getValue()) {
            if (auto *F = llvm::dyn_cast_or_null<llvm::Function>(getGlobalFromMetadata(Value))) {
              // add symbol to graph
              Graph.parserAddFunctionUse(Node, ContextName, F->getName(), F);
              continue;
            }
            llvm::errs() << *Value->getValue() << "\n";
          } else {
            auto *GV = M.getNamedValue(SymbolName);
            if (auto *F = llvm::dyn_cast_or_null<Function>(GV)) {
              Graph.parserAddFunctionUse(Node, ContextName, F->getName(), F);
              continue;
            }
          }
        }
        // if not skipped by "continue"
        llvm::errs() << "[Warning] Function use symbol not found: " << SymbolName << "\n";
        Graph.parserAddFunctionUse(Node, ContextName, SymbolName, nullptr);
      }
    }

    auto *Calls = M.getNamedMetadata("typegraph.calls." + MetaIdent);
    if (Calls) {
      for (unsigned I = 0; I < Calls->getNumOperands(); I++) {
        auto Node = std::stoi(llvm::cast<llvm::MDString>(Calls->getOperand(I)->getOperand(0))->getString());
        auto Repr = llvm::cast<llvm::MDString>(Calls->getOperand(I)->getOperand(1))->getString();
        Graph.parserAddIndirectCall(Node, Repr);
      }
    }

    // Parse interface definitions
    auto *Interfaces = M.getNamedMetadata("typegraph.interfaces." + MetaIdent);
    if (Interfaces) {
      for (unsigned I = 0; I < Interfaces->getNumOperands(); I++) {
        // We only consider symbols that survived this far. Otherwise, they are not "external" anyways.
        if (Interfaces->getOperand(I)->getNumOperands() > 1) {
          auto *SymRef = llvm::dyn_cast_or_null<llvm::ValueAsMetadata>(Interfaces->getOperand(I)->getOperand(1));
          auto Repr = llvm::cast<llvm::MDString>(Interfaces->getOperand(I)->getOperand(0))->getString();
          if (auto *Symbol = getGlobalFromMetadata(SymRef)) {
            Graph.parserAddInterface(Repr, Symbol);
          } else {
            std::stringstream Stream(Repr);
            std::string SymbolName;
            std::getline(Stream, SymbolName, '\t');
            if (auto *Symbol2 = M.getFunction(SymbolName)) {
              Graph.parserAddInterface(Repr, Symbol2);
            }
          }
        }
      }
    }

    // Parse defining units
    auto *Defining = M.getNamedMetadata("typegraph.defining." + MetaIdent);
    if (Defining) {
      for (unsigned I = 0; I < Defining->getNumOperands(); I++) {
        if (Defining->getOperand(I)->getNumOperands() == 2) {
          auto Context = llvm::cast<llvm::MDString>(Defining->getOperand(I)->getOperand(0))->getString();
          auto Unit = llvm::cast<llvm::MDString>(Defining->getOperand(I)->getOperand(1))->getString();
          const auto *ContextRef = Graph.SymbolContainer->get(Context);
          const auto *UnitRef = Graph.SymbolContainer->get(Unit);
          if (Settings.linktime_layering) {
            auto It = Graph.ContextDefiningUnits.find(ContextRef);
            assert(It == Graph.ContextDefiningUnits.end() || It->second == UnitRef);
          }
          Graph.ContextDefiningUnits[ContextRef] = UnitRef;
        }
      }
    }

    // Parse callgraph
    auto *CallgraphSCCs = M.getNamedMetadata("callgraph.sccs." + MetaIdent);
    if (CallgraphSCCs) {
      for (unsigned I = 0; I < CallgraphSCCs->getNumOperands(); I++) {
        auto NodeId = std::stoi(llvm::cast<llvm::MDString>(CallgraphSCCs->getOperand(I)->getOperand(0))->getString());
        auto Repr = llvm::cast<llvm::MDString>(CallgraphSCCs->getOperand(I)->getOperand(1))->getString();
        Graph.CallGraph->parserAddSCC(NodeId, Repr);
      }
    }
    auto *CallgraphEdges = M.getNamedMetadata("callgraph.edges." + MetaIdent);
    if (CallgraphEdges) {
      for (unsigned I = 0; I < CallgraphEdges->getNumOperands(); I++) {
        auto Src = std::stoi(llvm::cast<llvm::MDString>(CallgraphEdges->getOperand(I)->getOperand(0))->getString());
        auto Dst = std::stoi(llvm::cast<llvm::MDString>(CallgraphEdges->getOperand(I)->getOperand(1))->getString());
        Graph.CallGraph->parserAddEdge(Src, Dst);
      }
    }

    // Reset the parser after every graph
    Graph.parserReset();
  }
}

namespace {
std::string serializeEdge(TGEdge &E) {
  std::string S = std::to_string(E.Type);
  switch (E.Type) {
  case CAST_SIMPLE:
  case POINTS_TO:
  case typegraph::EdgeType::REACHABILITY:
    return S;
  case STRUCT_MEMBER:
    return S + "\t" + std::to_string(E.StructOffset);
  case UNION_MEMBER:
    return S + "\t" + *E.UnionType;
  }
  llvm_unreachable("EdgeKind");
}
} // namespace

void WriteTypegraphToMetadata(TypeGraph &Graph, llvm::Module &M, const std::string &UniqueIdentifier) {
  auto &C = M.getContext();
  // write to module
  auto *MetaNodes = M.getOrInsertNamedMetadata("typegraph.nodes." + UniqueIdentifier);
  auto *MetaEdges = M.getOrInsertNamedMetadata("typegraph.edges." + UniqueIdentifier);
  auto *MetaCalls = M.getOrInsertNamedMetadata("typegraph.calls." + UniqueIdentifier);
  auto *MetaFunctions = M.getOrInsertNamedMetadata("typegraph.functions." + UniqueIdentifier);
  auto *MetaInterfaces = M.getOrInsertNamedMetadata("typegraph.interfaces." + UniqueIdentifier);
  auto *MetaCGSCCs = M.getOrInsertNamedMetadata("callgraph.sccs." + UniqueIdentifier);
  auto *MetaCGEdges = M.getOrInsertNamedMetadata("callgraph.edges." + UniqueIdentifier);
  for (auto v : Graph.vertex_set()) {
    auto *n1 = llvm::MDString::get(C, std::to_string(v));
    MetaNodes->addOperand(llvm::MDNode::get(C, {n1, llvm::MDString::get(C, *Graph[v].Type + "\t" + *Graph[v].Context + "\t" + (Graph[v].ContextIsGlobal ? "1" : "0") + "\t" + std::to_string(Graph[v].Layer))}));
    // serialize additional type/context pairs
    for (auto &TCP : Graph[v].AdditionalNames) {
      MetaNodes->addOperand(llvm::MDNode::get(C, {n1, llvm::MDString::get(C, *TCP.Type + "\t" + *TCP.Context + "\t" + (TCP.ContextIsGlobal ? "1" : "0") + "\t" + std::to_string(TCP.Layer))}));
    }
    // serialize all edges
    for (auto &edge : Graph.out_edges(v)) {
      auto *n2 = llvm::MDString::get(C, std::to_string(edge.target));
      auto *repr = llvm::MDString::get(C, serializeEdge(edge.property));
      MetaEdges->addOperand(llvm::MDNode::get(C, {n1, n2, repr}));
    }
    for (auto &use : Graph[v].FunctionUses) {
      auto *contextName = llvm::MDString::get(C, *use.ContextName);
      if (use.Function) {
        auto *symbolName = llvm::MDString::get(C, use.Function->getName());
        auto *ref = llvm::ValueAsMetadata::get(use.Function);
        MetaFunctions->addOperand(llvm::MDNode::get(C, {n1, contextName, symbolName, ref}));
      } else {
        llvm::errs() << "[WARNING] Function use without reference (\"" << *use.SymbolName << "\"/\""<<*use.ContextName<<"\")\n";
        MetaFunctions->addOperand(llvm::MDNode::get(C, {n1, contextName, contextName}));
      }
    }
  }
  for (auto &it : Graph.CallInfos) {
    // we store the call's node next to the call instruction, but keep the serialized name for completeness
    auto *n = llvm::MDString::get(C, std::to_string(it.second.V));
    char sep = '\t';
    std::string S = *it.first + "\t" + (it.second.IsResolvePoint ? "1" : "0") + std::to_string(it.second.NumArgs);
    for (auto V: it.second.AllVertices) {
      S += sep + std::to_string(V);
      sep = ',';
    }
    auto *callname = llvm::MDString::get(C, S);
    MetaCalls->addOperand(llvm::MDNode::get(C, {n, callname}));
  }
  for (const auto &It : Graph.Interfaces) {
    for (const auto &Interface : It.second) {
      std::stringstream Stream;
      Interface.serialize(Stream);
      Stream.flush();
      auto *n1 = llvm::MDString::get(C, Stream.str());
      if (Interface.Symbol) {
        MetaInterfaces->addOperand(llvm::MDNode::get(C, {n1, llvm::ValueAsMetadata::get(Interface.Symbol)}));
      } else {
        MetaInterfaces->addOperand(llvm::MDNode::get(C, {n1}));
      }
    }
  }
  if (!Graph.ContextDefiningUnits.empty()) {
    auto *MetaDefining = M.getOrInsertNamedMetadata("typegraph.defining." + UniqueIdentifier);
    for (auto &It: Graph.ContextDefiningUnits) {
      MetaDefining->addOperand(llvm::MDNode::get(C, {llvm::MDString::get(C, *It.getFirst()), llvm::MDString::get(C, *It.getSecond())}));
    }
  }

  // Call graph
  for (auto V : Graph.CallGraph->vertex_set()) {
    auto *N1 = llvm::MDString::get(C, std::to_string(V));
    std::string S;
    for (const std::string *Ctx: (*Graph.CallGraph)[V].Contexts) {
      if (!S.empty()) S += "\t";
      S += *Ctx;
    }
    MetaCGSCCs->addOperand(llvm::MDNode::get(C, {N1, llvm::MDString::get(C, S)}));
    // serialize all edges
    for (auto &Edge : Graph.CallGraph->out_edges(V)) {
      auto *N2 = llvm::MDString::get(C, std::to_string(Edge.target));
      MetaCGEdges->addOperand(llvm::MDNode::get(C, {N1, N2}));
    }
  }
}

int CheckGraphForIntegrity(TypeGraph &Graph, llvm::Module &M) {
  int ErrorCount = 0;
  // Check for function symbols that are not present in LLVM
  for (auto V : Graph.vertex_set()) {
    for (auto &F : Graph[V].FunctionUses) {
      if (!F.Function) {
        llvm::errs() << "[WARNING] No function found for name: \"" << *F.ContextName << "\"\n";
        ErrorCount++;
      }
    }
  }
  // Check for calls that are not present
  for (auto &F : M.functions()) {
    for (auto &BB : F) {
      for (auto &Ins : BB) {
        if (auto *Call = llvm::dyn_cast<llvm::CallBase>(&Ins)) {
          if (!Call->isIndirectCall())
            continue;
          // check metadata
          auto *MD = Call->getMetadata("typegraph_node");
          if (MD) {
            std::vector<const std::string *> Callnames;
            for (auto &Op : MD->operands()) {
              const auto *CallName = Graph.SymbolContainer->get(llvm::cast<llvm::MDString>(Op)->getString());
              Callnames.push_back(CallName);
            }
            auto LastV = Graph.NO_VERTEX;
            for (const auto *Callname : Callnames) {
              auto V = Graph.getVertexForCall(Callname);
              if (V == Graph.NO_VERTEX) {
                llvm::errs() << "[WARNING] Indirect call named \"" << *Callname
                             << "\" has no matching type vertex: " << *Call << "\n";
                ErrorCount++;
              } else if (!Graph[V].HasIndirectCalls) {
                llvm::errs() << "[WARNING] Indirect call named \"" << *Callname
                             << "\" has vertex that is not marked as indirect!\n";
                ErrorCount++;
              } else if (V < 0 || V >= (Vertex)Graph.vertices.size()) {
                llvm::errs() << "[ERROR] Call vertex " << V << " is out of range!\n";
                ErrorCount++;
              }
              if (LastV == Graph.NO_VERTEX) {
                LastV = V;
              } else if (LastV != V) {
                llvm::errs() << "[WARNING] One call resolves to different vertices: " << *Call << " | name \""
                             << *Callname << "\" is " << Graph[V] << ", but we've seen " << Graph[LastV] << " before.\n";
                ErrorCount++;
              }
            }
          } else {
            llvm::errs() << "[WARNING] Indirect call instruction without metadata: " << *Call << " in "
                         << Call->getFunction()->getName() << "\n";
            ErrorCount++;
          }
        }
      }
    }
  }
  // Check graph interfaces
  for (const auto &InterfaceList : Graph.Interfaces) {
    for (const auto &Interface : InterfaceList.second) {
      if (Interface.SymbolName != InterfaceList.first) {
        llvm::errs() << "[WARNING] Interface in wrong list: \"" << *Interface.SymbolName << "\" in \""
                     << *InterfaceList.first << "\"\n";
        ErrorCount++;
      }
      for (auto V: Interface.Types) {
        if (V < TypeGraph::NO_VERTEX || V >= (Vertex) Graph.vertices.size()) {
          llvm::errs() << "[ERROR] Interface vertex " << V << " is out of range!\n";
          ErrorCount++;
        }
      }
    }
  }
  return ErrorCount;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"
// v1: all function uses and indirect calls are "external", no propagation
// v2: function uses and indirect calls might be internal, propagation happens
void markExternalInterfaceNodes(TypeGraph &Graph, bool AllCallsAreExternal, bool AllFunctionUsesAreExternal, bool isWholeDSO) {
  llvm::DenseSet<llvm::GlobalValue*> UsedFunctions;
  std::vector<Vertex> Sources;
  std::set<const std::string *> ExternalContexts;

  // Collect used functions, mark uses+calls if necessary
  for (auto V: Graph.vertex_set()) {
    for (auto &Use: Graph[V].FunctionUses) {
      UsedFunctions.insert(Use.Function);
    }
    if (Graph[V].External)
      continue;
    if (AllFunctionUsesAreExternal && Graph[V].FunctionUses.size() > 0) {
      Graph[V].External = true;
      Sources.push_back(V);
    } else if (AllCallsAreExternal && Graph[V].HasIndirectCalls) {
      Graph[V].External = true;
      Sources.push_back(V);
    } else if (!AllFunctionUsesAreExternal) {
      for (auto &FU: Graph[V].FunctionUses) {
        if (FU.IsDynamicFunction)
          Graph[V].External = true;
      }
    }
  }

  // mark external interfaces
  for (auto &It : Graph.Interfaces) {
    for (auto &Interface : It.second) {
      // check if interface is still public
      if (!Interface.Symbol)
        continue;
      // symbol must be "used function" or "exported"
      if (!Interface.IsExternal &&
          (Interface.Symbol->hasLocalLinkage() ||
           (isWholeDSO && Interface.Symbol->getVisibility() == llvm::GlobalValue::HiddenVisibility))) {
        if (UsedFunctions.find(Interface.Symbol) != UsedFunctions.end()) {
          // this function might get external later, if the function pointer is leaking
          for (auto V: Interface.Types) {
            if (V != TypeGraph::NO_VERTEX) {
              Graph[V].NameIsImportant = true;
              Graph.PotentialExternalInterfaces[Interface.ContextName].insert(V);
            }
          }
        }
        continue;
      }
      Interface.IsExternal = true;
      // Interface nodes
      for (auto V : Interface.Types) {
        if (V != TypeGraph::NO_VERTEX && !Graph[V].External) {
          Graph[V].External = true;
          Sources.push_back(V);
        }
      }
      // Vararg => whole context is external
      if (Interface.IsVarArg || Interface.DoNotMinimize) {
        ExternalContexts.emplace(Interface.ContextName);
      }
    }
  }
  // add function uses (and probably calls)
  if (!ExternalContexts.empty()) {
    for (auto V : Graph.vertex_set()) {
      if (!Graph[V].External && ExternalContexts.find(Graph[V].Context) != ExternalContexts.end()) {
        Graph[V].External = true;
        Sources.push_back(V);
      }
    }
  }
  // mark subnodes
  struct ExternalNodesVisitor : public basegraph::default_bfs_visitor<TypeGraph> {
    inline bool discoverVertex(typename TypeGraph::Vertex V, TypeGraph &G) {
      G[V].External = true;
      return true;
    }
    inline bool examineBackwardsEdge(const typename TypeGraph::Edge &E, TypeGraph &) { return false; }
    inline bool examineForwardsEdge(const typename TypeGraph::Edge &E, TypeGraph &G) {
      auto Type = G[E].Type;
      return Type == EdgeType::POINTS_TO || Type == EdgeType::STRUCT_MEMBER || Type == EdgeType::UNION_MEMBER;
    }
  } Visitor;
  basegraph::ColorMap Colors(Graph);
  basegraph::breadth_first_search_forward(Graph, Visitor, Sources, Colors);
}
#pragma clang diagnostic pop

void markImportantInterfaceNodes(TypeGraph &Graph) {
  // Collect used functions, mark uses+calls if necessary
  llvm::DenseSet<llvm::GlobalValue *> UsedFunctions;
  for (auto V : Graph.vertex_set()) {
    for (auto &Use : Graph[V].FunctionUses) {
      UsedFunctions.insert(Use.Function);
    }
  }

  // mark function interfaces that are indirectly called
  // - so that their interface symbol names don't dissappear during equivalence passes
  for (auto &It : Graph.Interfaces) {
    for (auto &Interface : It.second) {
      // check if interface is still public
      if (!Interface.Symbol)
        continue;
      if (UsedFunctions.find(Interface.Symbol) != UsedFunctions.end()) {
        // this function might get external later, if the function pointer is leaking
        for (auto V : Interface.Types) {
          if (V != TypeGraph::NO_VERTEX) {
            Graph[V].NameIsImportant = true;
          }
        }
      }
    }
  }
}

class BinaryStringStream {
  std::vector<char> Buffer;
  long CurrentCounterPos = 0;
public:
  BinaryStringStream() {
    Buffer.reserve(16);
  }
  BinaryStringStream(long Counters) {
    Buffer.resize(Counters * sizeof(long), 0);
  }

  void write(const char* Data, size_t Len) {
    Buffer.insert(Buffer.end(), Data, Data + Len);
  }

  BinaryStringStream &operator<<(const std::string &Str) {
    *this << Str.size();
    write(Str.c_str(), Str.size() + 1);
    return *this;
  }

  BinaryStringStream &operator<<(const long L) {
    write((const char*) &L, sizeof(long));
    return *this;
  }

  BinaryStringStream &operator<<(const size_t L) {
    write((const char*) &L, sizeof(size_t));
    return *this;
  }

  BinaryStringStream &operator<<(const void* Pointer) {
    write((const char*) &Pointer, sizeof(void*));
    return *this;
  }

  template <typename T>
  BinaryStringStream &operator<<(const T* Pointer) {
    write((const char*) &Pointer, sizeof(Pointer));
    return *this;
  }

  /*template <typename T, typename std::enable_if<std::is_fundamental<T>::value, bool>::type = true>
  BinaryStringStream &operator<<(const T &V) {
    Oss.write((const char *)&V, sizeof(T));
    return *this;
  }

  template <typename T, typename std::enable_if<std::is_fundamental<typename T::value_type>::value, bool>::type = true>
  BinaryStringStream &operator<<(const T &V) {
    Oss.write((char *)V.data(), V.size() * sizeof(typename T::value_type));
    return *this;
  }

  template <typename _InputIterator> BinaryStringStream &write(_InputIterator First, _InputIterator Last) {
    char *Data = (char *)&(*First);
    auto N = std::distance(First, Last);
    Oss.write(Data, N * sizeof(*First));
    return *this;
  }

  template <typename T, typename std::enable_if<std::is_fundamental<T>::value, bool>::type = true>
  BinaryStringStream &write(const T *V, std::streamsize Count) {
    Oss.write((const char *)V, sizeof(T) * Count);
    return *this;
  }*/

  std::size_t size() { return Buffer.size(); }

  const char* data() {
    return Buffer.data();
  }

  void initCounters(int Num) {
    CurrentCounterPos = Buffer.size();
    for (int I = 0; I < Num; I++)
      (*this) << 0L;
  }

  void count() {
    ((long*) (Buffer.data() + CurrentCounterPos))[0]++;
  }

  void nextCounter() {
    CurrentCounterPos += sizeof(long);
  }

};

static llvm::Constant *makeStringPointer(Module &M, StringRef String) {
  Constant *StrConstant = ConstantDataArray::getString(M.getContext(), String);
  auto *GV = new GlobalVariable(M, StrConstant->getType(), true, GlobalValue::PrivateLinkage, StrConstant, "", nullptr, GlobalVariable::NotThreadLocal);
  GV->setUnnamedAddr(GlobalValue::UnnamedAddr::Global);
  GV->setAlignment(Align::None());
  Constant *Zero = ConstantInt::get(Type::getInt32Ty(M.getContext()), 0);
  Constant *Indices[] = {Zero, Zero};
  return ConstantExpr::getInBoundsGetElementPtr(GV->getValueType(), GV, Indices);
}

static llvm::Constant *makePointerArray(Module &M, std::vector<Constant *> Constants) {
  auto *PtrType = Type::getInt8PtrTy(M.getContext());
  auto *Arr = ConstantArray::get(ArrayType::get(PtrType, Constants.size()), Constants);
  auto *GV = new GlobalVariable(M, Arr->getType(), true, GlobalValue::PrivateLinkage, Arr, "", nullptr, GlobalVariable::NotThreadLocal);
  GV->setUnnamedAddr(GlobalValue::UnnamedAddr::Global);
  Constant *Zero = ConstantInt::get(Type::getInt32Ty(M.getContext()), 0);
  Constant *Indices[] = {Zero, Zero};
  return ConstantExpr::getBitCast(ConstantExpr::getInBoundsGetElementPtr(GV->getValueType(), GV, Indices), PtrType);
}

std::vector<llvm::Value*> WriteTypegraphToConstant(TypeGraph &Graph, llvm::Module &M, LLVMDispatcherBuilder &DBuilder) {
  auto &C = M.getContext();
  BinaryStringStream Stream;
  std::vector<Constant *> References;
  auto *PtrType = Type::getInt8PtrTy(M.getContext());
  auto *LongType = Type::getInt64Ty(M.getContext());

  // Call dispatcher functions
  Stream << (size_t)DBuilder.ExternalCallDispatchers.size();
  for (auto It : DBuilder.ExternalCallDispatchers) {
    Stream << *It.first;
    References.push_back(ConstantExpr::getBitCast(It.second, PtrType));
    // Save all IDs that should not be handled by this dispatcher (=> anything we could have already handled here)
    SetCls<long> Ids;
    for (auto V: Graph.CallInfos[It.first].AllVertices) {
      for (auto &FU: Graph[V].FunctionUses) {
        auto *F = DBuilder.getFunctionOpt(FU.Function);
        if (F && F->Leaking && !F->Removed && !FU.IsDynamicFunction) {
          Ids.insert(F->ID);
        }
      }
    }
    Stream << (size_t) Ids.size();
    for (long ID : Ids)
      Stream << ID;
  }

  // Dynamic symbol things
  Stream << (size_t) DBuilder.ExternalDynamicSymbolInfos.size();
  for (auto It: DBuilder.ExternalDynamicSymbolInfos) {
    Stream << *It.first;
    References.push_back(ConstantExpr::getBitCast(It.second, PtrType));
  }

  Stream << (size_t) DBuilder.DynamicSymbolTargets.size();
  for (auto It: DBuilder.DynamicSymbolTargets) {
    Stream << *It.first;
    Stream << (size_t) It.second.size();
    for (const auto &S: It.second) {
      Stream << S;
    }
  }

  Stream.initCounters(6);

  // Functions
  llvm::DenseMap<const Function *, unsigned> FunctionToIndex;
  for (auto &It: DBuilder.getFunctions()) {
    if (It.second->Leaking && !It.second->Removed && It.second->Ref) {
      References.push_back(ConstantExpr::getBitCast(It.first, PtrType));
      Stream << It.second->ID;
      Stream.count();
      unsigned Index = FunctionToIndex.size();
      FunctionToIndex[It.first] = Index;
    }
  }
  Stream.nextCounter();

  // Vertices
  for (auto V : Graph.vertex_set()) {
    std::string NodeRepr = *Graph[V].Type + "\t" + *Graph[V].Context + "\t" + (Graph[V].ContextIsGlobal ? "1" : "0") +
                           "\t" + std::to_string(Graph[V].Layer);
    Stream << V << NodeRepr;
    Stream.count();
    // serialize additional type/context pairs
    for (auto &TCP : Graph[V].AdditionalNames) {
      // TODO better check if context is external?
      if (TCP.Layer == 0) {
        NodeRepr = *TCP.Type + "\t" + *TCP.Context + "\t" + (TCP.ContextIsGlobal ? "1" : "0") + "\t" +
                   std::to_string(TCP.Layer);
        Stream << V << NodeRepr;
        Stream.count();
      }
    }
  }
  Stream.nextCounter();
  for (auto V : Graph.vertex_set()) {
    // serialize all edges
    for (auto &Edge : Graph.out_edges(V)) {
      Stream << V << Edge.target << serializeEdge(Edge.property);
      Stream.count();
    }
  }
  Stream.nextCounter();
  for (auto V : Graph.vertex_set()) {
    for (auto &Use : Graph[V].FunctionUses) {
      if (Use.Function) {
        Stream << V << *Use.ContextName << *Use.SymbolName << (long) FunctionToIndex[Use.Function];
        Stream.count();
      } else if (Use.IsDynamicFunction) {
        Stream << V << *Use.ContextName << *Use.SymbolName << (long) -1;
        Stream.count();
      }
    }
  }
  Stream.nextCounter();
  for (auto &It : Graph.CallInfos) {
    // we store the call's node next to the call instruction, but keep the serialized name for completeness
    char Sep = '\t';
    std::string S = *It.first + "\t" + (It.second.IsResolvePoint ? "1" : "0") + std::to_string(It.second.NumArgs);
    for (auto V : It.second.AllVertices) {
      S += Sep + std::to_string(V);
      Sep = ',';
    }
    Stream << It.second.V << S;
    Stream.count();
  }
  Stream.nextCounter();
  for (const auto &It : Graph.Interfaces) {
    for (const auto &Interface : It.second) {
      std::stringstream SS;
      Interface.serialize(SS);
      SS.flush();
      Stream << SS.str();
      Stream.count();
    }
  }
  Stream.nextCounter();

  // Create a global string out of it
  return {
      makeStringPointer(M, StringRef(Stream.data(), Stream.size())),
      makePointerArray(M, References),
      ConstantInt::get(LongType, DBuilder.ModuleID)
  };
}

} // namespace typegraph
