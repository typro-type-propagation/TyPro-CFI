#ifndef LLVM_TYPEGRAPHS_ASSEMBLYBUILDER_H
#define LLVM_TYPEGRAPHS_ASSEMBLYBUILDER_H

#include "enforcing_rt.h"
#include <cstring>
#include <map>

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
  inline void write(const char *Buffer, size_t Len) {
    assertSpace(Len);
    memmove(CurrentPos, Buffer, Len);
    CurrentPos += Len;
  }
  inline void write(uint64_t C) {
    assertSpace(sizeof(uint64_t));
    ((uint64_t *) CurrentPos)[0] = C;
    CurrentPos += sizeof(uint64_t);
  }
  inline void write(uint32_t C) {
    assertSpace(sizeof(uint32_t));
    ((uint32_t *) CurrentPos)[0] = C;
    CurrentPos += sizeof(uint32_t);
  }
  inline void write32(uint32_t C) {
    assertSpace(sizeof(uint32_t));
    ((uint32_t *) CurrentPos)[0] = C;
    CurrentPos += sizeof(uint32_t);
  }
  inline void write(uint8_t C) {
    assertSpace(sizeof(uint8_t));
    ((uint8_t *) CurrentPos)[0] = C;
    CurrentPos += sizeof(uint8_t);
  }

  /**
   * Make the generated code executable, write the generated addresses
   */
  ~AssemblyBuilder();

  void generateDispatcher(size_t Index, std::vector<FunctionInfos> &Targets, void **OutputAddress, FunctionID PreferredID);

  char *generateLazyDispatchers(int Min, int Max);
};

} // namespace rt
} // namespace typegraph

#endif // LLVM_TYPEGRAPHS_ASSEMBLYBUILDER_H
