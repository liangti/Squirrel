#ifndef INCLUDED_MEMORY_ALLOCATOR_H
#define INCLUDED_MEMORY_ALLOCATOR_H
#include <memory/utility.h>

namespace sqrl {

class AllocatorImpl;

class AllocatorBase {
private:
  AllocatorImpl *impl;

public:
  AllocatorBase();
  word_t *allocate(size_t);
  void deallocate(word_t *);
};

// allocator interface
template <typename T> class Allocator {
private:
  int x;

public:
  AllocatorBase base;
  [[nodiscard("Memory leak")]] virtual T *allocate(size_t size) {
    return (T *)base.allocate(size * sizeof(T));
  }
  virtual void deallocate(T *t, size_t size) {
    for (size_t itr = 0; itr < size; itr++){
      base.deallocate((word_t *)t);
      t++;
    }
  }
  virtual ~Allocator() {}
};

}; // namespace sqrl

#endif