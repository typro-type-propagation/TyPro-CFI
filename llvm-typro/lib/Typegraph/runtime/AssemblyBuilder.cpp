#include "AssemblyBuilder.h"
#include <algorithm>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <vector>
#include <cstdint>
#include <limits>

#define FINAL_CHECK true

namespace typegraph {
namespace rt {

namespace {

#define ARM_CONDITIONAL_BRANCH_IMM 19
#define ARM_BRANCH_IMM 26
#define MIPS_BRANCH_ABS 27
#define MIPS_BRANCH_REL 20
#define MIPS_BRANCH_REL16 21
#define MIPS_BRANCH_MEMREL16 22

struct JmpTarget {
  AssemblyBuilder &Builder;
  size_t OffsetFrom;
  char Type = 1;

  void setTarget(ssize_t Offset) {
    if (Type == 0) return;
    // absolute jump encodings
    if (Type == MIPS_BRANCH_ABS) {
      // absolute, 26-bit jump based on "next" instruction
      uintptr_t AddrBase = (uintptr_t) Builder.byOffset(OffsetFrom);
      uintptr_t AddrDest = (uintptr_t) Builder.byOffset(Offset);
      assert((AddrBase & 0xffffffff0000000) == (AddrDest & 0xffffffff0000000));
      ((uint32_t*) Builder.byOffset(OffsetFrom))[-1] |= (AddrDest & 0xfffffff)/4;
      return;
    }
    // function-relative encodings. In MIPS, $t9 in PIC is always the start of the current function, we can address relative to that.
    if (Type == MIPS_BRANCH_MEMREL16) {
      Offset = Offset - Builder.currentFunctionStartOffset();
      assert(-0x8000 <= Offset && Offset <= 0x7fff);
      ((uint32_t*) Builder.byOffset(OffsetFrom))[-1] |= (Offset & 0xffffu);
      return;
    }
    // relative jump encodings
    Offset = Offset - OffsetFrom;
    if (Type == 1) {
      assert(0 <= Offset && Offset <= 0x7f);
      Builder.byOffset(OffsetFrom - Type)[0] = Offset;
    } else if (Type == 4) {
      assert(0 <= Offset && Offset <= 0x7fffffff);
      ((int *)Builder.byOffset(OffsetFrom - Type))[0] = Offset;
    } else if (Type == ARM_CONDITIONAL_BRANCH_IMM) {
      // aarch64's conditional branch mode (offset/4), <<5 - 19 bits given (signed)
      assert(0 <= Offset && Offset <= 0xfffff);
      ((uint32_t*) Builder.byOffset(OffsetFrom))[0] |= (Offset/4) << 5;
    } else if (Type == ARM_BRANCH_IMM) {
      // aarch64's branch mode (offset/4) - 26 bits given
      assert(0 <= Offset && Offset <= 0x4ffffff);
      ((uint32_t*) Builder.byOffset(OffsetFrom))[0] |= Offset/4;
    } else if (Type == MIPS_BRANCH_REL) {
      assert(0 <= Offset && Offset <= 0x1ffff);
      ((uint32_t*) Builder.byOffset(OffsetFrom))[-1] |= Offset/4;
    } else if (Type == MIPS_BRANCH_REL16) {
      assert(0 <= Offset && Offset <= 0x7fff);
      ((uint32_t*) Builder.byOffset(OffsetFrom))[-1] |= Offset/4;
    }
  }

  JmpTarget(AssemblyBuilder &Builder, size_t OffsetFrom, char Type = 1)
      : Builder(Builder), OffsetFrom(OffsetFrom), Type(Type) {}

  JmpTarget(const JmpTarget &T) : Builder(T.Builder), OffsetFrom(T.OffsetFrom), Type(T.Type) { }

  JmpTarget& operator=(const JmpTarget &T) {
    Builder = T.Builder;
    OffsetFrom = T.OffsetFrom;
    Type = T.Type;
    return *this;
  }
};

// Constant list: https://stackoverflow.com/a/66249936
#if defined(__x86_64__) || defined(_M_X64)
#include "AsmWriterx86_64.h"
#elif defined(__aarch64__) || defined(_M_ARM64)
#include "AsmWriterAArch64.h"
#elif (defined(mips) || defined(__mips__) || defined(__mips)) && UINTPTR_MAX == 0xffffffffffffffff
#include "AsmWriterMips64El.h"
#else
#error "Unsupported architecture!"
#endif

} // namespace

AssemblyBuilder::~AssemblyBuilder() {
  if (PageStart) {
    int Result = mprotect(PageStart, PageSize, PROT_READ | PROT_EXEC);
    assert(Result == 0);

    for (auto It : OutputAddresses) {
      *It.first = (void *)(PageStart + It.second);
      dprintf("Wrote %p (offset %ld) to %p\n", (void *)(PageStart + It.second), It.second, (void*) It.first);
    }

    dprintf("Wrote %ld dispatchers: %p - %p\n", NumDispatchers, (void*) PageStart, (void*) CurrentPos);
  }
}

void AssemblyBuilder::assertSpace(size_t Bytes) {
  if (!PageStart) {
    PageSize = getpagesize();
    PageStart = (char *)mmap((void*) NextMapPos, PageSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    assert(PageStart != MAP_FAILED);
    CurrentPos = PageStart;
    NextMapPos = (uintptr_t) PageStart + PageSize;
  }
  size_t CurrentOffset = CurrentPos - PageStart;
  if (PageSize - CurrentOffset < Bytes) {
    char *OldPageStart = PageStart;
    PageStart = (char *)mremap(PageStart, PageSize, PageSize + getpagesize(), MREMAP_MAYMOVE);
    assert(PageStart != MAP_FAILED);
    if (OldPageStart != PageStart) {
      dprintf("[RT] Warning: jit page moved (%p => %p)!\n", (void*) OldPageStart, (void*) PageStart);
    }
    PageSize += getpagesize();
    CurrentPos = PageStart + CurrentOffset;
    NextMapPos = (uintptr_t) PageStart + PageSize;
  }
}

void AssemblyBuilder::generateDispatcher(size_t Index, std::vector<FunctionInfos> &Targets, void **OutputAddress,
                                         FunctionID PreferredID) {
  /*
  if (Targets.empty()) {
    *OutputAddress = (void *)emptyTargetSetHandler;
    return;
  }
  // */

  if (!FINAL_CHECK && Targets.size() == 1) {
    *OutputAddress = Targets[0].Address;
    return;
  }

  assertSpace(16);
  roundCurrentPosTo16Bytes();
  OutputAddresses[OutputAddress] = CurrentPos - PageStart;
  CurrentFunctionOffset = CurrentPos - PageStart;

  AsmWriter Asm{*this};
  // Asm.writeDebuggerTrap();
  Asm.genSwitch(Targets, PreferredID);
  Asm.errorCasesJumpHere();
  // debug invalid ID
  Asm.genInvalidIdDebugTrap(Index);
  // crash
  Asm.writeCrash();

  NumDispatchers++;
}

char *AssemblyBuilder::generateLazyDispatchers(int Min, int Max) {
  AsmWriter Asm{*this};
  assertSpace(Asm.getLazyDispatcherEstimatedBytes(Max - Min));
  char *Start = currentPos();

  for (int I = Min; I < Max; I++) {
    Asm.genLazyDispatcher(I);
  }
  Asm.genLazyDispatcherTail();

  return Start;
}


uintptr_t AssemblyBuilder::NextMapPos = 0x6a0000000000;

} // namespace rt
} // namespace typegraph
