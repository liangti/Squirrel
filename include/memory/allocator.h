#ifndef INCLUDED_MEMORY_ALLOCATOR_H
#define INCLUDED_MEMORY_ALLOCATOR_H

#if __cplusplus < 202002L
#include <concepts>
#elif __cplusplus >= 202002L
#include <type_traits>
#endif

#include <memory/_allocator_impl.h>
#include <metaprogramming/types.h>

namespace sqrl {

#if __cplusplus < 202002L

// allocator interface for C++17

// compile time check Allocator implementation satisfy AllocatorInterface
template <class AllocatorImpl, typename T> constexpr bool AllocatorInterface() {
  using allocateRT = typename std::result_of<decltype (
      &AllocatorImpl::allocate)(AllocatorImpl, size_t)>::type;
  using deallocateRT = typename std::result_of<decltype (
      &AllocatorImpl::deallocate)(AllocatorImpl, T *, size_t)>::type;
  return sqrl::is_same_v<allocateRT, T *> &&
         sqrl::is_same_v<deallocateRT, void>;
}

static_assert(AllocatorInterface<GenericAllocator, word_t>());
static_assert(AllocatorInterface<ObjectAllocator<int>, int>());

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

#endif

template <typename T> using Allocator = ObjectAllocator<T>;

}; // namespace sqrl

#endif