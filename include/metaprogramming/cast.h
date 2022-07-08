#ifndef CAST_INCLUDE_H
#define CAST_INCLUDE_H

#include <unordered_set>

#include <metaprogramming/tuple.h>
#include <metaprogramming/types.h>
#include <metaprogramming/hash.h>

#include <abi/vtable.h>

namespace sqrl {

namespace {

// if <T1, T2> pair exists it means T1 is base class of T2
static std::unordered_set<std::pair<size_t, size_t>, pair_hash> type_edges;

template <typename T1, typename T2>
constexpr inline void insert_type_edge(){
  if constexpr (std::is_base_of<T1, T2>::value) {
    type_edges.emplace(typeid(T1).hash_code(), typeid(T2).hash_code());
  }
  if constexpr (std::is_base_of<T2, T1>::value) {
    type_edges.emplace(typeid(T2).hash_code(), typeid(T1).hash_code());
  }
}

template <typename Empty> constexpr void register_types_first_loop() {}

/* 
Register type relationship of T1 against all the rest of types in
template type list.

For example:
register_types_first_loop<T1, T2, T3, T4>();

It should run:
insert_type_edge<T1, T2>();
insert_type_edge<T1, T3>();
insert_type_edge<T1, T4>();
*/
template <typename T1, typename T2, typename... Args>
constexpr void register_types_first_loop() {
  insert_type_edge<T1, T2>();
  register_types_first_loop<T1, Args...>();
}

template <bool> constexpr void register_types_second_loop() {}

/*
Register each type in template argument list.

For example:
register_types_second_loop<T1, T2, T3, T4>();

It should run:
register_types_first_loop<T1, T2, T3, T4>();
  insert_type_edge<T1, T2>();
  insert_type_edge<T1, T3>();
  insert_type_edge<T1, T4>();
register_types_first_loop<T2, T3, T4>();
  insert_type_edge<T2, T3>();
  insert_type_edge<T2, T4>();
...

Since insert_type_edge is bidirection it will eventually capture all
combination of pairs of types in template argument list.
*/
template <bool Na, typename T, typename... Args>
constexpr void register_types_second_loop() {
  register_types_first_loop<T, Args...>();
  register_types_second_loop<Na, Args...>();
}

}; // namespace

template <typename... Args> constexpr void type_info_register() {
  register_types_second_loop<true, Args...>();
}

template <typename To, typename From> bool is_a(From from) {
  static_assert(sqrl::is_pointer<From>::value,
                "Argument for is_a must be a pointer");
  using FromT = typename sqrl::remove_pointer<From>::type;
  static_assert(std::is_polymorphic<FromT>::value,
                "Argument for is_a must contain virtual member");
  if constexpr (sqrl::is_pointer<To>::value) {
    using ToT = typename sqrl::remove_pointer<To>::type;
    static_assert(std::is_polymorphic<ToT>::value,
                  "Query type of is_a must contain virtual member");
    return typeid(*from).hash_code() == typeid(ToT).hash_code();
  } else {
    static_assert(std::is_polymorphic<To>::value,
                  "Query type of is_a must contain virtual member");
    return typeid(*from).hash_code() == typeid(To).hash_code();
  }
}

template <typename To, typename From> To dyn_cast(From from) {
  static_assert(sqrl::is_pointer<From>::value,
                "Argument for dyn_cast must be a pointer");
  static_assert(sqrl::is_pointer<To>::value,
                "Return type of dyn_cast must be a pointer");

  using FromT = typename sqrl::remove_pointer<From>::type;
  using ToT = typename sqrl::remove_pointer<To>::type;

  static_assert(std::is_polymorphic<FromT>::value,
                "Argument for dyn_cast must contain virtual member");
  static_assert(std::is_polymorphic<ToT>::value,
                "Return type of dyn_cast must contain virtual member");

  if constexpr (sqrl::same_t<FromT, ToT>::value) {
    return from;
  } else if constexpr (std::is_base_of<ToT, FromT>::value) {
    return (ToT *)from;
  } else if constexpr (std::is_base_of<FromT, ToT>::value) {
    // must dereference in order to get runtime type info
    size_t from_id = typeid(*from).hash_code();
    size_t to_id = typeid(ToT).hash_code();
    if (type_edges.find(std::make_pair(from_id, to_id)) != type_edges.cend()) {
      return nullptr;
    }
    return (ToT *)from;
  }
  else{}

  static_assert("Not support yet");
}

}; // namespace sqrl

#endif