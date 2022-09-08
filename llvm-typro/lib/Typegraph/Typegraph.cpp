#include "llvm/Typegraph/Typegraph.h"
#include "llvm/Typegraph/TGCallGraph.h"
#include "algorithms.h"
#include <iostream>
#include <fstream>
#include <souffle/SouffleInterface.h>
#ifndef WITHOUT_LLVM
#include "llvm/IR/Function.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Typegraph/timeclock.h"
#include "llvm/Typegraph/TypegraphSettings.h"
#endif

namespace souffle {
extern size_t max_number_of_nodes;
} // namespace souffle

//#if defined(__aarch64__) || defined(_M_ARM64)
//#define DOMAIN_TYPE int64_t
//#else
#define DOMAIN_TYPE int32_t
//#endif

namespace typegraph {

void InterfaceDesc::serialize(std::ostream &OS) const {
  OS << *SymbolName << "\t" << *ContextName << "\t";
  OS << (IsFunction ? '1' : '0') << (IsVarArg ? '1' : '0') << (DoNotMinimize ? '1' : '0') << (IsDefined ? '1' : '0') << "\t";
  for (size_t I = 0; I < Types.size(); I++) {
    if (I > 0)
      OS << ',';
    OS << Types[I];
  }
}

TypeGraph::TypeGraph()
    : SymbolContainer(std::allocate_shared<StringContainer>(DefaultAlloc<StringContainer>())),
      CallGraph(std::allocate_shared<TGCallGraph>(DefaultAlloc<TGCallGraph>(), SymbolContainer)) {
  parserReset();
}

TypeGraph::TypeGraph(std::shared_ptr<StringContainer> SymbolContainer)
    : SymbolContainer(SymbolContainer),
      CallGraph(std::allocate_shared<TGCallGraph>(DefaultAlloc<TGCallGraph>(), SymbolContainer)) {
  parserReset();
}

Vertex TypeGraph::createVertex(const std::string *Type, const std::string *Context, bool ContextIsGlobal, int Layer) {
  auto V = TypeContextToVertex[TypeContextPair(Type, Context, Layer)] = add_vertex(TGNode(Type, Context, ContextIsGlobal, Layer));
  // TODO
  if (Context->substr(0, 5) == "call#" || Context->substr(0, 13) == "resolvepoint#")
    vertices[V].property.NameIsImportant = true;
  return V;
}

Vertex TypeGraph::createVertex(const TGNode &Node) {
  auto V = add_vertex(Node);
  TypeContextToVertex[TypeContextPair(Node.Type, Node.Context, Node.Layer)] = V;
  for (const auto &TCP : Node.AdditionalNames) {
    TypeContextToVertex[TypeContextPair(TCP.Type, TCP.Context, TCP.Layer)] = V;
  }
  return V;
}

void TypeGraph::joinVertex(Vertex V, TGNode &Node, bool KeepOnlyExternalNames) {
  auto &P = vertices[V].property;
  auto NeedVName = !KeepOnlyExternalNames || P.External || P.NameIsImportant;
  auto NeedNodeName = !KeepOnlyExternalNames || Node.External || Node.NameIsImportant;

  // Type, Context
  TypeContextToVertex[TypeContextPair(Node.Type, Node.Context, Node.Layer)] = V;
  for (const auto &TCP : Node.AdditionalNames) {
    TypeContextToVertex[TypeContextPair(TCP.Type, TCP.Context, TCP.Layer)] = V;
  }
  if (!NeedVName) {
    // overwrite type/context (we need only P or Node's names)
    if (NeedNodeName) {
      P.Type = Node.Type;
      P.Context = Node.Context;
      P.ContextIsGlobal = Node.ContextIsGlobal;
      P.VoidMergedWithVoid |= Node.VoidMergedWithVoid;
      P.Layer = Node.Layer;
      P.AdditionalNames = Node.AdditionalNames;
    }
  } else if (NeedNodeName) {
    P.AdditionalNames.reserve(P.AdditionalNames.size() + 1 + Node.AdditionalNames.size());
    // add type/context as secondary (we need P and Nodes names)
    P.AdditionalNames.emplace_back(Node.Type, Node.Context, Node.Layer, Node.ContextIsGlobal);
    // P.AdditionalNames.insert(P.AdditionalNames.end(), Node.AdditionalNames.begin(), Node.AdditionalNames.end());
    for (auto &TCP : Node.AdditionalNames) {
      P.AdditionalNames.emplace_back(TCP);
    }
  }

  // function uses, calls, flags
  // P.FunctionUses.merge(Node.FunctionUses);
  P.FunctionUses.insert(Node.FunctionUses.begin(), Node.FunctionUses.end());
  P.HasIndirectCalls |= Node.HasIndirectCalls;
  P.External |= Node.External;
  P.NameIsImportant |= Node.NameIsImportant;
}

void TypeGraph::assertIntegrity(bool CheckTypeContextMap) {
  assert_integrity();

  auto Limit = (Vertex)vertices.size();

  for (auto &InterfaceList : Interfaces) {
    for (auto &Interface : InterfaceList.second) {
      for (auto V : Interface.Types) {
        assert(V >= NO_VERTEX && V < Limit && "Interface node out of bounds!");
        assert((V == NO_VERTEX || !vertices[V].empty) && "Interface node is deleted!");
      }
    }
  }
  for (auto &It : CallInfos) {
    assert(It.second.V != NO_VERTEX && "Call points to NO_VERTEX");
    assert(It.second.V >= 0 && It.second.V < Limit && "Call node out of bounds!");
    assert(!vertices[It.second.V].empty && "Call node is deleted!");
    assert(It.second.V == It.second.AllVertices.at(0) && "V must be first node in AllVertices");
  }
  if (CheckTypeContextMap) {
    for (auto &It : TypeContextToVertex) {
      assert(It.second != NO_VERTEX && "TypeContext points to NO_VERTEX");
      assert(It.second >= 0 && It.second < Limit && "TypeContext node out of bounds!");
      assert(!vertices[It.second].empty && "TypeContext node is deleted!");
    }
  }
}

void TypeGraph::loadFromFile(const std::string &Filename) {
  std::ifstream F1(Filename);
  if (F1.fail()) {
    std::cerr << "Error: Could not read file " << Filename << std::endl;
    return;
  }
  parserNodesStart();
  for (std::string Line; getline(F1, Line);) {
    if (Line.empty())
      continue;
    if (Line[0] == 'N') {
      std::stringstream Stream(Line.substr(2));
      int Idx;
      Stream >> Idx;
      parserAddNode(Idx, Line.substr(3 + Stream.tellg()));
    } else if (Line[0] == 'S') {
      std::stringstream Stream(Line.substr(2));
      int Idx;
      Stream >> Idx;
      CallGraph->parserAddSCC(Idx, Line.substr(3 + Stream.tellg()));
    }
  }
  parserNodesFinished();
  F1.close();

  std::ifstream F2(Filename);
  if (F2.fail()) {
    std::cerr << "Error: Could not read file " << Filename << std::endl;
    return;
  }
  for (std::string Line; getline(F2, Line);) {
    if (Line.empty())
      continue;
    switch (Line[0]) {
    case 'E': {
      std::stringstream Stream(Line.substr(2));
      int Src, Dst;
      Stream >> Src >> Dst;
      parserAddEdge(Src, Dst, Line.substr(3 + Stream.tellg()));
      break;
    }

    case 'F': {
      std::stringstream Stream(Line.substr(2));
      int Idx;
      std::string Context;
      std::string SymbolName;
      Stream >> Idx;
      std::getline(Stream, Context, '\t'); // this line twice, because first one reads just the '\t'
      std::getline(Stream, Context, '\t');
      std::getline(Stream, SymbolName, '\t');
      parserAddFunctionUse(Idx, Context, SymbolName, nullptr);
      break;
    }

    case 'C': {
      std::stringstream Stream(Line.substr(2));
      int Idx;
      Stream >> Idx;
      parserAddIndirectCall(Idx, Line.substr(3 + Stream.tellg()));
      break;
    }

    case 'I': {
      parserAddInterface(Line.substr(2), nullptr);
      break;
    }

    case 'T': {
      // Callgraph Edge
      std::stringstream Stream(Line.substr(2));
      int Src, Dst;
      Stream >> Src >> Dst;
      CallGraph->parserAddEdge(Src, Dst);
      break;
    }

    case 'D': {
      std::stringstream Stream(Line.substr(2));
      std::string Context;
      std::string Unit;
      std::getline(Stream, Context, '\t');
      std::getline(Stream, Unit, '\t');
      const auto *CtxRef = SymbolContainer->get(Context);
      const auto *UnitRef = SymbolContainer->get(Unit);
      assert(ContextDefiningUnits.find(CtxRef) == ContextDefiningUnits.end() || ContextDefiningUnits[CtxRef] == UnitRef);
      ContextDefiningUnits[CtxRef] = UnitRef;
    }
    }
  }
  F2.close();

  parserReset();
}

void TypeGraph::saveToFile(const std::string &Filename) {
  std::ofstream File(Filename);
  for (auto V : vertex_set()) {
    File << "N " << V << "\t" << *vertices[V].property.Type << "\t" << *vertices[V].property.Context << "\t"
         << (vertices[V].property.ContextIsGlobal ? "1" : "0") << (vertices[V].property.External ? "1" : "0") << "\t"
         << vertices[V].property.Layer << "\n";
    for (const auto &TCP : vertices[V].property.AdditionalNames) {
      File << "N " << V << "\t" << *TCP.Type << "\t" << *TCP.Context << "\t" << (TCP.ContextIsGlobal ? "1" : "0")
           << "\t" << TCP.Layer << "\n";
    }
  }
  for (auto V : vertex_set()) {
    for (auto &E : out_edges(V)) {
      File << "E " << E.source << "\t" << E.target << "\t" << E.property.Type;
      if (E.property.Type == STRUCT_MEMBER) {
        File << "\t" << E.property.StructOffset;
      } else if (E.property.Type == UNION_MEMBER) {
        File << "\t" << *E.property.UnionType;
      }
      File << "\n";
    }
    for (auto &Use : vertices[V].property.FunctionUses) {
      File << "F " << V << "\t" << *Use.ContextName << "\t";
#ifdef WITHOUT_LLVM
      File << *Use.SymbolName;
#else
      if (Use.Function && Use.Function->hasName()) {
        File << Use.Function->getName().str();
      } else {
        File << *Use.SymbolName;
      }
#endif
      File << "\n";
    }
  }
  for (const auto &It : CallInfos) {
    File << "C " << It.second.V << "\t" << *It.first << "\t" << (It.second.IsResolvePoint ? '1' : '0') << It.second.NumArgs;
    char Sep = '\t';
    for (auto V: It.second.AllVertices) {
      File << Sep << std::to_string(V);
      Sep = ',';
    }
    File << "\n";
  }
  for (const auto &It : Interfaces) {
    for (const auto &Interface : It.second) {
      File << "I ";
      Interface.serialize(File);
      File << "\n";
    }
  }

  for (auto V : CallGraph->vertex_set()) {
    File << "S " << V;
    for (const std::string *C: (*CallGraph)[V].Contexts) {
      File << "\t" << *C;
    }
    File << "\n";
  }
  for (auto V : CallGraph->vertex_set()) {
    for (auto &E : CallGraph->out_edges(V)) {
      File << "T " << E.source << "\t" << E.target << "\n";
    }
  }

  for (auto &It: ContextDefiningUnits) {
    File << "D " << *It.first << "\t" << *It.second << "\n";
  }
}

void TypeGraph::parserReset() {
  ParserMap.clear();
  ParserMap[-1] = NO_VERTEX;
  CallGraph->parserReset();
}

void TypeGraph::parserNodesStart() { ParserJoinMap = std::vector<Vertex>(vertices.size(), NO_VERTEX); }

Vertex TypeGraph::parserAddNode(int Idx, const std::string &Repr) {
  std::string TyBuffer, CtxBuffer, CtxGlobalBuffer, LayerBuffer;
  std::stringstream Stream(Repr);
  std::getline(Stream, TyBuffer, '\t');
  std::getline(Stream, CtxBuffer, '\t');
  std::getline(Stream, CtxGlobalBuffer, '\t');
  std::getline(Stream, LayerBuffer, '\t');
  assert(!TyBuffer.empty() && !CtxBuffer.empty());
  const auto *Ty = SymbolContainer->get(TyBuffer);
  const auto *Ctx = SymbolContainer->get(CtxBuffer);
  bool ContextIsGlobal = CtxGlobalBuffer[0] == '1';
  int Layer = LayerBuffer.empty() ? 0 : std::stoi(LayerBuffer);
  // is this an additional name?
  auto It = ParserMap.find(Idx);
  if (It != ParserMap.end()) {
    auto V = It->second;
    auto V2 = getVertexOpt(Ty, Ctx, Layer);
    if (V2 == NO_VERTEX) {
      // case 1: this name has not been present before
      vertices[V].property.AdditionalNames.emplace_back(Ty, Ctx, Layer, ContextIsGlobal);
      TypeContextToVertex[TypeContextPair(Ty, Ctx, Layer)] = V;
      // TODO
      if (Ctx->substr(0, 5) == "call#" || Ctx->substr(0, 13) == "resolvepoint#")
        vertices[V].property.NameIsImportant = true;
    } else if (V2 == V) {
      // case 2: this name was present before - in this node
      // do nothing, return V
    } else {
      // case 3: this name was present before - in another node V2. => join V and V2.
      joinVertex(V, vertices[V2].property);
      // move edges from/to V2 to V
      replace_vertex_uses(V2, V);
      // delete V2 and remove V2 from maps: calls, interfaces, ParserMap
      if (vertices[V2].property.HasIndirectCalls) {
        for (auto &CallIt : CallInfos) {
          if (CallIt.second.V == V2)
            CallIt.second.V = V;
          for (auto &SomeV: CallIt.second.AllVertices) {
            if (SomeV == V2) SomeV = V;
          }
        }
      }
      for (auto &ParserIt : ParserMap) {
        if (ParserIt.second == V2)
          ParserIt.second = V;
      }
      /*
      for (auto &InterfaceList : Interfaces) {
        for (auto &Interface : InterfaceList.second) {
          for (auto &InterfaceVertex : Interface.Types) {
            if (InterfaceVertex == V2)
              InterfaceVertex = V;
          }
        }
      }
       */
      // record changes to the OLD nodes (from previous node sets) in this map (to fix interfaces etc)
      if (V2 >= (Vertex)ParserJoinMap.size()) {
        ParserJoinMap.resize(V2 + 10, NO_VERTEX);
      }
      if (ParserJoinMap[V2] == NO_VERTEX) {
        if (V < (Vertex)ParserJoinMap.size() && ParserJoinMap[V] != NO_VERTEX) {
          parserNodesFlush();
        }
        ParserJoinMap[V2] = V;
      }
      remove_vertex(V2);
    }
    return V;
  }

  Vertex V = getVertex(Ty, Ctx, ContextIsGlobal, Layer);
  // assert(V == Idx && "Parser changed numbers!");
  ParserMap[Idx] = V;
  return V;
}

void TypeGraph::parserNodesFlush() {
  for (auto &InterfaceList : Interfaces) {
    for (auto &Interface : InterfaceList.second) {
      for (auto &InterfaceVertex : Interface.Types) {
        if (InterfaceVertex != NO_VERTEX) {
          Vertex V;
          while (InterfaceVertex < (Vertex)ParserJoinMap.size() && (V = ParserJoinMap[InterfaceVertex]) != NO_VERTEX) {
            InterfaceVertex = V;
          }
        }
      }
    }
  }
  std::fill(ParserJoinMap.begin(), ParserJoinMap.end(), NO_VERTEX);
}

void TypeGraph::parserNodesFinished() {
  for (auto &InterfaceList : Interfaces) {
    for (auto &Interface : InterfaceList.second) {
      for (auto &InterfaceVertex : Interface.Types) {
        if (InterfaceVertex != NO_VERTEX) {
          Vertex V;
          while (InterfaceVertex < (Vertex)ParserJoinMap.size() && (V = ParserJoinMap[InterfaceVertex]) != NO_VERTEX) {
            InterfaceVertex = V;
          }
        }
      }
    }
  }
  ParserJoinMap.clear();
}

void TypeGraph::parserAddEdge(int Src, int Dst, const std::string &Repr) {
  auto V1 = ParserMap.find(Src);
  auto V2 = ParserMap.find(Dst);
  assert(V1 != ParserMap.end());
  assert(V2 != ParserMap.end());
  std::stringstream Ss(Repr);
  int Type;
  Ss >> Type;
  TGEdge Edge(static_cast<EdgeType>(Type));
  if (Edge.Type == STRUCT_MEMBER) {
    Ss >> Edge.StructOffset;
  } else if (Edge.Type == UNION_MEMBER) {
    std::string UT;
    Ss >> UT;
    Edge.UnionType = SymbolContainer->get(UT);
  }
  add_edge(V1->second, V2->second, Edge);
}

Vertex TypeGraph::parserAddFunctionUse(int Idx, const std::string &Context, const std::string &SymbolName,
                                       llvm::Function *Function) {
  const auto *ContextPtr = SymbolContainer->get(Context);
  const auto *SymbolNamePtr = SymbolContainer->get(SymbolName);
  auto V = ParserMap.find(Idx);
  assert(V != ParserMap.end());
  auto Usage = vertices[V->second].property.FunctionUses.insert(FunctionUsage(ContextPtr, SymbolNamePtr, Function));
#ifndef WITHOUT_LLVM
  if (!Usage.second && Function) {
    Usage.first->Function = Function;
    Usage.first->NumArgs = Function->arg_size();
    Usage.first->IsVarArg = Function->isVarArg();
  } else {
    if (llvm::StringRef(Context).startswith("dynamicsymbol#")) {
      Usage.first->IsDynamicFunction = true;
    }
  }
#else
  if (Usage.second && SymbolName.substr(0, 14) == "dynamicsymbol#") {
    *(const_cast<bool *>(&Usage.first->IsDynamicFunction)) = true;
  }
#endif
  return V->second;
}

void TypeGraph::parserAddIndirectCall(int Idx, const std::string &Repr) {
  auto V = ParserMap.find(Idx);
  assert(V != ParserMap.end());
  std::stringstream Stream(Repr);
  std::string CallSymbolText;
  std::string Text;
  std::getline(Stream, CallSymbolText, '\t');
  std::getline(Stream, Text, '\t');
  bool IsResolvePoint = Text[0] == '1';
  int NumArgs = std::stoi(Text.substr(1));
  std::getline(Stream, Text, '\t');

  const auto *CallSymbol = SymbolContainer->get(CallSymbolText);
  CallInfo CI(V->second, NumArgs, IsResolvePoint);
  std::stringstream Stream2(Text);
  std::string VertexText;
  while(std::getline(Stream2, VertexText, ',')) {
    auto V2 = ParserMap.find(std::stol(VertexText));
    assert(V2 != ParserMap.end());
    CI.addVertex(V2->second);
  }

  for (auto CallV: CI.AllVertices) {
    vertices[CallV].property.HasIndirectCalls = true;
  }
  CallInfos[CallSymbol] = std::move(CI);
}

void TypeGraph::parserAddInterface(const std::string &Repr, llvm::GlobalValue *Value) {
  std::stringstream Stream(Repr);
  InterfaceDesc Interface;
  std::string SymbolName;
  std::string ContextName;
  std::getline(Stream, SymbolName, '\t');
  std::getline(Stream, ContextName, '\t');
  Interface.SymbolName = SymbolContainer->get(SymbolName);
  Interface.ContextName = SymbolContainer->get(ContextName);
  Interface.Symbol = Value;
#ifndef WITHOUT_LLVM
  if (Value && Value->hasName()) {
    Interface.SymbolName = SymbolContainer->get(Value->getName());
  }
#endif
  std::string Params;
  std::getline(Stream, Params, '\t');
  Interface.IsFunction = Params[0] == '1';
  Interface.IsVarArg = Params[1] == '1';
  Interface.DoNotMinimize = Params[2] == '1';
  Interface.IsDefined = Params[3] == '1';
  for (int I; Stream >> I;) {
    auto V = ParserMap.find(I);
    assert(V != ParserMap.end());
    Interface.Types.push_back(V->second);
    if (Stream.peek() == ',')
      Stream.ignore();
  }
  Interfaces[Interface.SymbolName].push_back(std::move(Interface));
}

void TypeGraph::computeInterfaceRelations() {
  for (auto &It : Interfaces) {
    InterfaceDesc *RefInterface = nullptr;
    for (auto &Interface : It.second) {
      if (RefInterface == nullptr) {
        RefInterface = &Interface;
      } else {
        for (size_t I = 0; I < std::min(RefInterface->Types.size(), Interface.Types.size()); I++) {
          auto V1 = RefInterface->Types[I];
          auto V2 = Interface.Types[I];
          if (V1 != NO_VERTEX && V2 != NO_VERTEX && V1 != V2) {
            std::cerr << "[Interface Checker] Symbol \"" << *It.first << "\": Definitions differ in [" << I << "], ";
            std::cerr << "\"" << *(*this)[V1].Type << "\" != \"" << *(*this)[V2].Type << "\"\n";
            add_edge(V1, V2, TGEdge(CAST_SIMPLE));
            add_edge(V2, V1, TGEdge(CAST_SIMPLE));
          }
        }
        // Check if sizes differ
        if (Interface.Types.size() > RefInterface->Types.size()) {
          std::cerr << "[Interface Checker] Symbol \"" << *It.first << "\": Definitions differ in size\n";
          RefInterface = &Interface;
        }
      }
    }
  }
}

void TypeGraph::computeFunctionUsesDebug(const std::string &Basedir) {
#ifndef WITHOUT_LLVM
  llvm::sys::fs::create_directories(Basedir);
#endif
  std::ofstream SimpleCast(Basedir + "/SimpleCast.facts");
  std::ofstream VoidCast(Basedir + "/VoidCast.facts");
  std::ofstream NonVoidCast(Basedir + "/NonVoidCast.facts");
  std::ofstream PointsTo(Basedir + "/PointsTo.facts");
  std::ofstream StructMember(Basedir + "/StructMember.facts");
  std::ofstream UnionMember(Basedir + "/UnionMember.facts");
  std::ofstream Calls(Basedir + "/Call.facts");
  std::ofstream FunctionUses(Basedir + "/FunctionUses.facts");
  std::ofstream ExternalInterfaces(Basedir + "/ExternalInterfaces.facts");
  std::ofstream VertexTypeContext(Basedir + "/VertexTypeContext.facts");
  std::ofstream VertexTypeCall(Basedir + "/VertexTypeCall.facts");
  std::ofstream External(Basedir + "/External.facts");
  const std::string VoidStr("voіd *");
  for (const auto V : vertex_set()) {
    if (vertices[V].property.Context->substr(0, 5) == "call#" || vertices[V].property.Context->substr(0, 13) == "resolvepoint#") {
      VertexTypeCall << V << "\t" << *vertices[V].property.Type << "\t" << *vertices[V].property.Context << "\n";
    } else {
      VertexTypeContext << V << "\t" << *vertices[V].property.Type << "\t" << *vertices[V].property.Context << "\n";
    }
    // other type/context pairs
    for (const auto &TCP : vertices[V].property.AdditionalNames) {
      if (TCP.Context->substr(0, 5) == "call#" || TCP.Context->substr(0, 13) == "resolvepoint#") {
        VertexTypeCall << V << "\t" << *TCP.Type << "\t" << *TCP.Context << "\n";
      } else {
        VertexTypeContext << V << "\t" << *TCP.Type << "\t" << *TCP.Context << "\n";
      }
    }
    for (const auto &E : out_edges(V)) {
      switch ((*this)[E].Type) {
      case CAST_SIMPLE:
      case REACHABILITY:
        if (E.source != E.target) {
          if (vertices[V].property.Type->find(VoidStr) != std::string::npos) {
            VoidCast << E.source << "\t" << E.target << "\n";
          } else {
            NonVoidCast << E.source << "\t" << E.target << "\n";
          }
        }
        SimpleCast << E.source << "\t" << E.target << "\n";
        break;
      case POINTS_TO:
        PointsTo << E.source << "\t" << E.target << "\n";
        break;
      case STRUCT_MEMBER:
        StructMember << E.source << "\t" << E.target << "\t" << E.property.StructOffset << "\n";
        break;
      case UNION_MEMBER:
        UnionMember << E.source << "\t" << E.target << "\t" << SymbolContainer->getUniqueId(E.property.UnionType)
                    << "\n";
        break;
      }
    }
    for (const auto &F : vertices[V].property.FunctionUses) {
      FunctionUses << V << "\t" << *F.ContextName << "\t" << F.NumArgs << "\t" << (F.IsVarArg ? 1 : 0) << "\n";
      if (F.IsDynamicFunction) {
        const auto &TargetSet = DynamicSymbolTargets.find(F.ContextName);
        if (TargetSet != DynamicSymbolTargets.end()) {
          for (const auto *Target: TargetSet->second) {
            const auto &InterfaceList = Interfaces.find(Target);
            if (InterfaceList != Interfaces.end()) {
              for (auto &Interface: InterfaceList->second) {
                FunctionUses << V << "\t" << *Interface.ContextName << "\t" << Interface.Types.size() - 1 << "\t" << (Interface.IsVarArg ? 1 : 0) << "\n";
              }
            }
          }
        }
      }
    }
    if (vertices[V].property.External) {
      External << V << "\n";
    }
  }
  for (auto &It: PotentialExternalInterfaces) {
    for (Vertex V: It.second) {
      ExternalInterfaces << *It.first << "\t" << V << "\n";
    }
  }
  for (const auto &It : CallInfos) {
    for (auto V: It.second.AllVertices) {
      Calls << V << "\t" << *It.first << "\t" << (It.second.IsResolvePoint ? 0xffff : It.second.NumArgs) << "\n";
    }
  }
}

void TypeGraph::computeReachability(bool StorePossibleFunctions, bool StoreReachability, int NumThreads) {
  const char *ProgramName = "compute_function_uses";
  souffle::max_number_of_nodes = vertices.size();
  std::unique_ptr<souffle::SouffleProgram> Program(souffle::ProgramFactory::newInstance(ProgramName));
  assert(Program.get());
  Program->setNumThreads(NumThreads);

  //auto *SimpleCast = Program->getRelation("SimpleCast");
  auto *VoidCast = Program->getRelation("VoidCast");
  auto *NonVoidCast = Program->getRelation("NonVoidCast");
  auto *PointsTo = Program->getRelation("PointsTo");
  auto *StructMember = Program->getRelation("StructMember");
  auto *UnionMember = Program->getRelation("UnionMember");
  auto *Calls = Program->getRelation("Call");
  auto *FunctionUses = Program->getRelation("FunctionUses");
  auto *ExternalInterfaces = Program->getRelation("ExternalInterfaces");
  auto *VertexTypeContext = Program->getRelation("VertexTypeContext");
  auto *VertexTypeCall = Program->getRelation("VertexTypeCall");
  auto *External = Program->getRelation("External");
  //assert(SimpleCast && PointsTo && StructMember && Calls && FunctionUses && VertexTypeContext && External);
  //assert(SimpleCast && PointsTo && StructMember && Calls && FunctionUses && VertexTypeContext && External);
  assert(PointsTo && StructMember && Calls && FunctionUses && ExternalInterfaces && VertexTypeContext && VertexTypeCall && External);
  assert(VoidCast);
  assert(NonVoidCast);
  const std::string VoidStr("voіd *");
  for (const auto V : vertex_set()) {
    if (vertices[V].property.Context->substr(0, 5) == "call#" || vertices[V].property.Context->substr(0, 13) == "resolvepoint#") {
      VertexTypeCall->insert(souffle::tuple(VertexTypeCall)
                                << (DOMAIN_TYPE)V << *vertices[V].property.Type << *vertices[V].property.Context);
    } else {
      VertexTypeContext->insert(souffle::tuple(VertexTypeContext)
                                << (DOMAIN_TYPE)V << *vertices[V].property.Type << *vertices[V].property.Context);
    }
    // other type/context pairs
    for (const auto &TCP : vertices[V].property.AdditionalNames) {
      if (TCP.Context->substr(0, 5) == "call#" || TCP.Context->substr(0, 13) == "resolvepoint#") {
        VertexTypeCall->insert(souffle::tuple(VertexTypeCall) << (DOMAIN_TYPE)V << *TCP.Type << *TCP.Context);
      } else {
        VertexTypeContext->insert(souffle::tuple(VertexTypeContext) << (DOMAIN_TYPE)V << *TCP.Type << *TCP.Context);
      }
    }
    for (const auto &E : out_edges(V)) {
      switch ((*this)[E].Type) {
      case CAST_SIMPLE:
      case REACHABILITY:
        if (E.source != E.target) {
          if (vertices[V].property.Type->find(VoidStr) != std::string::npos)
            VoidCast->insert(souffle::tuple(VoidCast) << (DOMAIN_TYPE)E.source << (DOMAIN_TYPE)E.target);
          else
            NonVoidCast->insert(souffle::tuple(NonVoidCast) << (DOMAIN_TYPE)E.source << (DOMAIN_TYPE)E.target);
          //SimpleCast->insert(souffle::tuple(SimpleCast) << (DOMAIN_TYPE)E.source << (DOMAIN_TYPE)E.target);
        }
        break;
      case POINTS_TO:
        PointsTo->insert(souffle::tuple(PointsTo) << (DOMAIN_TYPE)E.source << (DOMAIN_TYPE)E.target);
        break;
      case STRUCT_MEMBER:
        StructMember->insert(souffle::tuple(StructMember)
                             << (DOMAIN_TYPE)E.source << (DOMAIN_TYPE)E.target << (DOMAIN_TYPE)E.property.StructOffset);
        break;
      case UNION_MEMBER:
        UnionMember->insert(souffle::tuple(UnionMember) << (DOMAIN_TYPE)E.source << (DOMAIN_TYPE)E.target
                                                        << (DOMAIN_TYPE)SymbolContainer->getUniqueId(E.property.UnionType));
        break;
      }
    }
    for (const auto &F : vertices[V].property.FunctionUses) {
      FunctionUses->insert(souffle::tuple(FunctionUses)
                           << (DOMAIN_TYPE)V << *F.ContextName << (DOMAIN_TYPE)F.NumArgs << (DOMAIN_TYPE)(F.IsVarArg ? 1 : 0));
      if (F.IsDynamicFunction) {
        const auto &TargetSet = DynamicSymbolTargets.find(F.ContextName);
        if (TargetSet != DynamicSymbolTargets.end()) {
          for (const auto *Target: TargetSet->second) {
            const auto &InterfaceList = Interfaces.find(Target);
            if (InterfaceList != Interfaces.end()) {
              for (auto &Interface: InterfaceList->second) {
                FunctionUses->insert(souffle::tuple(FunctionUses)
                                     << (DOMAIN_TYPE)V << *Interface.ContextName << (DOMAIN_TYPE)Interface.Types.size() - 1
                                     << (DOMAIN_TYPE)(Interface.IsVarArg ? 1 : 0));
              }
            }
          }
        }
      }
    }
    if (StoreReachability && vertices[V].property.External) {
      External->insert(souffle::tuple(External) << (DOMAIN_TYPE)V);
    }
  }
  for (auto &It: PotentialExternalInterfaces) {
    for (Vertex V: It.second) {
      ExternalInterfaces->insert(souffle::tuple(ExternalInterfaces) << *It.first << (DOMAIN_TYPE) V);
    }
  }
  for (const auto &It : CallInfos) {
    for (auto V: It.second.AllVertices) {
      Calls->insert(souffle::tuple(Calls)
                    << (DOMAIN_TYPE)V << *It.first << (DOMAIN_TYPE)(It.second.IsResolvePoint ? 0xffff : It.second.NumArgs));
    }
  }

  Program->run();

  if (StorePossibleFunctions) {
    auto *PossibleFunctions = Program->getRelation("PossibleFunctions");
    assert(PossibleFunctions);
    for (auto &PossibleFunction : *PossibleFunctions) {
      DOMAIN_TYPE Call, Type;
      PossibleFunction >> Call >> Type;
      if (Call == Type)
        continue;
      for (const auto &F : vertices[Type].property.FunctionUses) {
        auto Usage = vertices[Call].property.FunctionUses.insert(F);
#ifndef WITHOUT_LLVM
        if (!Usage.second && F.Function && !Usage.first->Function) {
          Usage.first->Function = F.Function;
          Usage.first->NumArgs = F.Function->arg_size();
          Usage.first->IsVarArg = F.Function->isVarArg();
        }
#else
        (void)(Usage);
#endif
      }
    }
  }

  if (StoreReachability) {
    auto *ExternalReachability = Program->getRelation("ExternalReachability");
    auto *NewPointsTo = Program->getRelation("NewPointsTo");
    assert(ExternalReachability && NewPointsTo);
    for (auto &Er : *ExternalReachability) {
      DOMAIN_TYPE From, To;
      Er >> From >> To;
      if (From != To) {
        add_edge(From, To, TGEdge(CAST_SIMPLE)); // was: REACHABILITY
      }
    }
    for (auto &PT : *NewPointsTo) {
      DOMAIN_TYPE From, To;
      PT >> From >> To;
      if (From != To) {
        add_edge(From, To, TGEdge(POINTS_TO));
      }
    }
  }

  for (auto &Ex : *External) {
    DOMAIN_TYPE V;
    Ex >> V;
    vertices[V].property.External = true;
  }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"
void TypeGraph::markExternallyReachableCallsAndUses() {
  std::vector<Vertex> Sources;

  for (auto V : vertex_set()) {
    if (vertices[V].property.External)
      continue;
    if (vertices[V].property.HasIndirectCalls) {
      for (auto &E : in_edges(V)) {
        if ((E.property.Type == REACHABILITY || E.property.Type == CAST_SIMPLE) &&
            vertices[E.source].property.External) {
          Sources.push_back(V);
          continue;
        }
      }
    }
    if (vertices[V].property.FunctionUses.size() > 0) {
      for (auto &E : out_edges(V)) {
        if ((E.property.Type == REACHABILITY || E.property.Type == CAST_SIMPLE) &&
            vertices[E.target].property.External) {
          Sources.push_back(V);
          continue;
        }
      }
    }
  }

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
  basegraph::ColorMap Colors(*this);
  basegraph::breadth_first_search_forward(*this, Visitor, Sources, Colors);
}
#pragma clang diagnostic pop

std::unique_ptr<TypeGraph> TypeGraph::getMinifiedGraph(bool RemoveInternalContexts) {
  // Mark a few things as "external" that might have been missed before
  // for example: call information about dlsym functions
  for (auto V: vertex_set()) {
    if (!vertices[V].property.External && vertices[V].property.HasIndirectCalls) {
      for (auto &FU : vertices[V].property.FunctionUses) {
        if (FU.IsDynamicFunction) {
          vertices[V].property.External = true;
          break;
        }
      }
    }
  }

  std::vector<Vertex> OldToMinVertex(vertices.size(), NO_VERTEX);
  // Copy all external vertices to a new graph
  auto Graph = std::unique_ptr<TypeGraph>(new TypeGraph(SymbolContainer));
  Graph->CallGraph = CallGraph;
  for (auto V : vertex_set()) {
    if (vertices[V].property.External) {
      auto V2 = OldToMinVertex[V] = Graph->add_vertex(vertices[V].property);
      TypeContextToVertex[TypeContextPair(vertices[V].property.Type, vertices[V].property.Context,
                                          vertices[V].property.Layer)] = V2;
      // other type/context pairs
      for (const auto &TCP : vertices[V].property.AdditionalNames) {
        TypeContextToVertex[TypeContextPair(TCP.Type, TCP.Context, TCP.Layer)] = V2;
      }
    }
  }

  if (RemoveInternalContexts) {
    // find external contexts
    StringContainerMap<bool> Contexts(*Graph->SymbolContainer, false);
    for (auto &It: Interfaces) {
      for (auto &Interface: It.second) {
        if (Interface.IsExternal)
          Contexts[Interface.ContextName] = true;
      }
    }
    for (auto &It: CallInfos) {
      auto V2 = OldToMinVertex[It.second.V];
      if (V2 != NO_VERTEX) {
        Contexts[It.first] = true;
      }
    }
    // remove internal contexts
    size_t CountTGPairs = 0;
    size_t CountRemovedTGPairs = 0;
    for (auto &V: Graph->vertices) {
      if (V.empty) continue;

      CountTGPairs += 1 + V.property.AdditionalNames.size();
      for (ssize_t I = V.property.AdditionalNames.size() - 1; I >= 0; I--) {
        if (!Contexts[V.property.AdditionalNames[I].Context]) {
          if (I != (ssize_t) V.property.AdditionalNames.size() - 1)
            V.property.AdditionalNames[I] = V.property.AdditionalNames.back();
          V.property.AdditionalNames.pop_back();
          CountRemovedTGPairs++;
        }
      }
      if (!V.property.AdditionalNames.empty() && !Contexts[V.property.Context]) {
        V.property.Type = V.property.AdditionalNames.back().Type;
        V.property.Context = V.property.AdditionalNames.back().Context;
        V.property.Layer = V.property.AdditionalNames.back().Layer;
        V.property.ContextIsGlobal = V.property.AdditionalNames.back().ContextIsGlobal;
        V.property.VoidMergedWithVoid = V.property.AdditionalNames.back().VoidMergedWithVoid;
        V.property.AdditionalNames.pop_back();
        CountRemovedTGPairs++;
      }
    }

    fprintf(stderr, "getMinifiedGraph: Removed %ld / %ld type/context pairs (%.2f%%)\n", CountRemovedTGPairs,
            CountTGPairs, CountRemovedTGPairs * 100.0 / CountTGPairs);
  }

  // Copy all edges between them
  for (auto V : vertex_set()) {
    if (!vertices[V].property.External)
      continue;
    for (auto &E : out_edges(V)) {
      if (vertices[E.target].property.External) {
        Graph->add_edge_unique(OldToMinVertex[E.source], OldToMinVertex[E.target], E.property);
      }
    }
  }
  // Copy remaining stuff
  for (auto It : CallInfos) {
    auto V2 = OldToMinVertex[It.second.V];
    if (V2 != NO_VERTEX) {
      CallInfo CI(V2, It.second.NumArgs, It.second.IsResolvePoint);
      for (auto CV1: It.second.AllVertices) {
        auto CV2 = OldToMinVertex[CV1];
        if (CV2 != NO_VERTEX)
          CI.addVertex(CV2); // TODO unique?
      }
      Graph->CallInfos[It.first] = std::move(CI);
    }
  }
  for (auto It : Interfaces) {
    auto &Vec = Graph->Interfaces[It.first];
    for (auto &Interface : It.second) {
      if (!Interface.IsExternal)
        continue;
      auto &Interface2 = Vec.emplace_back(Interface);
      for (size_t I = 0; I < Interface2.Types.size(); I++) {
        if (Interface2.Types[I] != NO_VERTEX) {
          assert(OldToMinVertex[Interface2.Types[I]] != NO_VERTEX);
          Interface2.Types[I] = OldToMinVertex[Interface2.Types[I]];
        }
      }
    }
  }

  // Copy information about context origins
  Graph->ContextDefiningUnits.insert(ContextDefiningUnits.begin(), ContextDefiningUnits.end());

  return Graph;
}

std::unique_ptr<TypeGraph> TypeGraph::computeEquivalenceClasses(bool KeepOnlyExternalNames, int NumThreads) {
  std::unique_ptr<souffle::SouffleProgram> Program(souffle::ProgramFactory::newInstance("compute_equivalence"));
  assert(Program.get());
  Program->setNumThreads(NumThreads);

  auto *SimpleCast = Program->getRelation("SimpleCast");
  auto *PointsTo = Program->getRelation("PointsTo");
  auto *StructMember = Program->getRelation("StructMember");
  assert(SimpleCast && PointsTo && StructMember);

  for (const auto V : vertex_set()) {
    for (const auto &E : out_edges(V)) {
      // assert(E.source >= 0 && E.source < vertices.size());
      // assert(E.target >= 0 && E.target < vertices.size());
      switch ((*this)[E].Type) {
      case CAST_SIMPLE:
      case REACHABILITY:
        SimpleCast->insert(souffle::tuple(SimpleCast) << (DOMAIN_TYPE)E.source << (DOMAIN_TYPE)E.target);
        break;
      case POINTS_TO:
        PointsTo->insert(souffle::tuple(PointsTo) << (DOMAIN_TYPE)E.source << (DOMAIN_TYPE)E.target);
        break;
      case STRUCT_MEMBER:
        StructMember->insert(souffle::tuple(StructMember)
                             << (DOMAIN_TYPE)E.source << (DOMAIN_TYPE)E.target << (DOMAIN_TYPE)E.property.StructOffset);
        break;
      case UNION_MEMBER:
        break;
      }
    }
  }

  Program->run();

  auto *Equivalence = Program->getRelation("Equivalence");
  assert(Equivalence);
  auto Graph = std::unique_ptr<TypeGraph>(new TypeGraph(SymbolContainer));
  std::vector<Vertex> Classes(vertices.size(), NO_VERTEX);
  for (auto &Er : *Equivalence) {
    DOMAIN_TYPE From, To;
    Er >> From >> To;
    // assert(From >= 0 && From < Classes.size());
    // assert(To >= 0 && To < Classes.size());
    auto C = Classes[From];
    if (C == NO_VERTEX) {
      Classes[From] = C = Graph->createVertex(vertices[From].property);
    }
    if (Classes[To] == NO_VERTEX) {
      Classes[To] = C;
      Graph->joinVertex(C, vertices[To].property, KeepOnlyExternalNames);
    }
    // std::cerr << "EQ: " << From << " " << To << "\n";
  }
  for (auto V : vertex_set()) {
    if (Classes[V] == NO_VERTEX)
      Classes[V] = Graph->createVertex(vertices[V].property);
  }
  std::cerr << "Equivalence computation: " << num_vertices() << " => " << Graph->num_vertices() << " classes"
            << std::endl;

  // Copy all edges between these vertices
  for (auto V : vertex_set()) {
    for (auto &E : out_edges(V)) {
      auto From = Classes[E.source];
      auto To = Classes[E.target];
      if (From == To && (E.property.Type == EdgeType::CAST_SIMPLE || E.property.Type == EdgeType::REACHABILITY))
        continue;
      Graph->add_edge(From, To, E.property);
    }
  }
  // Copy remaining stuff
  for (auto It : CallInfos) {
    CallInfo CI(Classes[It.second.V], It.second.NumArgs);
    for (auto V: It.second.AllVertices) {
      CI.addVertex(Classes[V]);
    }
    Graph->CallInfos[It.first] = std::move(CI);
  }
  for (auto It : Interfaces) {
    auto &Vec = Graph->Interfaces[It.first];
    for (auto &Interface : It.second) {
      if (!Interface.IsExternal)
        continue;
      auto &Interface2 = Vec.emplace_back(Interface);
      for (size_t I = 0; I < Interface2.Types.size(); I++) {
        if (Interface2.Types[I] != NO_VERTEX) {
          assert(Classes[Interface2.Types[I]] != NO_VERTEX);
          Interface2.Types[I] = Classes[Interface2.Types[I]];
        }
      }
    }
  }

  return Graph;
}

namespace {

// #define DEBUG_EQUIVALENCE

struct CombineEquivalencesWorker {
  TypeGraph &Graph;
  bool KeepOnlyExternalNames;
  bool RespectDefiningUnits;
  bool RespectTypes = false;
  std::vector<Vertex> OldToNewVertex;
  long JoinedNodes = 0;
  std::queue<Vertex> Queue;
  std::vector<bool> Enqueued;

  const std::string *NonJoinableNames[2];

#ifdef DEBUG_EQUIVALENCE
  Vertex SourceNode = Graph.NO_VERTEX;
  Vertex TargetNode = Graph.NO_VERTEX;
  Vertex CurrentNode = Graph.NO_VERTEX;
  MapCls<Vertex, SetCls<Vertex>> MergedIntoNode;
#endif

  CombineEquivalencesWorker(TypeGraph &Graph, bool KeepOnlyExternalNames, bool RespectDefiningUnits)
      : Graph(Graph), KeepOnlyExternalNames(KeepOnlyExternalNames), RespectDefiningUnits(RespectDefiningUnits),
        OldToNewVertex(Graph.vertices.size(), TypeGraph::NO_VERTEX), Enqueued(Graph.vertices.size(), true) {
    NonJoinableNames[0] = Graph.SymbolContainer->get("void *");
    NonJoinableNames[1] = Graph.SymbolContainer->get("const void *");
  }

  inline bool isJoinable(Vertex V) {
    if (RespectTypes) return true;
    const auto *T = Graph[V].Type;
    return T != NonJoinableNames[0] && T != NonJoinableNames[1];
  }

  inline bool isJoinable(Vertex V1, Vertex V2) {
    if (
#ifndef WITHOUT_LLVM
        Settings.linktime_layering &&
#endif
        Graph[V1].ContextIsGlobal != Graph[V2].ContextIsGlobal)
      return false;
    if (RespectTypes) {
      if (Graph[V1].Type != Graph[V2].Type)
        return false;
    }
    if (RespectDefiningUnits) {
      const auto *U1 = Graph.getDefiningUnit(V1);
      const auto *U2 = Graph.getDefiningUnit(V2);
      return U1 && U2 && U1 == U2;
    }
    return true;
  }

  bool join(Vertex V, Vertex V2, Vertex VMain) {
    if (V == V2)
      return false;
    // ...?
    if (!isJoinable(V) && !isJoinable(V2)) {
      if (Graph[V].VoidMergedWithVoid && Graph[V2].VoidMergedWithVoid) {
        return false;
      }
      Graph[V].VoidMergedWithVoid = true;
    }
    // Check if both vertices come from global/nonglobal context or different compilation units
    if (!isJoinable(V, V2))
      return false;
    if (Graph.vertices[V].empty || Graph.vertices[V2].empty) {
      if (!Enqueued[VMain]) {
        Queue.push(VMain);
        Enqueued[VMain] = true;
      }
      return false;
    }
    Graph.joinVertex(V, Graph[V2], KeepOnlyExternalNames);
    Graph.replace_vertex_uses(V2, V);
    Graph.remove_vertex(V2);
    OldToNewVertex[V2] = V;
    JoinedNodes++;
    if (!Enqueued[V]) {
      Queue.push(V);
      Enqueued[V] = true;
    }
#ifdef DEBUG_EQUIVALENCE
    if (CurrentNode != TargetNode) {
      if (V != CurrentNode) {
        MergedIntoNode[V].insert(V2);
        MergedIntoNode[V].insert(MergedIntoNode[V2].begin(), MergedIntoNode[V2].end());
      }
      if (V2 == CurrentNode) {
        CurrentNode = V;
        if (V == TargetNode) {
          while (!Queue.empty()) Queue.pop();
        }
      }
      if (V2 == TargetNode)
        TargetNode = V;
    }
#endif
    return true;
  }

  void progressVertex(Vertex V) {
    //if (!isJoinable(V))
    //  return;
    auto &OutEdges = Graph.out_edges(V);
    if (OutEdges.size() == 0)
      return;

    Vertex Pointee = TypeGraph::NO_VERTEX;
    MapCls<int, Vertex> StructMembers;
    MapCls<const std::string *, Vertex> UnionTypes;
    SetCls<Vertex> CastedTo;
    // Case 1: X->*Y, X->*Z: Y=Z
    for (size_t I = 0; I < OutEdges.size(); I++) {
      const auto &E = OutEdges[I];
      switch (Graph[E].Type) {
      case POINTS_TO:
        if (Pointee == TypeGraph::NO_VERTEX) {
          Pointee = E.target;
        } else {
          if (join(Pointee, E.target, V))
            --I;
        }
        break;
      case STRUCT_MEMBER: {
        // Case 1.2: X->.x1, X->.x2: x1=x2
        auto It = StructMembers.find(Graph[E].StructOffset);
        if (It == StructMembers.end()) {
          StructMembers[Graph[E].StructOffset] = E.target;
        } else {
          if (join(It->second, E.target, V))
            --I;
        }
        break;
      }
      case UNION_MEMBER: {
        // Case 1.3: X->.x1, X->.x2: x1=x2
        auto It = UnionTypes.find(Graph[E].UnionType);
        if (It == UnionTypes.end()) {
          UnionTypes[Graph[E].UnionType] = E.target;
        } else {
          if (join(It->second, E.target, V))
            --I;
        }
        break;
      }
      case CAST_SIMPLE:
      case REACHABILITY:
        CastedTo.insert(E.target);
        break;
      }
    }
    if (Pointee != TypeGraph::NO_VERTEX) {
      // Case 2: X->*x2, Y->*y2, X->Y: x2=y2.     V --> V2 --E2-->* ...
      // first for outgoing X->Y edges
      for (Vertex V2 : CastedTo) {
        if (V2 == V || V2 == Pointee || Graph.vertices[V2].empty)
          continue;
        auto &OutEdges2 = Graph.out_edges(V2);
        for (size_t J = 0; J < OutEdges2.size(); J++) {
          const auto &E2 = OutEdges2[J];
          if (Graph[E2].Type == POINTS_TO) {
            if (E2.target != V2 && E2.target != V && E2.target != Pointee) {
              if (join(Pointee, E2.target, V))
                --J;
            }
          }
        }
      }
      // then for incoming X<-Y edges
      //*
      auto &InEdges = Graph.in_edges(V);
      for (size_t I = 0; I < InEdges.size(); I++) {
        const auto &E = InEdges[I];
        if (E.source == V)
          continue;
        if (Graph[E].Type == CAST_SIMPLE || Graph[E].Type == REACHABILITY) {
          auto &OutEdges2 = Graph.out_edges(E.source);
          for (size_t J = 0; J < OutEdges2.size(); J++) {
            const auto &E2 = OutEdges2[J];
            if (Graph[E2].Type == POINTS_TO) {
              if (E2.target != E.source && E2.target != V && E2.target != Pointee) {
                // std::cerr << "RULE 4 V=" << V << " Pointee=" << Pointee << ". " << E.source << " -> " << E.target <<
                // " ; " << E.source << " -*-> " << E2.target << " ; " << V << " -*-> " << Pointee << "\n";
                if (join(Pointee, E2.target, V))
                  --J;
              }
            }
          }
        }
      }
      // */
    }
    // Rule: Combine nodes X <-> Y (both-sided cast)
    //*
    if (!CastedTo.empty()) {
      auto &InEdges = Graph.in_edges(V);
      for (size_t I = 0; I < InEdges.size(); I++) {
        const auto &E = InEdges[I];
        if (E.source == V)
          continue;
        if ((Graph[E].Type == CAST_SIMPLE || Graph[E].Type == REACHABILITY) &&
            CastedTo.find(E.source) != CastedTo.end()) {
          if (join(V, E.source, V))
            --I;
        }
      }
    }
    // */
  }

  void combineEquivalences() {
    for (auto V : Graph.vertex_set()) {
      Enqueued[V] = false;
      progressVertex(V);
    }
    while (!Queue.empty()) {
      auto V = Queue.front();
      Queue.pop();
      Enqueued[V] = false;
      if (!Graph.vertices[V].empty) {
        progressVertex(V);
      }
    }
    adjustAdditionalFields();
  }

  void combineEquivalencesSimple() {
    for (auto V : Graph.vertex_set()) {
      progressVertex(V);
    }
    adjustAdditionalFields();
  }

  inline Vertex getNewVertex(Vertex V) {
    Vertex Last = V;
    while (V != TypeGraph::NO_VERTEX) {
      Last = V;
      V = OldToNewVertex[Last];
    }
    return Last;
  }

  void adjustAdditionalFields() {
    for (auto &CallIt : Graph.CallInfos) {
      CallIt.second.V = getNewVertex(CallIt.second.V);
      for (auto &V: CallIt.second.AllVertices) {
        V = getNewVertex(V);
      }
    }
    for (auto &InterfaceList : Graph.Interfaces) {
      for (auto &Interface : InterfaceList.second) {
        for (auto &InterfaceVertex : Interface.Types) {
          InterfaceVertex = getNewVertex(InterfaceVertex);
        }
      }
    }
    for (auto &It : Graph.getTypeContextToVertex()) {
      It.second = getNewVertex(It.second);
    }
    for (auto &It: Graph.PotentialExternalInterfaces) {
      SetCls<Vertex> NewNodes;
      for (auto V: It.second) {
        auto V2 = getNewVertex(V);
        if (V != TypeGraph::NO_VERTEX && V2 != TypeGraph::NO_VERTEX) {
          NewNodes.insert(V2);
        }
      }
      It.second = std::move(NewNodes);
    }
  }

#ifdef DEBUG_EQUIVALENCE
  void combineEquivalencesDebug() {
    SetCls<Vertex> S;
    S.insert(SourceNode);
    for (auto &E: Graph.out_edges(SourceNode)) S.insert(E.target);
    for (auto &E: Graph.in_edges(SourceNode)) S.insert(E.source);
    S.insert(TargetNode);
    for (auto &E: Graph.out_edges(TargetNode)) S.insert(E.target);
    for (auto &E: Graph.in_edges(TargetNode)) S.insert(E.source);
    for (auto V : Graph.vertex_set()) {
      Enqueued[V] = false;
    }
    for (auto V: S) {
      Queue.push(V);
      Enqueued[V] = true;
    }
    while (!Queue.empty()) {
      auto V = Queue.front();
      Queue.pop();
      Enqueued[V] = false;
      if (!Graph.vertices[V].empty) {
        progressVertex(V);
      }
    }
    adjustAdditionalFields();
  }
#endif
};
} // namespace

void TypeGraph::combineEquivalencesInline(bool KeepOnlyExternalNames, bool RespectDefiningUnits, bool RespectTypes) {
  CombineEquivalencesWorker Worker(*this, KeepOnlyExternalNames, RespectDefiningUnits);
#ifdef DEBUG_EQUIVALENCE
  Worker.TargetNode = 14733;
  Worker.SourceNode = 11470;
  //*
  CombineEquivalencesWorker Worker2(*this, KeepOnlyExternalNames, RespectDefiningUnits);
  Worker2.RespectTypes = true;
  Worker2.combineEquivalences();
  Worker.TargetNode = Worker2.getNewVertex(Worker.TargetNode);
  Worker.SourceNode = Worker2.getNewVertex(Worker.SourceNode);
  // */
  Worker.CurrentNode = Worker.SourceNode;
  llvm::errs() << "Tracing... from " << Worker.SourceNode << " = (" << *vertices[Worker.SourceNode].property.Type
               << " , " << *vertices[Worker.SourceNode].property.Context << ") to " << Worker.TargetNode << " = ("
               << *vertices[Worker.TargetNode].property.Type << " , " << *vertices[Worker.TargetNode].property.Context
               << ")...\n";
  Worker.combineEquivalences();
  llvm::errs() << "Equivalence done. " << Worker.SourceNode << " -> " << Worker.TargetNode << " reached " << Worker.CurrentNode << " and collected " << Worker.MergedIntoNode[Worker.TargetNode].size() << " nodes\n";
  llvm::errs() << "Using nodes: " << Worker.TargetNode;
  for (auto V: Worker.MergedIntoNode[Worker.TargetNode]) llvm::errs() << ", " << V;
  llvm::errs() << "\n\n";
  if (Worker.CurrentNode != Worker.TargetNode) {
    llvm::errs() << "Current node: " << Worker.CurrentNode;
    for (auto V: Worker.MergedIntoNode[Worker.CurrentNode]) llvm::errs() << ", " << V;
    llvm::errs() << "\n\n";
  }
  exit(0);
#else
  Worker.RespectTypes = RespectTypes;
  Worker.combineEquivalences();
#endif
  /*for (int I = 0; I < 10; I++) {
    CombineEquivalencesWorker Worker(*this, KeepOnlyExternalNames);
    Worker.combineEquivalences();
    std::cerr << "Worker " << Worker.JoinedNodes << " removed.\n";
    if (Worker.JoinedNodes == 0)
      break;
  }*/
}

long TypeGraph::getNumExternalVertices() const {
  long Counter = 0;
  for (auto V: vertex_set()) {
    if (vertices[V].property.External) Counter++;
  }
  return Counter;
}

} // namespace typegraph

// Force linking of souffle programs
extern "C" {
extern char __factory_Sf_compute_function_uses_instance;
#ifndef WITHOUT_LLVM
extern char __factory_Sf_compute_function_uses_simple_instance;
extern char __factory_Sf_compute_equivalence_instance;
#endif
}
void use_souffle_programs() {
  __factory_Sf_compute_function_uses_instance = 0;
#ifndef WITHOUT_LLVM
  __factory_Sf_compute_function_uses_simple_instance = 0;
  __factory_Sf_compute_equivalence_instance = 0;
#endif
}
