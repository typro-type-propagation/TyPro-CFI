#ifndef LLVM_TYPEGRAPHS_TYPEGRAPHBUILDER_H
#define LLVM_TYPEGRAPHS_TYPEGRAPHBUILDER_H

#include "clang/AST/Expr.h"
#include "clang/AST/GlobalDecl.h"
#include "llvm/Support/JSON.h"
#include "llvm/IR/Value.h"
#include <map>

namespace llvm {
class CallBase;
class Module;
} // namespace llvm

namespace clang {

class ASTContext;

namespace CodeGen {
class CGCallee;
class CodeGenModule;
} // namespace CodeGen

class TypeGraphRepr;

struct TypeGraphContext {
  enum TypeGraphContextKind { DECLARATION, CALLINST, NUMERIC };

  TypeGraphContextKind kind;
  union {
    const Decl *decl;
    const llvm::CallBase *call;
    long number;
  };
  bool IsResolvePoint = false;
  int ResolvePointArgnum = 0;

  TypeGraphContext(const Decl *decl) : kind(DECLARATION), decl(decl) {}
  TypeGraphContext(const GlobalDecl &decl)
      : kind(DECLARATION), decl(decl.getDecl()) {}
  TypeGraphContext(const llvm::CallBase *call, bool IsResolvePoint = false, int ResolvePointArgnum = 0)
      : kind(CALLINST), call(call), IsResolvePoint(IsResolvePoint), ResolvePointArgnum(ResolvePointArgnum) {}
  TypeGraphContext(long number) : kind(NUMERIC), number(number) {}

  bool operator==(const TypeGraphContext &other) const;
  bool operator<(const TypeGraphContext &other) const;
};

/**
 * What we consider as "node" in the typegraph - a pair (type, context)
 */
struct TypeGraphEntry {
  QualType type;
  TypeGraphContext context;

  inline TypeGraphEntry(const QualType &type, const TypeGraphContext &context)
      : type(type), context(context) {}

  inline bool operator==(const TypeGraphEntry &other) const {
    return type == other.type && context == other.context;
  }
  inline bool operator<(const TypeGraphEntry &other) const {
    if (type == other.type)
      return context < other.context;
    return type < other.type;
  }
};

raw_ostream &operator<<(raw_ostream &OS, const TypeGraphEntry &entry);
raw_ostream &operator<<(raw_ostream &OS, const TypeGraphContext &context);

/**
 * The class constructing the Type Graph
 */
class TypeGraphBuilder {
  std::unique_ptr<TypeGraphRepr> graph;
  llvm::json::Object ExportedTypeInfos;
  std::map<const llvm::CallBase *, int> uniqueCallNumber;
  std::map<const std::string, int> nextCallNumber;
  std::set<const Expr *> ignoreTheseExpressions;

  bool ignoreFunctionRefUses = false;

  ASTContext &Context;
  clang::CodeGen::CodeGenModule &CGM;

  long nextUniqueNumber = 0;

public:
  TypeGraphBuilder(ASTContext &C, clang::CodeGen::CodeGenModule &CGM);
  ~TypeGraphBuilder();

  void addGlobalDeclaration(GlobalDecl &GD);

  /**
   * Add edge for an (explicit or implicit) type cast expression
   * @param GD
   * @param CE the expression performing the cast
   * @param DestTy the type to cast to
   */
  void addTypeCast(GlobalDecl &GD, const CastExpr *CE, QualType DestTy);

  /**
   * Add edge for an (explicit or implicit type cast
   * @param GD
   * @param E the expression to be casted
   * @param DestTy the type to cast to
   */
  void addTypeCast2(GlobalDecl &GD, const Expr *E, QualType DestTy);

  /**
   * Any kind of cast that is *not* covered by a Cast node in the AST.
   * For example: const cast in initializer lists.
   * @param GD
   * @param E
   * @param DestTy
   */
  void addImplicitTypeCast(GlobalDecl &GD, const Expr *E, QualType DestTy);

  void addAddressOfFunction(GlobalDecl &GD, const UnaryOperator *E);

  void beforeCallArgsEmission(const CallExpr *E);
  void addCall(GlobalDecl &GD, const CallExpr *E,
               clang::CodeGen::CGCallee &callee, llvm::CallBase *callinst,
               const Decl *targetDecl);
  void addCXXMemberCall(GlobalDecl &GD, const CallExpr *CE,
                        const CXXMethodDecl *MD, bool isVirtual,
                        const CXXMethodDecl *DevirtMD, const Expr *Base);
  void addCXXConstructorCall(GlobalDecl &GD, const CallExpr *CE,
                             const CXXConstructorDecl *MD, const Expr *Base);
  void addCXXConstructorCall(GlobalDecl &GD, const CXXConstructExpr *CE,
                             const CXXConstructorDecl *MD);

  void addBuiltinExpr(GlobalDecl &GD, unsigned BuiltinID, const CallExpr *E);

  void addGlobalVarUse(GlobalDecl &GD, const VarDecl *D, const Expr *E,
                       llvm::Value *V, QualType Ty);

  void addLocalVarDef(GlobalDecl &GD, const VarDecl *D);

  void setIgnoreFunctionRefUse(bool ignore) { ignoreFunctionRefUses = ignore; }
  void addFunctionRefUse(GlobalDecl &GD, const FunctionDecl *D, const Expr *E,
                         llvm::Value *V, QualType Ty);

  void Release(llvm::Module &M, const std::string &OutputFile);

  void addGlobalVarInitializer(const VarDecl &D, const Expr *init);

  void beforeGlobalReplacements(
      llvm::SmallVector<std::pair<llvm::GlobalValue *, llvm::Constant *>, 8>
          &Replacements);

  // save current vardecl
  GlobalDecl CurrentContext;

  // some methods don't have CodeGenModule - use this as a replacement
  static thread_local TypeGraphBuilder *CurrentInstance;

protected:
  TypeGraphEntry getSourceTypeOfExpression(const GlobalDecl &GD, const Expr *E,
                                           bool ignoreReevaluation = true);
};

class TypeGraphBuilderCurrentContextScope {
  TypeGraphBuilder &TGB;
  GlobalDecl LastContext;

public:
  inline TypeGraphBuilderCurrentContextScope(TypeGraphBuilder &TGB,
                                             const GlobalDecl &GD)
      : TGB(TGB), LastContext(TGB.CurrentContext) {
    TGB.CurrentContext = GD;
    TypeGraphBuilder::CurrentInstance = &TGB;
  }
  inline ~TypeGraphBuilderCurrentContextScope() {
    TGB.CurrentContext = LastContext;
    TypeGraphBuilder::CurrentInstance = nullptr;
  }
};

} // namespace clang

#endif // LLVM_TYPEGRAPHS_TYPEGRAPHBUILDER_H
