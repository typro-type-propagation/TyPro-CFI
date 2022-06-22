#ifndef LLVM_TYPEGRAPHS_ENFORCING_RT_H
#define LLVM_TYPEGRAPHS_ENFORCING_RT_H

#include <cstdint>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

#define DEBUG_OUTPUT 0
#if DEBUG_OUTPUT == 1
#define dprintf(s, ...) fprintf(stderr, s, ##__VA_ARGS__)
#else
#define dprintf(s, ...) do {} while(0)
#endif

#define assert(cond)                                                                                                   \
  do {                                                                                                                 \
    if (!(cond)) {                                                                                                     \
      fprintf(stderr, "ASSERTION FAILED: %s (%s:%d)\n", #cond, __FILE__, __LINE__);                                    \
      abort();                                                                                                         \
    }                                                                                                                  \
  } while (0)

typedef unsigned long FunctionID;

namespace llvm {

/**
 * This class is in the llvm:: namespace, shadowing LLVM's Function, so that Typegraph can use LLVM functions in the
 * compiler, and this function info class in the runtime, without having to change its code. For the sake of clarity,
 * it's also accessible as typegraph::rt::FunctionInfos.
 */
struct Function {
  FunctionID ID;
  void *Address;

  inline Function(FunctionID Id, void *Address) : ID(Id), Address(Address) {}
};

inline bool operator==(const Function &Lhs, const Function &Rhs) { return Lhs.ID == Rhs.ID; }
inline bool operator==(const Function &Lhs, const FunctionID &Rhs) { return Lhs.ID == Rhs; }
inline bool operator<(const Function &Lhs, const Function &Rhs) { return Lhs.ID < Rhs.ID; }
inline bool operator<(const Function &Lhs, const FunctionID &Rhs) { return Lhs.ID < Rhs; }

} // namespace llvm

namespace typegraph {
namespace rt {

typedef llvm::Function FunctionInfos;

class DynamicSymbolInfo;

struct DispatcherInfos {
  const std::string *CallName;
  void **DispatcherAddr;
  long ModuleID; // the module that defined this dispatcher - functions from this module should already be handled (?)

  size_t CurrentImplHandlesTargets = 0xffffffff; // If CurrentImplHandlesTargets != Targets => Dispatcher must be re-generated
  std::vector<FunctionInfos> Targets;
  std::set<FunctionID> AlreadyHandledTargets; // Targets handled in the program's code before calling the dispatcher

  inline DispatcherInfos(const std::string *CallName, void **DispatcherAddr, long ModuleID)
      : CallName(CallName), DispatcherAddr(DispatcherAddr), ModuleID(ModuleID) {}

  /**
   * Add a new target if it does not exist, but does not invalidate the dispatcher
   * @param Infos
   */
  inline bool insertTarget(const FunctionInfos &Infos) {
    if (std::find(Targets.begin(), Targets.end(), Infos) == Targets.end()) {
      Targets.push_back(Infos);
      return true;
    }
    return false;
  }
};

// Pre-generated code
extern std::vector<void *> LazyDispatchers;
void assertLazyDispatchersExist(size_t Max);
void emptyTargetSetHandler(void);

} // namespace rt
} // namespace typegraph

#define EXTERN extern __attribute__((__visibility__("default")))

extern "C" {
[[maybe_unused]] EXTERN void __tg_register_graph(const char *GraphData, void **References, long ModuleID);
EXTERN void __tg_dynamic_error(size_t Index, long ID);
EXTERN long __tg_dlsym_to_id(typegraph::rt::DynamicSymbolInfo *DynamicSymbolCall, void *Symbol);
EXTERN void *__tg_resolve_symbol(const char *Name, long ID);
void *generateDispatcher(int Index, uintptr_t CurrentID);
}

#endif