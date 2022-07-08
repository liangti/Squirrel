#include <unordered_map>

#include <abi/vtable.h>
#include <metaprogramming/hash.h>

namespace sqrl {

namespace {
/*
Background:

Given inheritance like this:
class A: public B, public C, public D{};

And pointers:
A* a = new A();
B* b = dynamic_cast<B*>(a);
C* c = dynamic_cast<C*>(c);
D* d = dynamic_cast<D*>(d);

The memory layout looks like:
+---------------------+------+------
+ B's vptr and members+      +  b
+---------------------+      +------
+ C's vptr and members+  a   +  c
+---------------------+      +------
+ D's vptr and members+      +  d
+---------------------+------+------

For object_memory_offset:
- key is a pair of typeid(<mdo, sub>)
- value is the offset of casting mdo to sub

Let's say we want to cast from A to D the offset should be:
sizeof(B) + sizeof(C)
*/
std::unordered_map<std::pair<size_t, size_t>, int, pair_hash>
    objects_memory_offset;

}; // namespace

namespace details {
void insert_to_offset_map(size_t derived_id, size_t base_id, int offset) {
  objects_memory_offset.emplace(std::make_pair(derived_id, base_id), offset);
}
}; // namespace details

/*
Given:
class A: public B, public C, public D{};

Initialize by:
A* a = new A();
B* b = dynamic_cast<B*>(a);

The vtable memory layout looks like:
                 offset
a's vptr  -----> typeinfo(A)
                 A's virtual functions
                 offset
b's vptr  -----> typeinfo(A)
                 B's virtual functions

Offset stores offset between current cast object's vptr to most derived object
top.

So that:
- b's offset should be sizeof(A)
- c's offset should be sizeof(A) + sizeof(B)
...
*/
int get_vtable_offset(void *vptr) {
  int **vt = reinterpret_cast<int **>(vptr);
  return vt[0][-4];
}

/*
                      offset
                      type_info ptr         ---> type info object
object ptr(vptr) ---> vtable(first virtual)
*/
size_t get_vtable_typeid(void *vptr) {
  std::type_info ***ti = reinterpret_cast<std::type_info ***>(vptr);
  return ti[0][-1][0].hash_code();
}

int get_object_offset(size_t mdo, size_t sub) {
  auto itr = objects_memory_offset.find(std::make_pair(mdo, sub));
  if (itr != objects_memory_offset.cend()) {
    return itr->second;
  }
  return -1;
}

}; // namespace sqrl