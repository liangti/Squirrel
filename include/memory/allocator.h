#ifndef INCLUDED_MEMORY_ALLOCATOR_H
#define INCLUDED_MEMORY_ALLOCATOR_H
#include <concepts>

#include <memory/_allocator_impl.h>
#include <metaprogramming/types.h>

namespace sqrl {

#if __cplusplus < 202002L

// allocator interface for C++17
template <typename T, typename Impl> class AllocatorInterface {
private:
  Impl impl;

public:
  AllocatorInterface() noexcept : impl(){};
  [[nodiscard("Memory leak")]] T *allocate(size_t size) {
    return impl.allocate(size);
  }
  void deallocate(T *t, size_t size) {
    // size is unused, for compatible with std API
    impl.deallocate(t);
  }
  ~AllocatorInterface() noexcept = default;
};

template <typename T> using Allocator = ObjectAllocator<T>;

// compile time check Allocator implementation satisfy AllocatorInterface
// TODO

#elif __cplusplus >= 202002L

// allocator interface for C++20
template <typename T, typename Impl>
concept AllocatorInterface = requires(Impl impl, T *tp) {
  { impl.allocate(size_t{}) }
  ->std::same_as<T *>;
  { impl.deallocate(tp, size_t{}) }
  ->std::same_as<void>;
};

// compile time check Allocator implementation satisfy AllocatorInterface
static_assert(AllocatorInterface<int, ObjectAllocator<int>>);
static_assert(AllocatorInterface<word_t, GenericAllocator>);

template <typename T> using Allocator = ObjectAllocator<T>;

#endif

}; // namespace sqrl

#endif