#include "enforcing_rt.h"
#include "AssemblyBuilder.h"

namespace typegraph {
namespace rt {

#if defined(__x86_64__) || defined(_M_X64)
// This function is referenced when the dispatcher has not yet been built
template<int Index>
void tgLazyDispatcher() {
  // store all important registers
  __asm__("pushq %rdi ; pushq %rsi ; pushq %rdx ; pushq %rcx ; pushq %r8 ; pushq %r9 ; pushq %r10 ; pushq %r11");
  // Get current ID from r11
  uintptr_t CurrentID;
  __asm__("movq %%r11, %%rsi" : "=S"(CurrentID));
  void *Dispatcher = generateDispatcher(Index, CurrentID);
  // restore all important registers and call dispatcher
  __asm__("popq %%r11 ; popq %%r10 ; popq %%r9 ; popq %%r8 ; popq %%rcx ; popq %%rdx ; popq %%rsi ; popq %%rdi ; addq $8, %%rsp ; jmp *%%rax" : : "a"(Dispatcher));
}
#endif

std::vector<void *, ProtectedAllocator<void *>> *LazyDispatchers = nullptr;

static void __attribute__((constructor)) initLazyDispatchersAndProtectedHeap() {
  ProtectedAllocatorWriteableScope Scope;
  LazyDispatchers = new (ProtectedAllocator<std::vector<void *, ProtectedAllocator<void *>>>().allocate(1)) std::vector<void *, ProtectedAllocator<void *>>();
#if defined(__x86_64__) || defined(_M_X64)
  LazyDispatchers->push_back((void *)tgLazyDispatcher<0>);
  LazyDispatchers->push_back((void *)tgLazyDispatcher<1>);
  LazyDispatchers->push_back((void *)tgLazyDispatcher<2>);
  LazyDispatchers->push_back((void *)tgLazyDispatcher<3>);
  LazyDispatchers->push_back((void *)tgLazyDispatcher<4>);
  LazyDispatchers->push_back((void *)tgLazyDispatcher<5>);
  LazyDispatchers->push_back((void *)tgLazyDispatcher<6>);
  LazyDispatchers->push_back((void *)tgLazyDispatcher<7>);
  LazyDispatchers->push_back((void *)tgLazyDispatcher<8>);
  LazyDispatchers->push_back((void *)tgLazyDispatcher<9>);
  LazyDispatchers->push_back((void *)tgLazyDispatcher<10>);
  LazyDispatchers->push_back((void *)tgLazyDispatcher<11>);
  LazyDispatchers->push_back((void *)tgLazyDispatcher<12>);
  LazyDispatchers->push_back((void *)tgLazyDispatcher<13>);
  LazyDispatchers->push_back((void *)tgLazyDispatcher<14>);
  LazyDispatchers->push_back((void *)tgLazyDispatcher<15>);
#endif
}

static void __attribute__((destructor)) prepareProtectedHeapForFree() {
  protectedMakeWriteable();
}

void assertLazyDispatchersExist(size_t Max) {
  if (LazyDispatchers->size() < Max) {
    AssemblyBuilder Builder;
    char *Ptr = Builder.generateLazyDispatchers(LazyDispatchers->size(), Max + 5);
#ifdef ASM_BUILDER_CHECKING
    Builder.startChecking();
    Builder.generateLazyDispatchers(LazyDispatchers->size(), Max + 5);
#endif
    for (size_t I = LazyDispatchers->size(); I < Max + 5; I++) {
      LazyDispatchers->push_back((void *)Ptr);
      Ptr += 16;
    }
  }
}

void emptyTargetSetHandler(void) {
#if defined(__x86_64__) || defined(_M_X64)
  __asm__("ud2");
#elif defined(__aarch64__) || defined(_M_ARM64)
  __asm__("brk 1");
#elif defined(mips) || defined(__mips__) || defined(__mips)
  __asm__("break");
#else
  #error "Unsupported architecture!"
#endif
}

} // namespace rt
} // namespace typegraph
