#ifndef LLVM_TYPEGRAPHS_TYPEGRAPH_BINARY_LOADER_H
#define LLVM_TYPEGRAPHS_TYPEGRAPH_BINARY_LOADER_H

#include "enforcing_rt.h"
#include <endian.h>
#include <llvm/Typegraph/Typegraph.h>

#if __BYTE_ORDER == __LITTLE_ENDIAN
// ok
#elif __BYTE_ORDER == __BIG_ENDIAN
#error "BIG ENDIAN ARCHITECTURES NOT SUPPORTED!"
#else
#error "Unclean endian"
#endif

class BinaryStreamReader {
  const char *Buffer;

public:
  BinaryStreamReader(const char *Buffer) : Buffer(Buffer) {}

  inline const char *getBuffer() { return Buffer; }

  inline int64_t readLong() {
    long L = *((const long *)Buffer);
    Buffer += sizeof(long);
    return L;
  }

  inline uint64_t readSize() {
    size_t L = *((const size_t *)Buffer);
    Buffer += sizeof(size_t);
    return L;
  }

  inline void *readPointer() {
    void *P = *((void *const *)Buffer);
    Buffer += sizeof(void *);
    return P;
  }

  inline std::string readString() {
    size_t Size = readSize();
    std::string S(Buffer, Size);
    Buffer += Size + 1;
    return S;
  }
};

namespace typegraph {
BinaryStreamReader loadTypegraphFromBinary(
    typegraph::TypeGraph &Graph, const char *GraphData,
    std::vector<std::unique_ptr<rt::FunctionInfos>, ProtectedAllocator<std::unique_ptr<rt::FunctionInfos>>> &Functions,
    void **&References);
} // namespace typegraph

#endif // LLVM_TYPEGRAPHS_TYPEGRAPH_BINARY_LOADER_H
