#ifndef INCLUDED_VTABLE_H
#define INCLUDED_VTABLE_H

namespace sqrl {

// Given a pointer x return its vptr pointer
#define get_vtable(x) reinterpret_cast<void *>(x)

namespace details {
void insert_to_offset_map(size_t, size_t, int);

template <int offset, typename Derived>
constexpr void store_object_memory_layout_helper() {}

template <int offset, typename Derived, typename Base, typename... Bases>
constexpr void store_object_memory_layout_helper() {
  insert_to_offset_map(typeid(Derived).hash_code(), typeid(Base).hash_code(),
                       offset);
  store_object_memory_layout_helper<offset + sizeof(Base), Derived, Bases...>();
}
}; // namespace details

/*
A derived class and all its base classes(Must be in order)

Given:
class A: public B, public C, public D{};

The function should be called by:
store_object_memory_layout<A, B, C, D>();
*/
template <typename Derived, typename... Bases>
constexpr void store_object_memory_layout() {
  details::store_object_memory_layout_helper<0, Derived, Bases...>();
}

/*
Corresponding to store_object_memory_layout, this function is
to query sub object offset within an object.
*/
int get_object_offset(size_t mdo, size_t sub);

int get_vtable_offset(void *vptr);

size_t get_vtable_typeid(void *vptr);

}; // namespace sqrl

#endif