#ifndef INCLUDED_SMART_POINTERS_H
#define INCLUDED_SMART_POINTERS_H

#include <memory/allocator.h>
#include <metaprogramming/types.h>

namespace sqrl {

// by default use delete since unique_ptr has no idea
// what allocator input pointer is using
class _deleter_default {
public:
  template <typename T> static void clean(T *data) { delete data; }
};

// smart pointers expose to users

template <typename T, class Deleter = _deleter_default> class unique_ptr {
private:
  T *ptr;

public:
  explicit unique_ptr(T *_ptr) : ptr(_ptr) {}

  unique_ptr(unique_ptr<T, Deleter> &other) {
    ptr = other.ptr;
    if (this != &other) {
      other.ptr = nullptr;
    }
  }

  unique_ptr<T, Deleter> &operator=(unique_ptr<T, Deleter> &other) {
    ptr = other.ptr;
    if (this != &other) {
      other.ptr = nullptr;
    }
    return *this;
  }

  ~unique_ptr() { Deleter::clean(ptr); }

  T *operator->() { return ptr; }

  T &operator*() { return *ptr; }

  bool is_null() { return ptr == nullptr; }
};

template <typename T> class weak_ptr;

template <typename T, class Deleter = _deleter_default> class shared_ptr {
  friend weak_ptr<T>;

private:
  T *ptr;
  int *count;

public:
  explicit shared_ptr(T *_ptr) : ptr(_ptr), count(new int(1)) {}

  template <typename Allocator> shared_ptr(shared_ptr<T, Allocator> &other) {
    count = other.count;
    ptr = other.ptr;
    if (this != &other) {
      (*count)++;
    }
  }

  ~shared_ptr() {
    if ((*count) == 1) {
      reset();
    } else {
      (*count)--;
    }
  }
  T *operator->() { return ptr; }

  template <typename Allocator>
  shared_ptr<T, Deleter> &operator=(shared_ptr<T, Allocator> &rhs) {
    count = rhs.count;
    ptr = rhs.ptr;
    if (this != &rhs) {
      (*count)++;
    }
    return *this;
  }

  T &operator*() { return *ptr; }

  void reset() {
    Deleter::clean(count);
    Deleter::clean(ptr);
  }

  int get_count() { return *count; }
};

template <typename T> class weak_ptr {
private:
  T *ptr;
  int *count;

public:
  explicit weak_ptr(shared_ptr<T> &sp) {
    ptr = sp.ptr;
    count = sp.count;
  }
  weak_ptr<T> &operator=(shared_ptr<T> &sp) {
    ptr = sp.ptr;
    count = sp.count;
    return *this;
  }
  T &operator*() { return *ptr; }

  T *operator->() { return ptr; }

  int get_count() { return *count; }
};

// array as type is not supported for all smart pointer initializer
template <class T> struct _Never_true : std::false_type {};

template <class T> class unique_ptr<T[]> {
public:
  template <class... Args> unique_ptr(Args &&...args) {
    static_assert(_Never_true<T>::value, "Array as type is not supported");
  }
};

template <class T, size_t N> class unique_ptr<T[N]> {
public:
  template <class... Args> unique_ptr(Args &&...args) {
    static_assert(_Never_true<T>::value, "Array as type is not supported");
  }
};

template <class T> class unique_ptr<T *> {
public:
  template <class... Args> unique_ptr(Args &&...args) {
    static_assert(_Never_true<T>::value, "Pointer as type is not supported");
  }
};

template <class T> class shared_ptr<T[]> {
public:
  template <class... Args> shared_ptr(Args &&...args) {
    static_assert(_Never_true<T>::value, "Array as type is not supported");
  }
};

template <class T, size_t N> class shared_ptr<T[N]> {
public:
  template <class... Args> shared_ptr(Args &&...args) {
    static_assert(_Never_true<T>::value, "Array as type is not supported");
  }
};

template <class T> class shared_ptr<T *> {
public:
  template <class... Args> shared_ptr(Args &&...args) {
    static_assert(_Never_true<T>::value, "Pointer as type is not supported");
  }
};

// implement make_unique and make_shared
// make_xxx are all using sqrl::Allocator to manage memory
template <class T> class _make_deleter {

public:
  static void clean(T *data) {
    sqrl::Allocator<T> allocator;
    allocator.deallocate(data, sizeof(T));
  }
};

template <class T> struct unique_t {
  typedef sqrl::unique_ptr<T, _make_deleter<T>> type;
};

template <class T> struct unique_t<T[]> {
  static_assert(_Never_true<T>::value,
                "make_unique does not support array type");
};

template <class T, size_t N> struct unique_t<T[N]> {
  static_assert(_Never_true<T>::value,
                "make_unique does not support array type");
};

template <class T> struct unique_t<T *> {
  static_assert(_Never_true<T>::value,
                "make_unique does not support pointer type");
};

template <class T, class... Args>
typename unique_t<T>::type make_unique(Args &&...args) {
  sqrl::Allocator<T> local_allocator;
  T *slot = local_allocator.allocate(sizeof(T));
  T *init = new (slot) T(std::forward<Args>(args)...);
  return unique_ptr<T, _make_deleter<T>>(init);
}

template <class T> struct shared_t {
  typedef sqrl::shared_ptr<T, _make_deleter<T>> type;
};

template <class T> struct shared_t<T[]> {
  static_assert(_Never_true<T>::value,
                "make_shared does not support array type");
};

template <class T, size_t N> struct shared_t<T[N]> {
  static_assert(_Never_true<T>::value,
                "make_shared does not support array type");
};

template <class T> struct shared_t<T *> {
  static_assert(_Never_true<T>::value,
                "make_shared does not support pointer type");
};

template <class T, class... Args>
typename shared_t<T>::type make_shared(Args &&...args) {
  sqrl::Allocator<T> local_allocator;
  T *slot = local_allocator.allocate(sizeof(T));
  T *init = new (slot) T(std::forward<Args>(args)...);
  return shared_ptr<T, _make_deleter<T>>(init);
}

}; // namespace sqrl

#endif