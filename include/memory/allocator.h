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

// compile time check Allocator implementation satisfy AllocatorInterface
static_assert(AllocatorInterface<ObjectAllocator<int>, int>());
static_assert(AllocatorInterface<GenericAllocator, word_t>());

#elif __cplusplus >= 202002L

// allocator interface for C++20
template <typename Impl, typename T>
concept AllocatorInterface = requires(Impl impl, T *tp) {
  { impl.allocate(size_t{}) }
  ->std::same_as<T *>;
  { impl.deallocate(tp, size_t{}) }
  ->std::same_as<void>;
};

// compile time check Allocator implementation satisfy AllocatorInterface
static_assert(AllocatorInterface<ObjectAllocator<int>, int>);
static_assert(AllocatorInterface<GenericAllocator, word_t>);

#endif

template <typename T> using Allocator = ObjectAllocator<T>;

}; // namespace sqrl

#endif