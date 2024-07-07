#ifndef INCLUDED_MEMORY_ALLOCATOR_H
#define INCLUDED_MEMORY_ALLOCATOR_H
#include <memory/_allocator_impl.h>

namespace sqrl {

// allocator interface


template <typename T, typename Impl> class AllocatorInterface {
private:
  int x;

public:
  Impl impl;
  AllocatorInterface() noexcept: impl(){};
  [[nodiscard("Memory leak")]] T *allocate(size_t size) {
    return (T *)impl.allocate(size * sizeof(T));
  }
  void deallocate(T *t, size_t size) {
    // size is unused, for compatible with std API
    impl.deallocate((word_t *)t);
  }
  ~AllocatorInterface() noexcept = default;
};

template <typename T> using Allocator = AllocatorInterface<T, AllocatorImpl>;

}; // namespace sqrl

#endif