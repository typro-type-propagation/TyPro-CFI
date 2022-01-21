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

std::vector<void *> LazyDispatchers = {
#if defined(__x86_64__) || defined(_M_X64)
    (void*) tgLazyDispatcher<0>,
    (void*) tgLazyDispatcher<1>,
    (void*) tgLazyDispatcher<2>,
    (void*) tgLazyDispatcher<3>,
    (void*) tgLazyDispatcher<4>,
    (void*) tgLazyDispatcher<5>,
    (void*) tgLazyDispatcher<6>,
    (void*) tgLazyDispatcher<7>,
    (void*) tgLazyDispatcher<8>,
    (void*) tgLazyDispatcher<9>,
    (void*) tgLazyDispatcher<10>,
    (void*) tgLazyDispatcher<11>,
    (void*) tgLazyDispatcher<12>,
    (void*) tgLazyDispatcher<13>,
    (void*) tgLazyDispatcher<14>,
    (void*) tgLazyDispatcher<15>,
#endif
};

void assertLazyDispatchersExist(size_t Max) {
  if (LazyDispatchers.size() < Max) {
    AssemblyBuilder Builder;
    char *Ptr = Builder.generateLazyDispatchers(LazyDispatchers.size(), Max + 5);
    for (size_t I = LazyDispatchers.size(); I < Max + 5; I++) {
      LazyDispatchers.push_back((void *)Ptr);
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
