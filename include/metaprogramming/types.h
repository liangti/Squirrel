#ifndef INCLUDED_TYPES_H
#define INCLUDED_TYPES_H

#include <type_traits>
#include <utility>

namespace sqrl {

// integral constant
template <class T, T v> struct integral_constant {
  static constexpr T value = v;
  using value_type = T;
};

struct true_type : integral_constant<bool, true> {};
struct false_type : integral_constant<bool, false> {};

// same_t check types equivalent
template <typename T, typename U> struct same_t : false_type {};

template <typename T> struct same_t<T, T> : true_type {};

// remove_cv extract type from const volatile

template <typename T> struct remove_cv { using type = T; };

template <typename T> struct remove_cv<const T> { using type = T; };

template <typename T> struct remove_cv<volatile T> { using type = T; };
template <typename T> struct remove_cv<const volatile T> { using type = T; };

// conditional <bool, T, U> gives T if bool=True, else U
template <bool, typename T, typename F> struct conditional { using type = T; };

template <typename T, typename F> struct conditional<false, T, F> {
  using type = F;
};

// remove_reference <T> remove & && from a type
template <typename T> struct remove_reference { using type = T; };

template <typename T> struct remove_reference<T &> { using type = T; };

template <typename T> struct remove_reference<T &&> { using type = T; };

// remove_array <T> remove [] from a type
template <typename T> struct remove_array { using type = T; };

template <typename T> struct remove_array<T[]> { using type = T; };

template <typename T, int N> struct remove_array<T[N]> { using type = T; };

// remove_pointer <T> remove * from a type
template <typename T> struct remove_pointer { using type = T; };
template <typename T> struct remove_pointer<T*> { using type = T; };
template <typename T> struct remove_pointer<T* const> { using type = T; };
template <typename T> struct remove_pointer<const T*> { using type = T; };

// is_pointer determine if a type is a pointer
template <typename T> struct is_pointer : false_type {};
template <typename T> struct is_pointer<T *> : true_type {};

// is_array determine if a type is an array
template <typename T> struct is_array : false_type {};

template <typename T> struct is_array<T[]> : true_type {};

template <typename T, int N> struct is_array<T[N]> : true_type {};

// is_function determine if a type is a function, TODO see cppreference for full
// support
template <typename T> struct is_function : false_type {};

template <typename Ret, typename... Args>
struct is_function<Ret(Args...)> : true_type {};

// ...

// decay <T> remove array, reference, pointer, cv from a type
template <typename T> struct decay {
private:
  using U = typename remove_reference<T>::type;

public:
  using type = typename conditional<
      is_array<U>::value, typename remove_array<U>::type *,
      typename conditional<std::is_function<U>::value,
                           typename std::add_pointer<U>::type,
                           typename remove_cv<U>::type>::type>::type;
};

// enable_if<bool, T> contains type=T if bool=true
template <bool, typename T = void> struct enable_if {};

template <typename T> struct enable_if<true, T> { using type = T; };

// result_of<T> contains the type of T's return type
template <typename T> struct result_of {};

template <typename F, typename... Args> struct result_of<F(Args...)> {
  using type = decltype(std::declval<F>()(std::declval<Args>()...));
};

// address_of(a) returns address of an object
// why not &a? because sometimes object can overload oeprator &
// address_of handle that, see test for an example
template <typename T>
typename enable_if<std::is_object<T>::value, T *>::type address_of(T &t) {
  // because object can overload operator
  // consider T contains cv qualifier
  return reinterpret_cast<T *>(
      &const_cast<char &>(reinterpret_cast<const char &>(t)));
}

template <typename T>
typename enable_if<!std::is_object<T>::value, T *>::type address_of(T &t) {
  return &t;
}

template <class T> constexpr T &FUN(T &t) noexcept { return t; }
template <class T> void FUN(T &&) = delete;

// reference_wrapper<T> wrap a reference into a copyable assignable object
template <typename T> class reference_wrapper {
public:
  // construct
  reference_wrapper(const T &t) { _ptr = const_cast<T *>(address_of(t)); }
  reference_wrapper(T &&t) = delete;

  // copyable
  reference_wrapper(const reference_wrapper &) = default;

  // assignable
  reference_wrapper &operator=(const reference_wrapper &) = default;

  // access
  T &operator*() { return *_ptr; }

  // destroy
  ~reference_wrapper() { _ptr = nullptr; }

private:
  T *_ptr;
};

// nothrow
template <class Fp, class... Args>
struct is_nothrow_constructible
    : public std::integral_constant<bool,
                                    noexcept(Fp(std::declval<Args>()...))> {};

}; // namespace sqrl

#endif