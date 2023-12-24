#ifndef CAST_INCLUDE_H
#define CAST_INCLUDE_H

#include <unordered_set>

#include <metaprogramming/hash.h>
#include <metaprogramming/tuple.h>
#include <metaprogramming/types.h>

#include <compiler/types.h>
#include <compiler/vtable.h>

namespace sqrl {

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

  if constexpr (sqrl::is_same_v<FromT, ToT>) {
    return from;
  } else if constexpr (std::is_base_of<ToT, FromT>::value) {
    return (ToT *)from;
  } else if constexpr (std::is_base_of<FromT, ToT>::value) {
    // must dereference in order to get runtime type info
    size_t from_id = typeid(*from).hash_code();
    size_t to_id = typeid(ToT).hash_code();
    if (sqrl::details::is_base_of(from_id, to_id)) {
      return nullptr;
    }
    return (ToT *)from;
  } else {
    size_t from_id = typeid(FromT).hash_code();
    size_t to_id = typeid(ToT).hash_code();
    size_t mdo_id = details::get_vtable_typeid(__gEt_vTaBle(from));
    if (sqrl::details::is_base_of(from_id, mdo_id) &&
        sqrl::details::is_base_of(to_id, mdo_id)) {
      int offset_to_mdo = details::get_object_offset(mdo_id, from_id);
      int offset_to_to = details::get_object_offset(mdo_id, to_id);
      char *raw = reinterpret_cast<char *>(from);
      raw = raw - offset_to_mdo + offset_to_to;
      return reinterpret_cast<ToT *>(raw);
    }
  }

  return nullptr;
}

}; // namespace sqrl

#endif