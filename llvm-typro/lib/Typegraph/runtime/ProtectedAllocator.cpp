#include "ProtectedAllocator.h"
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>

#define FreelistNumEntries 15
#define ChunkSize 0x4000
#define HeapStart ((char *)0x650000000000)

struct FreeList {
  size_t NumEntries;
  char *Entries[FreelistNumEntries];
};

struct Metadata {
  char *Pos;
  char *End;
  FreeList FreeLists[16];
};

static bool IsInitialized = false;

static inline void heapInit() {
  if (!IsInitialized) {
    void *Address = mmap(HeapStart, ChunkSize, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1, 0);
    if (Address == MAP_FAILED) {
      perror("initial mmap");
      _exit(1);
    }
    if (Address != HeapStart)
      _exit(2);
    Metadata *Meta = (Metadata *)Address;
    Meta->Pos = HeapStart + sizeof(Metadata);
    Meta->End = HeapStart + ChunkSize;
    // freelist is zero by default (kernel wiped memory)
    IsInitialized = true;
  }
}

static inline size_t roundSize(size_t N) { return (N + 15) & 0xfffffffffffffff0; }

static inline int getFreeListIndex(size_t N) { return N > 256 ? -1 : N / 16 - 1; }

void *protectedMalloc(size_t N) {
  N = roundSize(N);
  Metadata *Meta = (Metadata *) HeapStart;
  // return from freelist if applicable
  int Index = getFreeListIndex(N);
  if (Index >= 0 && Meta->FreeLists[Index].NumEntries > 0) {
      return Meta->FreeLists[Index].Entries[--Meta->FreeLists[Index].NumEntries];
  }

  // check that enough memory is available, otherwise reallocate
  while (Meta->Pos + N >= Meta->End) {
    auto Len = Meta->End - HeapStart;
    void *Result = mremap(HeapStart, Len, Len + ChunkSize, 0);
    if (Result == MAP_FAILED) {
      perror("mremap");
      _exit(3);
    }
    Meta->End += ChunkSize;
  }

  void *Result = Meta->Pos;
  Meta->Pos += N;
  return Result;
}

void protectedFree(void *P, size_t N) {
  N = roundSize(N);

  Metadata *Meta = (Metadata *) HeapStart;
  if (((char *) P) + N == Meta->Pos) {
    // Last assigned chunk can be simply free'd
    Meta->Pos -= N;
  } else {
    // check if freelist can be filled
    int Index = getFreeListIndex(N);
    if (Index >= 0 && Meta->FreeLists[Index].NumEntries < FreelistNumEntries) {
      Meta->FreeLists[Index].Entries[Meta->FreeLists[Index].NumEntries++] = (char *) P;
    }
    // otherwise we just ignore that memory. Don't care much about leaks.
  }
}

void protectedMakeWriteable() {
  heapInit();
  Metadata *Meta = (Metadata *) HeapStart;
  if (mprotect(HeapStart, Meta->End - HeapStart, PROT_READ | PROT_WRITE) != 0) {
    perror("mprotect rw");
    _exit(1);
  }
}

void protectedMakeReadonly() {
  heapInit();
  Metadata *Meta = (Metadata *) HeapStart;
  if (mprotect(HeapStart, Meta->End - HeapStart, PROT_READ) != 0) {
    perror("mprotect rw");
    _exit(1);
  }
}
