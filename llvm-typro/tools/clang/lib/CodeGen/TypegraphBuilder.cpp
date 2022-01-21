#include "TypegraphBuilder.h"
#include "CGCall.h"
#include "CodeGenModule.h"
#include "TypegraphFunctionInfos.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/RecordLayout.h"
#include "clang/AST/StmtVisitor.h"
#include "clang/Basic/Builtins.h"
#include "llvm/IR/Module.h"
#include "llvm/Typegraph/TGCallGraph.h"
#include "llvm/Typegraph/Typegraph.h"
#include "llvm/Typegraph/TypegraphSettings.h"
#include "llvm/Typegraph/timeclock.h"
#include "llvm/Typegraph/typegraph_llvm_utils.h"
#include <fstream>
#include <functional>
#include <llvm/Typegraph/typegraph_layering.h>

namespace {
inline bool startsWith(const std::string &check, const std::string &prefix) {
  return std::equal(prefix.begin(), prefix.end(), check.begin());
}

inline bool endsWith(std::string const &fullString, std::string const &ending) {
  if (fullString.length() >= ending.length()) {
    return (0 == fullString.compare(fullString.length() - ending.length(),
                                    ending.length(), ending));
  }
  return false;
}

inline clang::QualType cleanType(clang::QualType Ty) {
  return Ty.getCanonicalType();
}

inline std::string qualifierLessTypeAsString(clang::QualType Ty) {
  return Ty.getCanonicalType().getUnqualifiedType().getAsString();
}

llvm::Value *pullFunctionFromValue(llvm::Value *V) {
  if (auto *V2 = llvm::dyn_cast<llvm::BitCastOperator>(V)) {
    V = V2->getOperand(0);
  }
  return V;
}

} // namespace

namespace llvm {
template <> struct DenseMapInfo<clang::TypeGraphContext> {
  static inline clang::TypeGraphContext getEmptyKey() {
    return clang::TypeGraphContext(0L);
  }

  static inline clang::TypeGraphContext getTombstoneKey() {
    return clang::TypeGraphContext(-1L);
  }

  static unsigned getHashValue(const clang::TypeGraphContext &PtrVal) {
    switch (PtrVal.kind) {
    case clang::TypeGraphContext::TypeGraphContextKind::DECLARATION:
      return ((uintptr_t)PtrVal.decl) >> 4;
    case clang::TypeGraphContext::TypeGraphContextKind::CALLINST:
      return ((uintptr_t)PtrVal.call) >> 4;
    case clang::TypeGraphContext::TypeGraphContextKind::NUMERIC:
      return (unsigned)PtrVal.number;
    }
    abort();
  }

  static bool isEqual(const clang::TypeGraphContext &LHS,
                      const clang::TypeGraphContext &RHS) {
    return LHS == RHS;
  }
};

template <> struct DenseMapInfo<clang::TypeGraphEntry> {
  static inline clang::TypeGraphEntry getEmptyKey() {
    return clang::TypeGraphEntry(clang::QualType((clang::Type *)nullptr, 0), DenseMapInfo<clang::TypeGraphContext>::getEmptyKey());
  }

  static inline clang::TypeGraphEntry getTombstoneKey() {
    return clang::TypeGraphEntry(clang::QualType((clang::Type *)nullptr, 0), DenseMapInfo<clang::TypeGraphContext>::getTombstoneKey());
  }

  static unsigned getHashValue(const clang::TypeGraphEntry &PtrVal) {
    return DenseMapInfo<clang::TypeGraphContext>::getHashValue(PtrVal.context) + DenseMapInfo<clang::QualType>::getHashValue(PtrVal.type);
  }

  static bool isEqual(const clang::TypeGraphEntry &LHS,
                      const clang::TypeGraphEntry &RHS) {
    return LHS == RHS;
  }
};
} // namespace llvm

using namespace clang::CodeGen;

namespace clang {

bool TypeGraphContext::operator==(const TypeGraphContext &other) const {
  switch (kind) {
  case TypeGraphContextKind::DECLARATION:
    if (other.kind == TypeGraphContextKind::DECLARATION) {
      if (decl == other.decl)
        return true;
      const auto *d1 = decl;
      const auto *d2 = other.decl;
      while (d1->getPreviousDecl())
        d1 = d1->getPreviousDecl();
      while (d2->getPreviousDecl())
        d2 = d2->getPreviousDecl();
      return d1 == d2;
    }
    return false;
  case TypeGraphContextKind::CALLINST:
    return other.kind == TypeGraphContextKind::CALLINST &&
           call == other.call && IsResolvePoint == other.IsResolvePoint;
  case TypeGraphContextKind::NUMERIC:
    return other.kind == TypeGraphContextKind::NUMERIC &&
           number == other.number;
  }
  llvm_unreachable("TypeGraphContextKind");
}

bool TypeGraphContext::operator<(const TypeGraphContext &other) const {
  if (kind != other.kind)
    return kind < other.kind;
  switch (kind) {
  case TypeGraphContextKind::DECLARATION: {
    /*const auto *pd1 = decl->getPreviousDecl();
    const auto *pd2 = other.decl->getPreviousDecl();
    return (pd1 ? pd1 : decl) < (pd2 ? pd2 : other.decl);*/
    const auto *d1 = decl;
    const auto *d2 = other.decl;
    while (d1->getPreviousDecl())
      d1 = d1->getPreviousDecl();
    while (d2->getPreviousDecl())
      d2 = d2->getPreviousDecl();
    return d1 < d2;
  }
  case TypeGraphContextKind::CALLINST:
    return call < other.call;
  case TypeGraphContextKind::NUMERIC:
    return number < other.number;
  }
  llvm_unreachable("TypeGraphContextKind");
}

typedef long int Vertex;

struct Edge {
  Vertex target = 0;
  typegraph::EdgeType type;
  union {
    int struct_offset;
    const std::string* union_type;
  };

  Edge(typegraph::EdgeType type) : type(type) {}
  Edge(const Edge &E) : target(E.target), type(E.type) {
    if (type == typegraph::STRUCT_MEMBER) struct_offset = E.struct_offset;
    else if (type == typegraph::UNION_MEMBER) union_type = E.union_type;
  }

  inline bool operator==(const Edge &other) const {
    if (target != other.target || type != other.type)
      return false;
    if (type == typegraph::EdgeType::STRUCT_MEMBER)
      return struct_offset == other.struct_offset;
    if (type == typegraph::EdgeType::UNION_MEMBER)
      return union_type == other.union_type;
    return true;
  }
  inline bool operator<(const Edge &other) const {
    if (target == other.target) {
      if (type == other.type) {
        return (type == typegraph::EdgeType::STRUCT_MEMBER &&
                struct_offset < other.struct_offset) ||
               (type == typegraph::EdgeType::UNION_MEMBER &&
                union_type < other.union_type);
      }
      return type < other.type;
    }
    return target < other.target;
  }
};

struct FunctionUseRef {
  const NamedDecl *functionDecl;
  llvm::Value *functionRef;

  FunctionUseRef(const NamedDecl *functionDecl, llvm::Value *fr)
      : functionDecl(functionDecl), functionRef(fr) {}

  inline bool operator==(const FunctionUseRef &other) const {
    return functionRef == other.functionRef &&
           functionDecl == other.functionDecl;
  }
  inline bool operator<(const FunctionUseRef &other) const {
    if (functionRef == other.functionRef)
      return functionDecl < other.functionDecl;
    return functionRef < other.functionRef;
  }
};

struct VertexData {
  TypeGraphEntry entry;
  std::set<Edge> edges;
  std::set<FunctionUseRef> functions;
  std::set<llvm::CallBase *> dynamicFunctions;
  std::set<const std::string *> argumentReturnTypes;

  VertexData(const TypeGraphEntry &entry) : entry(entry) {}
};

constexpr Vertex NO_VERTEX = -1;

class TypeGraphRepr {
public:
  std::vector<VertexData> vertices;
  std::map<TypeGraphEntry, Vertex> vertexLookupTable;
  std::map<llvm::CallBase *, Vertex> indirectCallToCalleeType;
  std::map<llvm::CallBase *, std::vector<std::pair<int, Vertex>>> resolvePoints; // LLVM call, list of (argnum, Vertex)

  llvm::DenseSet<GlobalDecl> interfaces;
  llvm::DenseMap<GlobalDecl, std::vector<Vertex>> usedInterfaces;
  llvm::DenseSet<const Decl *> usedFunctions;
  llvm::DenseMap<TypeGraphContext, llvm::DenseSet<TypeGraphContext>> callGraph;

  bool expandLater = true;
  llvm::DenseSet<TypeGraphEntry> unexpandedEntries;

  std::shared_ptr<typegraph::StringContainer> SymbolContainer = std::make_shared<typegraph::StringContainer>();

  ASTContext *C;

  /**
   * As optimization, we exclude some types (that are too small to carry enough
   * important data)
   * @param type
   * @return
   */
  bool includeType(const QualType &type) {
    static uint64_t PtrBitSize = C->getTypeSize(C->getSizeType());
    if (type->isVoidType() || type->isFloatingType())
      return false;
    if (type->isPointerType() || type->isArrayType() ||
        type->isIncompleteType() || type->isFunctionType())
      return true;
    if (type->isRecordType() &&
        type->getAsRecordDecl()->getDefinition() == nullptr)
      return true;
    auto size = C->getTypeSize(type);
    if (size < PtrBitSize) {
      // llvm::errs() << "Exclude " << type.getAsString() << " because " <<
      // C->getTypeSize(type) << " < " << PtrBitSize << "\n";
      return false;
    }
    return true;
  }

  Vertex getOrInsertEntry(const TypeGraphEntry &entry) {
    auto newV = includeType(entry.type) ? vertices.size() : NO_VERTEX;
    auto result = vertexLookupTable.insert(
        std::pair<TypeGraphEntry, Vertex>(entry, newV));
    if (result.second && result.first->second != NO_VERTEX) {
      // assert cleanType has been applied properly
      assert(entry.type.getAsString() == cleanType(entry.type).getAsString());
      vertices.push_back(entry);
      generateSubtypes(entry, result.first->second);
    }
    return result.first->second;
  }

  bool addGraphEdge(TypeGraphEntry src, TypeGraphEntry dst, const Edge &e) {
    if (src == dst || src.type.isNull() || dst.type.isNull()) {
      return false;
    }
    // save
    auto a = getOrInsertEntry(src);
    auto b = getOrInsertEntry(dst);
    return addGraphEdge(a, b, e);
  }

  bool addGraphEdge(Vertex a, Vertex b, Edge e) {
    if (a == NO_VERTEX || b == NO_VERTEX)
      return false;
    e.target = b;
    vertices[a].edges.insert(e);
    return true;
  }

  void generateSubtypes(const TypeGraphEntry &entry, Vertex v) {
    if (entry.type.isNull())
      return;
    if (entry.type->isPointerType() && !entry.type->isVoidPointerType() && !entry.type->isFunctionPointerType()) {
      TypeGraphEntry sub(cleanType(entry.type->getPointeeType()), entry.context);
      auto v2 = getOrInsertEntry(sub);
      addGraphEdge(v, v2, Edge(typegraph::EdgeType::POINTS_TO));
    } else if (entry.type->isReferenceType()) {
      TypeGraphEntry sub(cleanType(entry.type.getNonReferenceType()), entry.context);
      auto v2 = getOrInsertEntry(sub);
      addGraphEdge(v, v2, Edge(typegraph::EdgeType::POINTS_TO));
    } else if (entry.type->isArrayType()) {
      auto T = cleanType(entry.type->getAsArrayTypeUnsafe()->getElementType());
      T.addFastQualifiers(entry.type.getCVRQualifiers());
      auto v2 = getOrInsertEntry(TypeGraphEntry(T, entry.context));
      addGraphEdge(v, v2, Edge(typegraph::EdgeType::POINTS_TO));
    } else if (entry.type->isUnionType()) {
      const RecordDecl *D = entry.type->getAsRecordDecl()->getDefinition();
      if (D) {
        for (auto *Field : D->fields()) {
          auto T = cleanType(Field->getType());
          T.addFastQualifiers(entry.type.getCVRQualifiers());
          auto v2 = getOrInsertEntry(TypeGraphEntry(T, entry.context));
          Edge e(typegraph::EdgeType::UNION_MEMBER);
          e.union_type = SymbolContainer->get(qualifierLessTypeAsString(Field->getType()));
          addGraphEdge(v, v2, e);
        }
      } else if (expandLater) {
        unexpandedEntries.insert(entry);
      }
    } else if (entry.type->isRecordType()) {
      const RecordDecl *D = entry.type->getAsRecordDecl()->getDefinition();
      if (D) {
        auto &Layout = C->getASTRecordLayout(D);
        for (auto *Field : D->fields()) {
          // a struct member has const/volatile qualifiers, if its field decl
          // is qualified or the struct variable accessed is qualified.
          // For example: members of a "const struct X" are always const.
          // RecordDecl doesn't follow these qualifiers.
          auto T = cleanType(Field->getType());
          T.addFastQualifiers(entry.type.getCVRQualifiers());
          auto v2 = getOrInsertEntry(TypeGraphEntry(T, entry.context));
          Edge e(typegraph::EdgeType::STRUCT_MEMBER);
          e.struct_offset = Layout.getFieldOffset(Field->getFieldIndex()) / 8;
          addGraphEdge(v, v2, e);
        }
      } else if (expandLater) {
        unexpandedEntries.insert(entry);
      }
    }
  }

  void generateEssentialSubtypes(QualType type,
                                 const TypeGraphContext &context) {
    if (type.isNull())
      return;
    if (type->isPointerType() && !type->isVoidPointerType() &&
        !type->isFunctionPointerType()) {
      generateEssentialSubtypes(type->getPointeeType(), context);
    } else if (type->isReferenceType()) {
      generateEssentialSubtypes(type.getNonReferenceType(), context);
    } else if (type->isArrayType()) {
      auto T = cleanType(type->getAsArrayTypeUnsafe()->getElementType());
      T.addFastQualifiers(type.getCVRQualifiers());
      generateEssentialSubtypes(T, context);
    } else if (type->isRecordType()) {
      const RecordDecl *D = type->getAsRecordDecl();
      if (D->isUnion()) {
        getOrInsertEntry(TypeGraphEntry(type, context));
      } else {
        // TODO for now
        getOrInsertEntry(TypeGraphEntry(type, context));
        /*for (auto *Field : D->fields()) {
          generateEssentialSubtypes(cleanType(Field->getType()), context);
        }*/
      }
    }
  }

  void expandAll() {
    // no concurrent modifications
    expandLater = false;

    for (auto &TGE: unexpandedEntries) {
      auto V = vertexLookupTable.find(TGE);
      if (V != vertexLookupTable.end()) {
        generateSubtypes(TGE, V->second);
      }
    }
  }

  Vertex getPointee(const TypeGraphEntry &TGE) {
    auto V = getOrInsertEntry(TGE);
    for (auto &E: vertices[V].edges) {
      if (E.type == typegraph::POINTS_TO)
        return E.target;
    }
    return NO_VERTEX;
  }
};

raw_ostream &operator<<(raw_ostream &OS, const TypeGraphEntry &entry) {
  if (entry.type.isNull())
    return OS << "(<null> @ " << entry.context << ")";
  return OS << "(" << entry.type.getAsString() << " @ " << entry.context << ")";
}

raw_ostream &operator<<(raw_ostream &OS, const TypeGraphContext &context) {
  switch (context.kind) {
  case TypeGraphContext::DECLARATION: {
    const auto *named = dyn_cast<NamedDecl>(context.decl);
    if (named) {
      if (!named->getDeclName()) {
        named->dump(OS);
        return OS;
      }
      return OS << named->getName();
    }
    return OS << "#" << context.decl->getID();
  }
  case TypeGraphContext::CALLINST:
    return OS << context.call;
  case TypeGraphContext::NUMERIC:
    return OS << "N" << context.number;
  }
  llvm_unreachable("TypeGraphContextKind");
}

class Serializer {
public:
  std::string module_unique_value;
  const std::map<const llvm::CallBase *, int> &call_numbers;
  CodeGenModule &CGM;

  Serializer(llvm::Module &M, CodeGenModule &CGM,
             const std::map<const llvm::CallBase *, int> &call_numbers)
      : module_unique_value(M.getName()), call_numbers(call_numbers), CGM(CGM) {
  }

  std::string serialize(const llvm::CallBase *call) {
    auto funcname = call->getFunction()->getName().str();
    if (call->getFunction()->hasLocalLinkage()) {
      funcname = "internal " + funcname + "@" + module_unique_value;
    }
    const auto &it = call_numbers.find(call);
    auto number = it != call_numbers.cend() ? std::to_string(it->second)
                                            : std::to_string((uintptr_t)call);
    return "call#" + std::to_string(call->getValueID()) + "." + number +
           " in " + funcname;
  }

  /**
   * Serialize resolve points
   * @param call
   * @param argnum
   * @return
   */
  std::string serialize(const llvm::CallBase *call, int argnum) {
    auto funcname = call->getFunction()->getName().str();
    if (call->getFunction()->hasLocalLinkage()) {
      funcname = "internal " + funcname + "@" + module_unique_value;
    }
    const auto &it = call_numbers.find(call);
    auto number = it != call_numbers.cend() ? std::to_string(it->second)
                                            : std::to_string((uintptr_t)call);
    return "resolvepoint#" + std::to_string(call->getValueID()) + "." +
           number + "." + std::to_string(argnum) + " in " + funcname;
  }

  std::string serializeDynamicFunction(const llvm::CallBase *call) {
    auto funcname = call->getFunction()->getName().str();
    if (call->getFunction()->hasLocalLinkage()) {
      funcname = "internal " + funcname + "@" + module_unique_value;
    }
    const auto &it = call_numbers.find(call);
    auto number = it != call_numbers.cend() ? std::to_string(it->second)
                                            : std::to_string((uintptr_t)call);
    return "dynamicsymbol#" + std::to_string(call->getValueID()) + "." +
           number + " in " + funcname;
  }

  std::string serialize(const NamedDecl *ND) {
    std::string name = "unknown";
    bool isInternal = !ND->isExternallyVisible();
    if (const auto *CD = dyn_cast_or_null<CXXConstructorDecl>(ND)) {
      name = CGM.getMangledName(GlobalDecl(CD, CXXCtorType::Ctor_Base));
    } else if (const auto *DD = dyn_cast_or_null<CXXDestructorDecl>(ND)) {
      name = CGM.getMangledName(GlobalDecl(DD, CXXDtorType::Dtor_Base));
    } else if (const auto *FD = dyn_cast_or_null<FunctionDecl>(ND)) {
      name = CGM.getMangledName(GlobalDecl(FD)).str();
    } else if (const auto *VD = dyn_cast_or_null<VarDecl>(ND)) {
      name = CGM.getMangledName(GlobalDecl(VD)).str();
      if (VD->getLinkageAndVisibility().getLinkage() ==
              Linkage::InternalLinkage ||
          VD->isInAnonymousNamespace())
        isInternal = true;
    } else if (ND->getDeclName().isIdentifier()) {
      name = ND->getName().str();
    } else {
      llvm::errs() << "Unknown named decl: ";
      ND->dump(llvm::errs());
    }
    if (isInternal) {
      name = "internal " + name + "@" + module_unique_value;
    }
    return name;
  }

  std::string serialize(const TypeGraphContext &ctx) {
    switch (ctx.kind) {
    case TypeGraphContext::DECLARATION:
      if (const auto *ND = dyn_cast<NamedDecl>(ctx.decl)) {
        return serialize(ND);
      }
      return "decl<" + std::to_string((uintptr_t)ctx.decl) + ">";
    case TypeGraphContext::CALLINST:
      if (ctx.IsResolvePoint)
        return serialize(ctx.call, ctx.ResolvePointArgnum);
      return serialize(ctx.call);
    case TypeGraphContext::NUMERIC:
      return "Number " + std::to_string(ctx.number) + "@" + module_unique_value;
    }
    llvm_unreachable("TypeGraphContextKind");
  }

  std::string serialize(const FunctionUseRef &use) {
    // must match "declaration" serialization above, including mangling,
    // namespaces and "internal"
    auto name = serialize(use.functionDecl);
    if (name == "unknown" && use.functionRef && use.functionRef->hasName())
      return "symbol " + use.functionRef->getName().str();
    return name;
  }

  std::string serialize(const Edge &e) {
    std::string s = std::to_string(e.type);
    switch (e.type) {
    case typegraph::EdgeType::CAST_SIMPLE:
    case typegraph::EdgeType::POINTS_TO:
    case typegraph::EdgeType::REACHABILITY:
      return s;
    case typegraph::EdgeType::STRUCT_MEMBER:
      return s + "\t" + std::to_string(e.struct_offset);
    case typegraph::EdgeType::UNION_MEMBER:
      return s + "\t" + *e.union_type;
    }
    llvm_unreachable("EdgeKind");
  }

  std::string serialize(const QualType &type) { return type.getAsString(); }

  std::string serialize(const TypeGraphEntry &e) {
    return serialize(e.type) + "\t" + serialize(e.context);
  }
};

thread_local TypeGraphBuilder *TypeGraphBuilder::CurrentInstance = nullptr;

void TypeGraphBuilder::addGlobalDeclaration(GlobalDecl &GD) {
  if (const auto *FD = llvm::dyn_cast<FunctionDecl>(GD.getDecl())) {
    graph->interfaces.insert(GD.getCanonicalDecl());
  } else if (const auto *VD = llvm::dyn_cast<VarDecl>(GD.getDecl())) {
    if (VD->isExternallyVisible()) {
      graph->interfaces.insert(GD.getCanonicalDecl());
    }
  }
}

void TypeGraphBuilder::addTypeCast(GlobalDecl &GD, const CastExpr *CE,
                                   QualType DestTy) {
  if (!GD.getDecl())
    GD = CurrentContext;
  assert(GD.getDecl());

  if (ignoreTheseExpressions.erase(CE) > 0)
    return;

  switch (CE->getCastKind()) {
  case CK_NullToPointer:
  case CK_ToVoid:
    return;

  default:
    addTypeCast2(GD, CE->getSubExpr(), DestTy);
  }
}

void TypeGraphBuilder::addTypeCast2(GlobalDecl &GD, const Expr *E, QualType DestTy) {
  if (!GD.getDecl())
    GD = CurrentContext;
  assert(GD.getDecl());
  DestTy = cleanType(DestTy);
  auto sourceNode = getSourceTypeOfExpression(GD, E);
  graph->addGraphEdge(sourceNode, TypeGraphEntry(DestTy, GD),
                      Edge(typegraph::EdgeType::CAST_SIMPLE));
}

void TypeGraphBuilder::addImplicitTypeCast(GlobalDecl &GD, const Expr *E,
                                           QualType DestTy) {
  if (!GD.getDecl())
    GD = CurrentContext;
  assert(GD.getDecl());
  DestTy = cleanType(DestTy);
  auto sourceNode = getSourceTypeOfExpression(GD, E);
  graph->addGraphEdge(sourceNode, TypeGraphEntry(DestTy, GD),
                      Edge(typegraph::EdgeType::CAST_SIMPLE));
}

void TypeGraphBuilder::addAddressOfFunction(GlobalDecl &GD,
                                            const UnaryOperator *E) {
  if (!GD.getDecl())
    GD = CurrentContext;
  assert(GD.getDecl());
  if (ignoreTheseExpressions.erase(E) > 0)
    return;
  auto DestTy = cleanType(E->getType());
  auto sourceNode = getSourceTypeOfExpression(GD, E->getSubExpr());
  graph->addGraphEdge(sourceNode, TypeGraphEntry(DestTy, GD),
                      Edge(typegraph::EdgeType::CAST_SIMPLE));
}

void TypeGraphBuilder::beforeCallArgsEmission(const CallExpr *E) {
  GlobalDecl GD;
  for (const auto *arg : E->arguments()) {
    // just evaluate the source type of all arguments.
    // that will block emission of collapsed casts later.
    getSourceTypeOfExpression(GD, arg);
  }
}

void TypeGraphBuilder::addCall(GlobalDecl &GD, const CallExpr *E,
                               CodeGen::CGCallee &callee,
                               llvm::CallBase *callinst,
                               const Decl *targetDecl) {
  if (!GD.getDecl())
    GD = CurrentContext;
  assert(GD.getDecl());
  auto isRemoved = ignoreTheseExpressions.erase(E) > 0;
  const auto *functionDecl = dyn_cast_or_null<FunctionDecl>(targetDecl);
  TypeGraphContext context = functionDecl ? TypeGraphContext(functionDecl)
                                          : TypeGraphContext(callinst);

  LibraryFunctionHandling Handling =
      (functionDecl && functionDecl->getIdentifier())
          ? GetHandlingForFunction(functionDecl->getName().data())
          : DEFAULT;
  if (Handling == IGNORE) {
    return;
  }
  if (Handling == MEMCPY && E->getNumArgs() == 3) {
    auto src = getSourceTypeOfExpression(GD, E->getArg(1), false);
    auto dst = getSourceTypeOfExpression(GD, E->getArg(0), false);
    graph->addGraphEdge(graph->getPointee(src), graph->getPointee(dst), typegraph::EdgeType::CAST_SIMPLE);
    return;
  }
  if (Handling == PER_CALL_CONTEXT) {
    context = TypeGraphContext(nextUniqueNumber++);
  }
  if (Handling == RESOLVE || Handling == RESOLVE_DEEP || Handling == RESOLVE_WITH_DATA) {
    auto num = nextCallNumber[callinst->getFunction()->getName().str()]++;
    uniqueCallNumber[callinst] = num;
  }
  if (Handling == DLSYM && E->getNumArgs() == 2) {
    auto returnType = cleanType(E->getType());
    auto num = nextCallNumber[callinst->getFunction()->getName().str()]++;
    uniqueCallNumber[callinst] = num;
    // create function use
    auto V = graph->getOrInsertEntry(TypeGraphEntry(returnType, GD));
    graph->vertices[V].dynamicFunctions.insert(callinst);
    return;
  }

  // Add casts parameter => argument
  // E->dump(llvm::errs());
  auto CT = E->getCallee()->getType();
  const FunctionProtoType *CalleeFunctionType = nullptr;
  if (CT->isFunctionPointerType()) {
    CalleeFunctionType = dyn_cast<FunctionProtoType>(CT->getPointeeType());
    if (CalleeFunctionType) {
      // llvm::errs() << "CalleeFunctionType = ";
      // CalleeFunctionType->dump(llvm::errs());
    }
  }
  unsigned int argnum = 0;
  int newEdges = 0;

  std::vector<int> Indices;
  if (Handling == RESOLVE_DEEP || Handling == RESOLVE_WITH_DATA) {
    Indices = GetHandlingIndicesForFunction(functionDecl->getName().data());
  }

  for (const auto *arg : E->arguments()) {
    ignoreTheseExpressions.erase(arg);
    auto src = getSourceTypeOfExpression(GD, arg, false);
    auto dstType = arg->getType();
    if (CalleeFunctionType && argnum < CalleeFunctionType->getNumParams()) {
      dstType = CalleeFunctionType->getParamType(argnum);
      if (dstType->isReferenceType() &&
          dstType.getNonReferenceType() == arg->getType()) {
        if (dstType->isRValueReferenceType()) {
          src.type = Context.getRValueReferenceType(src.type);
        } else if (dstType->isLValueReferenceType()) {
          src.type = Context.getLValueReferenceType(src.type);
        }
      }
    }

    if (Handling == RESOLVE && dstType->isFunctionPointerType()) {
      auto argVertex = graph->getOrInsertEntry(TypeGraphEntry(cleanType(dstType), context));
      graph->resolvePoints[callinst].emplace_back(argnum, argVertex);

    } else if (Handling == RESOLVE_WITH_DATA && dstType->isFunctionPointerType()) {
      auto argVertex = graph->getOrInsertEntry(TypeGraphEntry(cleanType(dstType), context));
      graph->resolvePoints[callinst].emplace_back(argnum, argVertex);
      for (size_t I = 0; I < Indices.size(); I += 2) {
        auto TypeInSig = E->getArg(Indices[I])->getType();
        if (CalleeFunctionType && Indices[I] < (int) CalleeFunctionType->getNumParams()) {
          TypeInSig = CalleeFunctionType->getParamType(Indices[I]);
        }
        auto TGInFunctionCall = TypeGraphEntry(cleanType(TypeInSig), context);
        auto CallTargetParamType = cast<FunctionProtoType>(dstType->getPointeeType().getCanonicalType())->getParamType(Indices[I+1]);
        auto TGInCallTarget = TypeGraphEntry(cleanType(CallTargetParamType), TypeGraphContext(callinst, true, argnum));
        graph->addGraphEdge(TGInFunctionCall, TGInCallTarget, Edge(typegraph::CAST_SIMPLE));
      }

    } else if (Handling == RESOLVE_DEEP) {
      // The passed function pointer is deeper in a struct. We
      // - find the correct typegraph node by traversing the C type
      // - find the correct LLVM address using a GEP instruction, and store it in additional metadata next to the call
      if (argnum == (unsigned) Indices.at(0)) {
        auto indexType = dstType;
        std::vector<llvm::Metadata *> GepIndices;
        GepIndices.push_back(llvm::MDString::get(callinst->getContext(), std::to_string(0)));
        if (indexType->isPointerType()) {
          indexType = dstType->getPointeeType();
        }
        for (size_t I = 1; I < Indices.size(); I++) {
          if (indexType->isStructureType()) {
            auto It = indexType->getAsRecordDecl()->field_begin();
            std::advance(It, Indices[I]);
            auto T = It->getType();
            T.addFastQualifiers(indexType.getCVRQualifiers());
            indexType = T;
          } else if (indexType->isUnionType()) {
            auto It = indexType->getAsUnionType()->getDecl()->field_begin();
            std::advance(It, Indices[I]);
            auto T = It->getType();
            T.addFastQualifiers(indexType.getCVRQualifiers());
            indexType = T;
            Indices[I] = 0;
          } else {
            assert(false);
          }
          GepIndices.push_back(llvm::MDString::get(callinst->getContext(), std::to_string(Indices[I])));
        }

        auto argVertex = graph->getOrInsertEntry(TypeGraphEntry(cleanType(indexType), context));
        graph->resolvePoints[callinst].emplace_back(argnum, argVertex);
        callinst->setMetadata("typegraph-resolve-deep", llvm::MDNode::get(callinst->getContext(), GepIndices));
      }
    }

    if (!functionDecl) {
      auto dstVertex = graph->getOrInsertEntry(TypeGraphEntry(cleanType(dstType), context));
      if (dstVertex != NO_VERTEX)
        graph->vertices[dstVertex].argumentReturnTypes.insert(
            graph->SymbolContainer->get("arg#" + std::to_string(argnum)));
    }
    if (graph->addGraphEdge(src, TypeGraphEntry(cleanType(dstType), context),
                        Edge(typegraph::EdgeType::CAST_SIMPLE)))
      newEdges++;
    argnum++;
  }

  // add casts for return value
  auto returnType = cleanType(E->getType());
  if (!returnType->isVoidType() && !isRemoved) {
    if (!functionDecl) {
      auto returnVertex = graph->getOrInsertEntry(TypeGraphEntry(returnType, context));
      if (returnVertex != NO_VERTEX)
        graph->vertices[returnVertex].argumentReturnTypes.insert(graph->SymbolContainer->get("return#value"));
    }
    if (graph->addGraphEdge(TypeGraphEntry(returnType, context), TypeGraphEntry(returnType, GD),
                            Edge(typegraph::EdgeType::CAST_SIMPLE)))
      newEdges++;
  }

  // add to call graph
  if (functionDecl && Handling == DEFAULT && newEdges > 0) {
    graph->callGraph[TypeGraphContext(GD)].insert(context);
  }

  // save type for call
  if (!functionDecl) {
    auto calleeType = TypeGraphEntry(cleanType(E->getCallee()->getType()), GD);
    graph->indirectCallToCalleeType[callinst] =
        graph->getOrInsertEntry(calleeType);
    auto num = nextCallNumber[callinst->getFunction()->getName().str()]++;
    uniqueCallNumber[callinst] = num;
  }

  // Catch implicit declarations
  const auto *D = dyn_cast_or_null<FunctionDecl>(E->getCalleeDecl());
  if (D && D->isImplicit() && !D->isHidden()) {
    graph->interfaces.insert(GlobalDecl(D).getCanonicalDecl());
    if (D->getNumParams() != E->getNumArgs()) {
      auto &vec = graph->usedInterfaces[GlobalDecl(D).getCanonicalDecl()];
      if (vec.empty()) {
        vec.push_back(graph->getOrInsertEntry(TypeGraphEntry(returnType, context)));
        for (const auto *arg : E->arguments()) {
          vec.push_back(graph->getOrInsertEntry(TypeGraphEntry(cleanType(arg->getType()), context)));
        }
      }
    }
  }
}

void TypeGraphBuilder::addCXXMemberCall(GlobalDecl &GD, const CallExpr *CE,
                                        const CXXMethodDecl *MD, bool isVirtual,
                                        const CXXMethodDecl *DevirtMD,
                                        const Expr *Base) {
  if (!GD.getDecl())
    GD = CurrentContext;
  assert(GD.getDecl());
  auto isRemoved = ignoreTheseExpressions.erase(CE) > 0;
  const auto *D = DevirtMD ? DevirtMD : MD;
  TypeGraphContext context = TypeGraphContext(D);

  auto CT = CE->getCallee()->getType();
  const FunctionProtoType *CalleeFunctionType = nullptr;
  if (CT->isFunctionPointerType()) {
    CalleeFunctionType = dyn_cast<FunctionProtoType>(CT->getPointeeType());
  }

  // "this" parameter
  if (Base->getType().getUnqualifiedType() !=
      D->getThisType().getUnqualifiedType()) {
    llvm::errs() << "[WARNING] Check these types: "
                 << Base->getType().getAsString() << "   vs   "
                 << D->getThisType().getAsString() << "\n";
  }
  // Add "this" parameter => method
  if (Base) {
    auto src = getSourceTypeOfExpression(GD, Base);
    graph->addGraphEdge(src,
                        TypeGraphEntry(cleanType(Base->getType()), context),
                        Edge(typegraph::EdgeType::CAST_SIMPLE));
  }

  // Add casts parameter => argument
  unsigned int argnum = 0;
  int newEdges = 0;
  for (const auto *arg : CE->arguments()) {
    auto src = getSourceTypeOfExpression(GD, arg, false);
    auto dstType = arg->getType();
    if (CalleeFunctionType && argnum < CalleeFunctionType->getNumParams()) {
      dstType = CalleeFunctionType->getParamType(argnum);
      if (dstType->isReferenceType() &&
          dstType.getNonReferenceType() == arg->getType()) {
        if (dstType->isRValueReferenceType()) {
          src.type = Context.getRValueReferenceType(src.type);
        } else if (dstType->isLValueReferenceType()) {
          src.type = Context.getLValueReferenceType(src.type);
        }
      }
    }
    if (graph->addGraphEdge(src, TypeGraphEntry(cleanType(dstType), context),
                        Edge(typegraph::EdgeType::CAST_SIMPLE)))
      newEdges++;
    argnum++;
  }

  // add casts for return value
  auto returnType = cleanType(CE->getType());
  if (!isRemoved && !returnType->isVoidType()) {
    if (graph->addGraphEdge(TypeGraphEntry(returnType, context),
                        TypeGraphEntry(returnType, GD),
                        Edge(typegraph::EdgeType::CAST_SIMPLE)))
      newEdges++;
  }

  if (newEdges > 0) {
    graph->callGraph[TypeGraphContext(GD)].insert(context);
  }

  // TODO handle member function pointers
}

void TypeGraphBuilder::addCXXConstructorCall(GlobalDecl &GD, const CallExpr *CE,
                                             const CXXConstructorDecl *MD,
                                             const Expr *Base) {
  if (!GD.getDecl())
    GD = CurrentContext;
  assert(GD.getDecl());
  // TODO for now, "this" could be handled differently
  addCXXMemberCall(GD, CE, MD, false, nullptr, Base);
}

void TypeGraphBuilder::addCXXConstructorCall(GlobalDecl &GD,
                                             const CXXConstructExpr *CE,
                                             const CXXConstructorDecl *MD) {
  if (!GD.getDecl())
    GD = CurrentContext;
  assert(GD.getDecl());
  auto isRemoved = ignoreTheseExpressions.erase(CE) > 0;
  TypeGraphContext context = TypeGraphContext(MD);

  // Add casts parameter => argument
  for (const auto *arg : CE->arguments()) {
    auto src = getSourceTypeOfExpression(GD, arg, false);
    graph->addGraphEdge(src, TypeGraphEntry(cleanType(arg->getType()), context),
                        Edge(typegraph::EdgeType::CAST_SIMPLE));
  }

  // add casts for return value
  auto returnType = cleanType(CE->getType());
  if (!isRemoved && !returnType->isVoidType()) {
    graph->addGraphEdge(TypeGraphEntry(returnType, context),
                        TypeGraphEntry(returnType, GD),
                        Edge(typegraph::EdgeType::CAST_SIMPLE));
  }
}

void TypeGraphBuilder::addBuiltinExpr(GlobalDecl &GD, unsigned BuiltinID, const CallExpr *E) {
  switch (BuiltinID) {
  case Builtin::BImemcpy:
  case Builtin::BI__builtin_memcpy:
  case Builtin::BImempcpy:
  case Builtin::BI__builtin_mempcpy:
  case Builtin::BI__builtin___memcpy_chk:
  case Builtin::BI__builtin_objc_memmove_collectable:
  case Builtin::BI__builtin___memmove_chk:
  case Builtin::BImemmove:
  case Builtin::BI__builtin_memmove: {
    auto src = getSourceTypeOfExpression(GD, E->getArg(1), false);
    auto dst = getSourceTypeOfExpression(GD, E->getArg(0), false);
    graph->addGraphEdge(graph->getPointee(src), graph->getPointee(dst), typegraph::EdgeType::CAST_SIMPLE);
    break;
  }
  }

  for (const auto *arg : E->arguments()) {
    // just evaluate the source type of all arguments.
    // that will block emission of collapsed casts later.
    getSourceTypeOfExpression(GD, arg);
  }
}

void TypeGraphBuilder::addGlobalVarUse(GlobalDecl &GD, const VarDecl *D,
                                       const Expr *E, llvm::Value *V, QualType Ty) {
  if (!GD.getDecl())
    GD = CurrentContext;
  assert(GD.getDecl());
  if (ignoreTheseExpressions.erase(E) > 0)
    return;
  // TODO could be better / more precise?
  /*if (D->getDeclName().isIdentifier())
    llvm::errs() << "[BUILDER] Use of global var " << D->getName() << "\n";
  else
    llvm::errs() << "[BUILDER] Use of global var that is not identifier\n";*/
  Ty = cleanType(Ty);
  // llvm::errs() << "          resulting in: " << TypeGraphEntry(Ty, D) << " =>
  // " << TypeGraphEntry(Ty, GD) << "\n"; both sides - for now
  graph->addGraphEdge(TypeGraphEntry(Ty, D), TypeGraphEntry(Ty, GD),
                      Edge(typegraph::EdgeType::CAST_SIMPLE));
  graph->addGraphEdge(TypeGraphEntry(Ty, GD), TypeGraphEntry(Ty, D),
                      Edge(typegraph::EdgeType::CAST_SIMPLE));
}

void TypeGraphBuilder::addFunctionRefUse(GlobalDecl &GD, const FunctionDecl *D,
                                         const Expr *E, llvm::Value *V, QualType Ty) {
  if (ignoreFunctionRefUses)
    return;
  if (!GD.getDecl())
    GD = CurrentContext;
  assert(GD.getDecl());
  Ty = cleanType(Ty);
  V = pullFunctionFromValue(V);
  // llvm::errs() << "[BUILDER] Use of function " << D->getName() << "\n";
  if (V && !isa<llvm::Function>(V))
    llvm::errs() << "[WARNING] addFunctionRefUse: V is not a function! " << *V
                 << "\n";
  Vertex v = graph->getOrInsertEntry(TypeGraphEntry(Ty, GD));
  assert(v != NO_VERTEX);
  graph->vertices[v].functions.emplace(D, V);
  while (D->getPreviousDecl())
    D = D->getPreviousDecl();
  graph->usedFunctions.insert(D);
}

TypeGraphEntry
TypeGraphBuilder::getSourceTypeOfExpression(const GlobalDecl &GD, const Expr *E,
                                            bool ignoreReevaluation) {
  // no nullptr => void* => ... casts
  if (E->getType()->isVoidPointerType()) {
    if (const auto *pe = dyn_cast<ParenExpr>(E)) {
      if (const auto *ce = dyn_cast<CStyleCastExpr>(pe->getSubExpr())) {
        if (isa<IntegerLiteral>(ce->getSubExpr())) {
          return TypeGraphEntry(QualType(), GD);
        }
      }
    }
  }
  if (isa<CXXNullPtrLiteralExpr>(E)) {
    return TypeGraphEntry(QualType(), GD);
  }

  // handle calls
  if (const auto *Call = dyn_cast_or_null<CallExpr>(E)) {
    const auto *functionDecl =
        dyn_cast_or_null<FunctionDecl>(Call->getCalleeDecl());
    if (functionDecl && functionDecl->getIdentifier()) {
      auto Handling = GetHandlingForFunction(functionDecl->getName().data());
      if (Handling == IGNORE)
        return TypeGraphEntry(QualType(), GD);
      if (Handling != DLSYM) {
        if (ignoreReevaluation)
          ignoreTheseExpressions.insert(Call);
        auto Ctx = Handling == PER_CALL_CONTEXT ? TypeGraphContext(nextUniqueNumber) : TypeGraphContext(functionDecl);
        return TypeGraphEntry(cleanType(E->getType()), Ctx);
      }
    }
  }

  // handle casts
  if (const auto *CE = dyn_cast_or_null<CastExpr>(E)) {
    switch (CE->getCastKind()) {
    case CK_BaseToDerived:
    case CK_DerivedToBase:
    case CK_UncheckedDerivedToBase:
    case CK_BaseToDerivedMemberPointer:
    case CK_DerivedToBaseMemberPointer:
    case CK_Dynamic:
      break;
    case CK_LValueToRValue:
      if (ignoreReevaluation && isa<DeclRefExpr>(CE->getSubExpr()))
        ignoreTheseExpressions.insert(CE);
      // case CK_NoOp:
      return getSourceTypeOfExpression(GD, CE->getSubExpr(),
                                       ignoreReevaluation);
    default:
      // do not handle CE in a subsequent call
      if (ignoreReevaluation)
        ignoreTheseExpressions.insert(CE);
      /*if (ignoreReevaluation && CE->getCastKind() == CK_LValueToRValue) {
        if (const auto *D =
      dyn_cast_or_null<NamedDecl>(CE->getSubExpr()->getReferencedDeclOfCallee()))
      { if (D->getName() == "s") {
          }
        }
      }*/
      return getSourceTypeOfExpression(GD, CE->getSubExpr(),
                                       ignoreReevaluation);
    }
  }

  // Handle unary operators
  if (const auto *Op = dyn_cast_or_null<UnaryOperator>(E)) {
    if (Op->getOpcode() == UO_AddrOf &&
        Op->getSubExpr()->getType()->isFunctionType()) {
      if (ignoreReevaluation)
        ignoreTheseExpressions.insert(Op);
      return getSourceTypeOfExpression(GD, Op->getSubExpr(),
                                       ignoreReevaluation);
    }
    if (Op->getOpcode() == UO_Deref && Op->getType()->isFunctionType()) {
      if (ignoreReevaluation)
        ignoreTheseExpressions.insert(Op);
      return getSourceTypeOfExpression(GD, Op->getSubExpr(),
                                       ignoreReevaluation);
    }
  }

  // Handle paren expression
  if (const auto *PE = dyn_cast_or_null<ParenExpr>(E)) {
    if (ignoreReevaluation)
      ignoreTheseExpressions.insert(PE);
    return getSourceTypeOfExpression(GD, PE->getSubExpr(), ignoreReevaluation);
  }

  auto Type = E->getType();

  if (auto *DE = dyn_cast_or_null<DeclRefExpr>(E)) {
    if (DE->getDecl()) {
      auto DeclType = DE->getDecl()->getType();
      // Sometimes declaration has different type than usage. Example:
      // incomplete functions and reference parameters. We don't want to see the
      // reference magic, but we have to see the real function type.
      if (Type != DeclType && Type->isFunctionType() &&
          DeclType->isFunctionType()) {
        llvm::errs() << "[WARNING] Decl has different type than DeclRef! ("
                     << DE->getDecl()->getType().getAsString() << "   vs   "
                     << DE->getType().getAsString() << ") in ";
        Type = DeclType;
      }
      // Global variable
      if (const auto *VD = dyn_cast<VarDecl>(DE->getDecl())) {
        if (VD->hasLinkage() || VD->isStaticDataMember()) {
          if (ignoreReevaluation)
            ignoreTheseExpressions.insert(E);
          return TypeGraphEntry(cleanType(Type), GlobalDecl(VD).getCanonicalDecl());
        }
      }
    }
  }

  if (!GD.getDecl())
    return TypeGraphEntry(cleanType(Type), GD);
  return TypeGraphEntry(cleanType(Type), GD.getCanonicalDecl());
}

void TypeGraphBuilder::beforeGlobalReplacements(
    llvm::SmallVector<std::pair<llvm::GlobalValue *, llvm::Constant *>, 8>
        &Replacements) {
  for (auto &I : Replacements) {
    llvm::GlobalValue *GV = I.first;
    llvm::Constant *C = I.second;
    for (auto &vd: graph->vertices) {
      auto it = vd.functions.begin();
      while (it != vd.functions.end()) {
        if (it->functionRef == GV) {
          vd.functions.insert(FunctionUseRef(it->functionDecl, pullFunctionFromValue(C)));
          it = vd.functions.erase(it);
        } else {
          it++;
        }
      }
    }
  }
}

TypeGraphBuilder::TypeGraphBuilder(ASTContext &C, CodeGenModule &CGM)
    : graph(new TypeGraphRepr()), Context(C), CGM(CGM) {
  graph->C = &C;
  TestHandlingForFunction();
  TypegraphFunctionsSetArch(C.getTargetInfo().getTriple().getArch());
}

TypeGraphBuilder::~TypeGraphBuilder() {}

namespace {
void collectNodesForGlobalDeclInterface(TypeGraphRepr *graph,
                                        const GlobalDecl &GD,
                                        std::vector<typegraph::Vertex> &vec,
                                        bool isDefinition) {
  if (const auto *FD = llvm::dyn_cast<FunctionDecl>(GD.getDecl())) {
    // if (FD->isExternallyVisible()) {
    Vertex rt = graph->getOrInsertEntry(TypeGraphEntry(cleanType(FD->getReturnType()), GD));
    vec.push_back(rt);
    if (isDefinition && rt != NO_VERTEX) {
      graph->vertices[rt].argumentReturnTypes.insert(graph->SymbolContainer->get("return#value"));
    }
    for (unsigned i = 0; i < FD->getNumParams(); i++) {
      Vertex a = graph->getOrInsertEntry(TypeGraphEntry(cleanType(FD->getParamDecl(i)->getType()), GD));
      vec.push_back(a);
      if (isDefinition && a != NO_VERTEX) {
        graph->vertices[a].argumentReturnTypes.insert(graph->SymbolContainer->get("arg#" + std::to_string(i)));
      }
    }
    //}
  } else if (const auto *VD = llvm::dyn_cast<VarDecl>(GD.getDecl())) {
    if (VD->isExternallyVisible()) {
      vec.push_back(graph->getOrInsertEntry(
          TypeGraphEntry(cleanType(VD->getType()), GD)));
    }
  }
}
} // namespace

void TypeGraphBuilder::Release(llvm::Module &M, const std::string &OutputFile) {
  if (!ignoreTheseExpressions.empty()) {
    // llvm::errs() << "[WARNING] ignoreTheseExpressions not empty! Size = " << ignoreTheseExpressions.size() << "\n";
    /*
    for (const auto *E: ignoreTheseExpressions) {
      llvm::errs() << "EXPR ";
      E->dump(llvm::errs());
    }
    assert(ignoreTheseExpressions.empty() && "ignoreTheseExpressions");
     */
  }

  /*
  std::map<const QualType, bool> DoNotMinimizeType;
  std::function<bool(const QualType)> CheckDoNotMinimizeType = [&CheckDoNotMinimizeType, &DoNotMinimizeType](const QualType Ty) {
    auto &It = DoNotMinimizeType[Ty];
    if (Ty->isVoidPointerType()) {
      It = true;
    } else if (Ty->isStructureOrClassType()) {
      if (Ty->isIncompleteType()) {
        It = true;
      } else {
        for (auto *Field: Ty->getAsRecordDecl()->fields()) {
          if (CheckDoNotMinimizeType(Field->getType())) {
            It = true;
            break;
          }
        }
      }
    } else if (Ty->isPointerType()) {
      It = CheckDoNotMinimizeType(Ty->getPointeeType());
    } else if (Ty->isReferenceType()) {
      It = CheckDoNotMinimizeType(Ty.getNonReferenceType());
    } else if (Ty->isFunctionProtoType()) {
      const auto *FT = Ty->getAs<FunctionProtoType>();
      for (auto Param: FT->param_types()) {
        It = CheckDoNotMinimizeType(Param);
        if (It)
          return It;
      }
      It = CheckDoNotMinimizeType(FT->getReturnType());
    }
    return It;
  };
   */

  graph->expandAll();

  Serializer serializer(M, CGM, uniqueCallNumber);
  // Port graph builder representation to regular TypeGraph instance
  typegraph::TypeGraph TG(graph->SymbolContainer);
  const std::string *Unit = TG.SymbolContainer->get(M.getName());
  // copy interface descriptions
  // this happens first, because this step generates new nodes on the fly.
  for (auto &GD : graph->interfaces) {
    const auto *symbol = TG.SymbolContainer->get(CGM.getMangledName(GD));
    llvm::GlobalValue *GV = M.getFunction(*symbol);
    if (!GV) {
      GV = M.getGlobalVariable(*symbol);
    }
    auto IsVisible = GV && !GV->hasLocalLinkage();
    if (const auto *FD = dyn_cast<FunctionDecl>(GD.getDecl())) {
      IsVisible &= FD->isExternallyVisible();
    }
    const auto *D = GD.getDecl();
    while (D->getPreviousDecl())
      D = D->getPreviousDecl();
    if (GV && (IsVisible ||
               graph->usedFunctions.find(D) != graph->usedFunctions.end())) {
      typegraph::InterfaceDesc Interface;
      Interface.SymbolName = symbol;
      Interface.Symbol = GV;
      Interface.ContextName = TG.SymbolContainer->get(serializer.serialize(GD));
      Interface.IsExternal = true;
      Interface.IsDefined = GV != nullptr && !GV->isDeclaration();
      if (const auto *FD = dyn_cast_or_null<FunctionDecl>(GD.getDecl())) {
        Interface.IsFunction = true;
        Interface.IsVarArg = FD->isVariadic();
        /*
        Interface.DoNotMinimize = Interface.IsVarArg || CheckDoNotMinimizeType(FD->getType());
        if (!Interface.IsVarArg && Interface.DoNotMinimize) {
          llvm::errs() << "Do not minimize: " << *Interface.SymbolName << "\n";
        }*/
        Interface.DoNotMinimize = Interface.IsVarArg;
      }
      // might update symbolic types in graph (arg#1 etc)
      collectNodesForGlobalDeclInterface(graph.get(), GD, Interface.Types, Interface.IsDefined);
      TG.Interfaces[symbol].push_back(std::move(Interface));
    }
  }
  for (auto &it : graph->usedInterfaces) {
    const auto *symbol = TG.SymbolContainer->get(CGM.getMangledName(it.first));
    llvm::GlobalValue *GV = M.getFunction(*symbol);
    const auto *D = it.first.getDecl();
    while (D->getPreviousDecl())
      D = D->getPreviousDecl();
    if (GV && (!GV->hasLocalLinkage() ||
               graph->usedFunctions.find(D) != graph->usedFunctions.end())) {
      typegraph::InterfaceDesc Interface;
      Interface.SymbolName = symbol;
      Interface.Symbol = GV;
      Interface.ContextName =
          TG.SymbolContainer->get(serializer.serialize(it.first));
      Interface.IsExternal = true;
      Interface.IsDefined = GV != nullptr && !GV->isDeclaration();
      if (const auto *FD = dyn_cast_or_null<FunctionDecl>(it.first.getDecl())) {
        Interface.IsFunction = true;
        Interface.IsVarArg = FD->isVariadic();
      }
      Interface.Types = std::move(it.second);
      TG.Interfaces[symbol].push_back(std::move(Interface));
    }
  }

  // Copy regular graph
  for (size_t v = 0; v < graph->vertices.size(); v++) {
    // copy vertices
    auto CtxRef = graph->vertices[v].entry.context;
    const auto *Ty = TG.SymbolContainer->get(
        serializer.serialize(graph->vertices[v].entry.type));
    const auto *Ctx = TG.SymbolContainer->get(
        serializer.serialize(CtxRef));
    bool ContextIsGlobal = false;
    if (CtxRef.kind == TypeGraphContext::DECLARATION) {
      if (dyn_cast_or_null<VarDecl>(CtxRef.decl) != nullptr) {
        ContextIsGlobal = true;
      }
    }
    auto v2 = TG.createVertex(Ty, Ctx, ContextIsGlobal, 0);
    assert(v2 == (typegraph::Vertex)v);
    // add additional contexts
    for (const auto *s: graph->vertices[v].argumentReturnTypes) {
      TG[v2].AdditionalNames.push_back(typegraph::TypeContextPair(s, Ctx, 0));
    }
    // copy function use
    for (auto &use : graph->vertices[v].functions) {
      auto symbol = use.functionRef ? use.functionRef->getName()
                                    : use.functionDecl->getName();
      typegraph::FunctionUsage usage(
          TG.SymbolContainer->get(serializer.serialize(use)),
          TG.SymbolContainer->get(symbol));
      if (auto *F = dyn_cast_or_null<llvm::Function>(use.functionRef)) {
        usage.Function = F;
        usage.NumArgs = F->arg_size();
        usage.IsVarArg = F->isVarArg();
      }
      TG[v].FunctionUses.insert(usage);
    }
    for (auto &call : graph->vertices[v].dynamicFunctions) {
      const auto *name = TG.SymbolContainer->get(serializer.serializeDynamicFunction(call));
      typegraph::FunctionUsage usage(name, name, nullptr);
      usage.IsDynamicFunction = true;
      TG[v].FunctionUses.insert(usage);
      call->setMetadata("typegraph_dlsym", llvm::MDNode::get(M.getContext(), {llvm::MDString::get(M.getContext(), *name)}));
    }
    // context definitions provided by this module
    if (CtxRef.kind != TypeGraphContext::TypeGraphContextKind::DECLARATION) {
      TG.ContextDefiningUnits[Ctx] = Unit;
    } else if (const auto *D = dyn_cast<NamedDecl>(CtxRef.decl)) {
      if (D->getIdentifier()) {
        auto *LLVMDecl = M.getNamedValue(D->getName());
        if (LLVMDecl != nullptr && !LLVMDecl->isDeclaration()) {
          // Ignore "inline" functions
          bool IsDefining = true;
          if (auto *F = dyn_cast<llvm::Function>(LLVMDecl)) {
            if (F->hasFnAttribute(llvm::Attribute::InlineHint) ||
                F->hasFnAttribute(llvm::Attribute::AlwaysInline)) {
              IsDefining = false;
            }
          }
          if (IsDefining) {
            TG.ContextDefiningUnits[Ctx] = Unit;
          }
        }
      }
    }
  }
  // copy edges
  for (size_t v = 0; v < graph->vertices.size(); v++) {
    for (auto &e : graph->vertices[v].edges) {
      typegraph::TGEdge tge(e.type);
      if (e.type == typegraph::STRUCT_MEMBER) {
        tge.StructOffset = e.struct_offset;
      } else if (e.type == typegraph::UNION_MEMBER) {
        tge.UnionType = e.union_type;
      }
      TG.add_edge(v, e.target, tge);
    }
  }
  // copy indirect calls
  for (auto &it : graph->indirectCallToCalleeType) {
    const auto *callname =
        TG.SymbolContainer->get(serializer.serialize(it.first));
    TG.addIndirectCall(it.second, callname, it.first->arg_size());
  }

  // copy resolve points
  for (auto &it : graph->resolvePoints) {
    for (auto &rp: it.second) {
      const auto *callname =
          TG.SymbolContainer->get(serializer.serialize(it.first, rp.first));
      TG.addIndirectCall(rp.second, callname, rp.first, true);
    }
  }

  // Create Callgraph
  for (auto It: graph->callGraph) {
    const auto *Ctx = TG.SymbolContainer->get(serializer.serialize(It.first));
    auto V = TG.CallGraph->getVertex(Ctx);
    for (auto Ctx2: It.second) {
      const auto *Ctx2Str = TG.SymbolContainer->get(serializer.serialize(Ctx2));
      auto V2 = TG.CallGraph->getVertex(Ctx2Str);
      TG.CallGraph->add_edge(V, V2, {});
    }
  }

  TG.CallGraph = TG.CallGraph->buildStronglyConnectedComponents();
  if (typegraph::Settings.clang_layering) {
    typegraph::applySimpleLayering(TG);
  }

  std::unique_ptr<typegraph::TypeGraph> MinimizedGraph = 0;
  std::unique_ptr<typegraph::TypeGraph> BackupGraph = 0;
  typegraph::TypeGraph *FinalGraph = &TG;
  if (typegraph::Settings.clang_minimize_graph && typegraph::Settings.enabled) {
    TimeClock Clock;
    // auto nv = TG.num_vertices();
    // auto ne = TG.num_edges();
    markExternalInterfaceNodes(TG, true, true, false);
    if (typegraph::Settings.clang_intermediate_graph_output && !OutputFile.empty() && OutputFile != "-") {
      auto fname = startsWith(OutputFile, "-o ")
          ? OutputFile.substr(3) + ".before.typegraph"
          : OutputFile + ".before.typegraph";
      TG.saveToFile(fname);
    }

    // auto MinimizedGraph2 = TG.computeEquivalenceClasses(true, 2);
    auto *MinimizedGraph2 = &TG;
    MinimizedGraph2->computeInterfaceRelations();
    MinimizedGraph2->combineEquivalencesInline(true, typegraph::Settings.linktime_layering, false);
    // MinimizedGraph2->AssertIntegrity();
    /* Clock.report("Equivalence: " + std::to_string(nv) + "=>" +
                 std::to_string(MinimizedGraph2->num_vertices()) +
                 " vertices, " + std::to_string(ne) + "=>" +
                 std::to_string(MinimizedGraph2->num_edges()) + " edges"); // */
    if (typegraph::Settings.clang_intermediate_graph_output && !OutputFile.empty() && OutputFile != "-") {
      auto fname = startsWith(OutputFile, "-o ")
          ? OutputFile.substr(3) + ".eq.typegraph"
          : OutputFile + ".before.typegraph";
      MinimizedGraph2->saveToFile(fname);
    }
    // FinalGraph = MinimizedGraph2;
    auto ExternalQuote = MinimizedGraph2->getNumExternalVertices() * 1.0 / MinimizedGraph2->num_vertices();
    // fprintf(stderr, "ExternalQuote = %f\n", ExternalQuote);

    if (true || ExternalQuote < 0.85) {  // Limited minimizer disabled here
      BackupGraph = 0; // Limited minimizer disabled here std::make_unique<typegraph::TypeGraph>(*MinimizedGraph2);
      MinimizedGraph2->computeReachability(false, true, 2);
      // MinimizedGraph2->computeReachabilityInline(false, true, typegraph::Settings.linktime_layering);
      MinimizedGraph = MinimizedGraph2->getMinifiedGraph();
      /*Clock.report("Equivalence+Minimizer: " + std::to_string(nv) + "=>" +
                   std::to_string(MinimizedGraph->num_vertices()) +
                   " vertices, " + std::to_string(ne) + "=>" +
                   std::to_string(MinimizedGraph->num_edges()) + " edges"); // */
      if (false && MinimizedGraph->num_edges() > 20 * BackupGraph->num_edges()) {  // Limited minimizer disabled here
        Clock.report("Too much new edges, withdrawing minimizer");
        FinalGraph = BackupGraph.get();
      } else {
        FinalGraph = MinimizedGraph.get(); // */
        BackupGraph = 0;
      }
    } else {
      Clock.report("ExternalQuote > 85%, skipping minimizer");
      FinalGraph = MinimizedGraph2;
    }

    /*
    Clock.restart();
    markExternalInterfaceNodes(TG, true, true, false);
    TG.computeReachability(false, true, 2);
    MinimizedGraph = TG.getMinifiedGraph();
    FinalGraph = MinimizedGraph.get();
    Clock.report("Minimizer: " + std::to_string(TG.num_vertices()) + "=>" +
                 std::to_string(MinimizedGraph->num_vertices()) +
                 " vertices, " + std::to_string(e) + "=>" +
                 std::to_string(MinimizedGraph->num_edges()) + " edges");
    // */
    // llvm::errs() << "[DEBUG] Minimizer: " << TG.num_vertices() << "=>" <<
    // MinimizedGraph->num_vertices() << " vertices, " << e << "=>" <<
    // MinimizedGraph->num_edges() << " edges\n";
  }

  // Export to module
  if (typegraph::Settings.enabled) {
    typegraph::WriteTypegraphToMetadata(*FinalGraph, M,
                                        serializer.module_unique_value);
  }

  // Annotate created indirect calls with additional metadata
  auto &C = M.getContext();
  for (auto &it : graph->indirectCallToCalleeType) {
    // we store the call's node next to the call instruction, but keep the
    // serialized name for completeness
    auto *callname = llvm::MDString::get(C, serializer.serialize(it.first));
    it.first->setMetadata(llvm::LLVMContext::MD_typegraph_node,
                          llvm::MDNode::get(C, {callname}));
  }

  // Annotate functions that need resolve points
  for (auto &it : graph->resolvePoints) {
    std::vector<llvm::Metadata *> Names;
    Names.push_back(llvm::MDString::get(C, "<resolvepoint>"));
    for (auto &rp : it.second) {
      auto RPName = serializer.serialize(it.first, rp.first);
      Names.push_back(llvm::MDString::get(C, RPName));
    }
    it.first->setMetadata(llvm::LLVMContext::MD_typegraph_node,
                          llvm::MDNode::get(C, Names));
  }

  // Write typegraph file if desired
  if (!OutputFile.empty() && OutputFile != "-") {
    // write to additional file
    auto fname = startsWith(OutputFile, "-o ")
                     ? OutputFile.substr(3) + ".typegraph"
                     : OutputFile + ".typegraph";
    FinalGraph->saveToFile(fname);

    if (typegraph::Settings.clang_show_graph) {
      auto exe = llvm::sys::fs::getMainExecutable(program_invocation_name, nullptr);
      exe = exe.substr(0, exe.rfind('/'));
      std::string cmd = "python3 " + exe + "/../../../tests/show_typegraph.py --numbers";
      if (typegraph::Settings.clang_show_graph_all) cmd += " --all";
      cmd += " '" + fname + "'";
      (void)system(cmd.c_str());
    }
  }
}

namespace {
class ConstExprParser : public StmtVisitor<ConstExprParser, void, QualType> {
  TypeGraphBuilder &TGB;
  CodeGenModule &CGM;
  GlobalDecl GD;

public:
  ConstExprParser(TypeGraphBuilder &tgb, CodeGenModule &CGM, GlobalDecl GD)
      : TGB(tgb), CGM(CGM), GD(GD) {}

  /*
  void Visit(Stmt *S) {
    llvm::errs() << "[VISIT] ";
    S->dump(llvm::errs());
    StmtVisitor<ConstExprParser, void>::Visit(S);
  }
  // */

  void VisitArraySubscriptExpr(ArraySubscriptExpr *E, QualType T) {
    // Auto-generated version seems to be broken.
    Visit(E->getBase(), E->getBase()->getType());
    Visit(E->getIdx(), E->getIdx()->getType());
  }

  void VisitCastExpr(CastExpr *CE, QualType T) {
    TGB.addTypeCast(GD, CE, T);
    Visit(CE->getSubExpr(), CE->getSubExpr()->getType());
  }

  void VisitUnaryAddrOf(UnaryOperator *E, QualType T) {
    if (T->isFunctionPointerType() &&
        E->getSubExpr()->getType()->isFunctionType()) {
      TGB.addAddressOfFunction(GD, E);
      if (T != E->getType()) {
        TGB.addTypeCast2(GD, E, T);
      }
    }
    Visit(E->getSubExpr(), E->getSubExpr()->getType());
  }

  void VisitDeclRefExpr(DeclRefExpr *E, QualType T) {
    if (auto *FD = dyn_cast<FunctionDecl>(E->getDecl())) {
      auto *V = CGM.GetAddrOfFunction(FD);
      TGB.addFunctionRefUse(GD, FD, E, V, FD->getType());
    } else if (auto *GV = dyn_cast<VarDecl>(E->getDecl())) {
      if (!GV->isLocalVarDeclOrParm()) {
        TGB.addGlobalVarUse(GD, GV, E, nullptr, T);
      }
    }
  }

  void VisitInitListExpr(InitListExpr *ILE, QualType T) {
    for (unsigned i = 0; i < ILE->getNumInits(); i++) {
      // Check if we have a "const struct" with non-qualified initializer fields
      // happens C++ only so far
      auto Ty = ILE->getInit(i)->getType();
      Ty.addFastQualifiers(T.getCVRQualifiers());
      /*if (Ty != ILE->getInit(i)->getType()) {
        TGB.addTypeCast2(GD, ILE->getInit(i), Ty);
      }*/
      Visit(ILE->getInit(i), Ty);
    }
  }

  void VisitParenExpr(ParenExpr *E, QualType T) {
    Visit(E->getSubExpr(), T);
  }
};
} // namespace

void TypeGraphBuilder::addLocalVarDef(GlobalDecl &GD, const VarDecl *D) {
  graph->generateEssentialSubtypes(cleanType(D->getType()), GD);
  if (D->getInit()) {
    ConstExprParser(*this, CGM, GD).Visit(const_cast<Expr *>(D->getInit()), cleanType(D->getType()));
    /*if (D->getType() != D->getInit()->getType()) {
      graph->addGraphEdge(TypeGraphEntry(cleanType(D->getInit()->getType()), GD),
                          TypeGraphEntry(cleanType(D->getType()), GD),
                          Edge(typegraph::EdgeType::CAST_SIMPLE));
    }*/
  }
}

void TypeGraphBuilder::addGlobalVarInitializer(const VarDecl &D,
                                               const Expr *init) {
  if (!init)
    return;
  ConstExprParser(*this, CGM, GlobalDecl(&D)).Visit(const_cast<Expr *>(init), cleanType(D.getType()));
  /*if (D.getType() != init->getType()) {
    graph->addGraphEdge(TypeGraphEntry(cleanType(init->getType()), GlobalDecl(&D)),
                        TypeGraphEntry(cleanType(D.getType()), GlobalDecl(&D)),
                        Edge(typegraph::EdgeType::CAST_SIMPLE));
  }*/
}

} // namespace clang
