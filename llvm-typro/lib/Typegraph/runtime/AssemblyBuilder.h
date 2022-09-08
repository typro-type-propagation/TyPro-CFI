#ifndef LLVM_TYPEGRAPHS_ASSEMBLYBUILDER_H
#define LLVM_TYPEGRAPHS_ASSEMBLYBUILDER_H

#define ASM_BUILDER_CHECKING

#include "enforcing_rt.h"
#include <cstring>
#include <map>
#include <unistd.h>

namespace typegraph {
namespace rt {

class AssemblyBuilder {
  std::map<void **, size_t> OutputAddresses;
  char *PageStart = nullptr;
  size_t PageSize = 0;
  char *CurrentPos = nullptr;
  size_t CurrentFunctionOffset;
  size_t NumDispatchers = 0;

  static uintptr_t NextMapPos;

  bool Checking = false;

  inline void roundCurrentPosTo16Bytes() {
    uintptr_t Pos = (uintptr_t)CurrentPos;
    if (Pos & 0xf) {
      CurrentPos = (char *)((Pos ^ (Pos & 0xf)) + 16);
    }
  }

  /**
   * Make sure that the current buffer has at least X bytes free space
   * @param Bytes
   */
  void assertSpace(size_t Bytes);

public:
  inline size_t currentOffset() { return CurrentPos - PageStart; }
  inline char *currentPos() { return CurrentPos; }
  inline char *byOffset(size_t Offset) { return PageStart + Offset; }
  inline size_t currentFunctionStartOffset() { return CurrentFunctionOffset; }

  inline uint32_t read32(size_t Offset) {
    return ((uint32_t*)byOffset(Offset))[0];
  }

  inline void write(const char *Buffer, size_t Len, size_t CheckLen=0xffffffff) {
    if (Checking) {
      if (memcmp(Buffer, CurrentPos, CheckLen == 0xffffffff ? Len : CheckLen) != 0) {
        fprintf(stderr, "[ERROR] Generated assembly has been altered! Position: %p  offset: 0x%lx  expected: buffer @ %p\n", CurrentPos, CurrentPos - PageStart, Buffer);
        _exit(1);
      }
    } else {
      assertSpace(Len);
      memmove(CurrentPos, Buffer, Len);
    }
    CurrentPos += Len;
  }

  inline void write(uint64_t C) {
    if (Checking) {
      if (((uint64_t *) CurrentPos)[0] != C) {
        fprintf(stderr, "[ERROR] Generated assembly has been altered! Position: %p  offset: 0x%lx  expected: 0x%lx  was: 0x%lx\n", CurrentPos, CurrentPos - PageStart, C, ((uint64_t *) CurrentPos)[0]);
        _exit(1);
      }
    } else {
      assertSpace(sizeof(uint64_t));
      ((uint64_t *)CurrentPos)[0] = C;
    }
    CurrentPos += sizeof(uint64_t);
  }

  inline void write(uint32_t C) {
    if (Checking) {
      if (((uint32_t *) CurrentPos)[0] != C) {
        fprintf(stderr, "[ERROR] Generated assembly has been altered! Position: %p  offset: 0x%lx  expected: 0x%x  was: 0x%x\n", CurrentPos, CurrentPos - PageStart, C, ((uint32_t *) CurrentPos)[0]);
        _exit(1);
      }
    } else {
      assertSpace(sizeof(uint32_t));
      ((uint32_t *)CurrentPos)[0] = C;
    }
    CurrentPos += sizeof(uint32_t);
  }

  inline void write32(uint32_t C) {
    if (Checking) {
      if (((uint32_t *) CurrentPos)[0] != C) {
        fprintf(stderr, "[ERROR] Generated assembly has been altered! Position: %p  offset: 0x%lx  expected: 0x%x  was: 0x%x\n", CurrentPos, CurrentPos - PageStart, C, ((uint32_t *) CurrentPos)[0]);
        _exit(1);
      }
    } else {
      assertSpace(sizeof(uint32_t));
      ((uint32_t *)CurrentPos)[0] = C;
    }
    CurrentPos += sizeof(uint32_t);
  }

  inline void write(uint8_t C) {
    if (Checking) {
      if (((uint8_t *) CurrentPos)[0] != C) {
        fprintf(stderr, "[ERROR] Generated assembly has been altered! Position: %p  offset: 0x%lx  expected: 0x%hhx  was: 0x%hhx\n", CurrentPos, CurrentPos - PageStart, C, ((uint8_t *) CurrentPos)[0]);
        _exit(1);
      }
    } else {
      assertSpace(sizeof(uint8_t));
      ((uint8_t *)CurrentPos)[0] = C;
    }
    CurrentPos += sizeof(uint8_t);
  }

  inline void write8At(ssize_t Offset, uint8_t C) {
    if (Checking) {
      if (((uint8_t *)byOffset(Offset))[0] != C) {
        fprintf(stderr, "[ERROR] Generated assembly has been altered! Position: %p  offset: 0x%lx  expected: 0x%x  was: 0x%x\n", byOffset(Offset), byOffset(Offset) - PageStart, C, ((uint8_t *)byOffset(Offset))[0]);
        _exit(1);
      }
    } else {
      ((uint8_t *)byOffset(Offset))[0] = C;
    }
  }

  inline void write32At(ssize_t Offset, uint32_t C) {
    if (Checking) {
      if (((uint32_t *)byOffset(Offset))[0] != C) {
        fprintf(stderr, "[ERROR] Generated assembly has been altered! Position: %p  offset: 0x%lx  expected: 0x%x  was: 0x%x\n", byOffset(Offset), byOffset(Offset) - PageStart, C, ((uint32_t *)byOffset(Offset))[0]);
        _exit(1);
      }
    } else {
      ((uint32_t *)byOffset(Offset))[0] = C;
    }
  }

  inline void write64At(ssize_t Offset, uint64_t C) {
    if (Checking) {
      if (((uint64_t *)byOffset(Offset))[0] != C) {
        fprintf(stderr, "[ERROR] Generated assembly has been altered! Position: %p  offset: 0x%lx  expected: 0x%lx  was: 0x%lx\n", byOffset(Offset), byOffset(Offset) - PageStart, C, ((uint64_t *)byOffset(Offset))[0]);
        _exit(1);
      }
    } else {
      ((uint64_t *)byOffset(Offset))[0] = C;
    }
  }

  inline void write32Mask(uint32_t C, uint32_t CheckMask) {
    // write all bits, but check only the bits from mask
    if (Checking) {
      uint32_t L = ((uint32_t *) CurrentPos)[0];
      if ((L & CheckMask) != (C & CheckMask)) {
        fprintf(stderr, "[ERROR] Generated assembly has been altered! Position: %p  offset: 0x%lx  expected: 0x%x  was: 0x%x  (mask 0x%x)\n", CurrentPos, CurrentPos - PageStart, C, ((uint32_t *) CurrentPos)[0], CheckMask);
        _exit(1);
      }
    } else {
      assertSpace(sizeof(uint32_t));
      ((uint32_t *)CurrentPos)[0] = C;
    }
    CurrentPos += sizeof(uint32_t);
  }

  /**
   * Make the generated code executable, write the generated addresses
   */
  ~AssemblyBuilder();

  void generateDispatcher(size_t Index, std::vector<FunctionInfos, ProtectedAllocator<FunctionInfos>> &Targets, void **OutputAddress, FunctionID PreferredID);

  char *generateLazyDispatchers(int Min, int Max);

#ifdef ASM_BUILDER_CHECKING
  void startChecking();
#endif
};

} // namespace rt
} // namespace typegraph

#endif // LLVM_TYPEGRAPHS_ASSEMBLYBUILDER_H
