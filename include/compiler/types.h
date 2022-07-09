#ifndef INCLUDED_COMPILER_TYPES_H
#define INCLUDED_COMPILER_TYPES_H

#include <unordered_set>

#include <metaprogramming/hash.h>

namespace sqrl {

namespace details {

// given two type id return bool to indicate if one is base of the other
bool is_base_of(size_t, size_t);

}; // namespace details

namespace compiler {
void insert_type_edge(size_t, size_t);

template <typename Empty> constexpr void register_types_first_loop() {}

/*
Register type relationship of T1 against all the rest of types in
template type list.

For example:
register_types_first_loop<T1, T2, T3, T4>();

It should run:
insert_type_edge(T1, T2);
insert_type_edge(T2, T1);
insert_type_edge(T1, T3);
insert_type_edge(T3, T1);
insert_type_edge(T1, T4);
insert_type_edge(T4, T1);
*/
template <typename T1, typename T2, typename... Args>
constexpr void register_types_first_loop() {
  if constexpr (std::is_base_of<T1, T2>::value) {
    insert_type_edge(typeid(T1).hash_code(), typeid(T2).hash_code());
  }
  if constexpr (std::is_base_of<T2, T1>::value) {
    insert_type_edge(typeid(T2).hash_code(), typeid(T1).hash_code());
  }
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

template <typename... Args> constexpr void type_info_register() {
  compiler::register_types_second_loop<true, Args...>();
}
}; // namespace compiler

}; // namespace sqrl
#endif