#ifndef INCLUDED_MEMORY_DESTROY_H
#define INCLUDED_MEMORY_DESTROY_H

#include <memory/block.h>

namespace sqrl {

template <class T> void destroy_at(T *ptr) { ptr->~T(); }

template <class T> void destroy_n(T *ptr, size_t n) {
  for (size_t itr = 0; itr < n; itr++) {
    destroy_at(ptr + itr);
  }
}

}; // namespace sqrl

#endif