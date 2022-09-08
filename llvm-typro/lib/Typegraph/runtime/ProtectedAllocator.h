#ifndef LLVM_TYPEGRAPHS_PROTECTEDALLOCATOR_H
#define LLVM_TYPEGRAPHS_PROTECTEDALLOCATOR_H

#include <memory>

void *protectedMalloc(size_t N);
void protectedFree(void *, size_t N);
void protectedMakeWriteable();
void protectedMakeReadonly();

struct ProtectedAllocatorWriteableScope {
  inline ProtectedAllocatorWriteableScope() { protectedMakeWriteable(); }
  inline ~ProtectedAllocatorWriteableScope() { protectedMakeReadonly(); }
};

template <class T> class ProtectedAllocator {
public:
  using value_type = T;

  ProtectedAllocator() noexcept {}
  template <class U> ProtectedAllocator(ProtectedAllocator<U> const &) noexcept {}

  value_type *allocate(std::size_t N) { return static_cast<value_type *>(protectedMalloc(N * sizeof(value_type))); }

  void deallocate(value_type *P, std::size_t N) noexcept { protectedFree(static_cast<void *>(P), N * sizeof(value_type)); }
};

template <class T, class U> bool operator==(ProtectedAllocator<T> const &, ProtectedAllocator<U> const &) noexcept {
  return true;
}

template <class T, class U> bool operator!=(ProtectedAllocator<T> const &X, ProtectedAllocator<U> const &Y) noexcept {
  return !(X == Y);
}

template <> class ProtectedAllocator<void> {
public:
  typedef void * pointer;
  typedef const void* const_pointer;
  typedef void value_type;
};

#endif // LLVM_TYPEGRAPHS_PROTECTEDALLOCATOR_H
