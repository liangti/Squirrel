#ifndef INCLUDED_SMART_POINTERS_H
#define INCLUDED_SMART_POINTERS_H

#include <memory/allocator.h>
#include <metaprogramming/types.h>

namespace sqrl {

// by default use delete since unique_ptr has no idea
// what allocator input pointer is using
struct _default_deleter {
public:
  template <class T> static void clean(T *data) { delete data; }
};

// smart pointers expose to users

template <class T, class Deleter = _default_deleter> class unique_ptr {
private:
  T *ptr;

public:
  unique_ptr() : ptr(nullptr) {}
  explicit unique_ptr(T *_ptr) : ptr(_ptr) {}

  unique_ptr(const unique_ptr &) = delete;
  unique_ptr(unique_ptr &&other) {
    ptr = other.ptr;
    if (this != &other) {
      other.ptr = nullptr;
    }
  }

  unique_ptr &operator=(const unique_ptr &) = delete;
  unique_ptr &operator=(unique_ptr &&other) {
    ptr = other.ptr;
    if (this != &other) {
      other.ptr = nullptr;
    }
    return *this;
  }

  unique_ptr &operator=(nullptr_t) {
    ptr = nullptr;
    return *this;
  }

  ~unique_ptr() {
    if (is_null()) {
      return;
    }
    Deleter::clean(ptr);
  }

  T *operator->() { return ptr; }

  T &operator*() { return *ptr; }

  bool is_null() { return ptr == nullptr; }
};

template <class T> class weak_ptr;

template <class T, class Deleter = _default_deleter> class shared_ptr {
  friend weak_ptr<T>;

private:
  T *ptr;
  int *count;

public:
  shared_ptr() : ptr(nullptr), count(nullptr) {}
  explicit shared_ptr(T *_ptr) : ptr(_ptr), count(new int(1)) {}

  shared_ptr(const shared_ptr &other) {
    count = other.count;
    ptr = other.ptr;
    if (this != &other) {
      (*count)++;
    }
  }
  shared_ptr(shared_ptr &&other) {
    if (this == &other) {
      return;
    }
    count = other.count;
    ptr = other.ptr;
    // cleanup other
    other.ptr = nullptr;
    other.count = nullptr;
  }
  ~shared_ptr() {
    if (is_null()) {
      return;
    }
    if ((*count) == 1) {
      reset();
    } else {
      (*count)--;
    }
  }
  T *operator->() { return ptr; }

  shared_ptr &operator=(const shared_ptr &rhs) {
    count = rhs.count;
    ptr = rhs.ptr;
    if (this != &rhs) {
      (*count)++;
    }
    return *this;
  }

  shared_ptr &operator=(nullptr_t) {
    ptr = nullptr;
    count = nullptr;
    return *this;
  }

  T &operator*() { return *ptr; }

  inline bool is_null() { return count == nullptr; }

  inline void reset() {
    if (is_null()) {
      return;
    }
    delete count;
    Deleter::clean(ptr);
  }

  inline void reset(T *_ptr) {
    reset();
    ptr = _ptr;
    count = new int(1);
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

template <class T> struct unique_t {
  typedef sqrl::unique_ptr<T, _default_deleter> type;
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
  T *init = new T(std::forward<Args>(args)...);
  return unique_ptr<T>(init);
}

template <class T> struct shared_t {
  typedef sqrl::shared_ptr<T, _default_deleter> type;
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
  T *init = new T(std::forward<Args>(args)...);
  return shared_ptr<T>(init);
}

// TODO: implement alloc_unique & alloc_shared
// to use sqrl::Allocator to allocate and delete
struct _alloc_allocator {
  template <class T> static T *allocate() {
    sqrl::Allocator<T> local_allocator;
    T *slot = local_allocator.allocate(sizeof(T));
    return slot;
  }
};
struct _alloc_deleter {
  template <class T> static void clean(T *data) {
    sqrl::Allocator<T> allocator;
    allocator.deallocate(data, 1);
  }
};

}; // namespace sqrl

#endif