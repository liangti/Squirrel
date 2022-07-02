#ifndef INCLUDED_TUPLE_H
#define INCLUDED_TUPLE_H

#include <functional>

namespace sqrl {

// Tuple

template <typename... Ts> class Tuple {};

template <typename T, typename... Ts> class Tuple<T, Ts...> {
public:
  T head;
  Tuple<Ts...> tail;
  Tuple(const T &t, const Ts &...ts) : head(t), tail(ts...) {}
};

// _TupleGet

template <int idx, typename... Ts> class _TupleGet {};

template <typename T, typename... Ts> class _TupleGet<0, Tuple<T, Ts...>> {
public:
  static T get(Tuple<T, Ts...> &data) { return data.head; }
};

template <int idx, typename T, typename... Ts>
class _TupleGet<idx, Tuple<T, Ts...>> {
public:
  static auto get(Tuple<T, Ts...> &data) {
    return _TupleGet<idx - 1, Tuple<Ts...>>::get(data.tail);
  }
};

// get method

template <int idx, template <typename...> class Tuple, typename... Args>
auto get(Tuple<Args...> &tuple) {
  return _TupleGet<idx, Tuple<Args...>>::get(tuple);
}

// size method
template <class Tuple> int size(Tuple &tuple) { return 0; }

template <template <typename...> class Tuple, typename t, typename... ts>
int size(Tuple<t, ts...> &tuple) {
  return 1 + size(tuple.tail);
}

// make_tuple
// for automatically deducing types of input parameters

template <typename T> struct unwrap_reference { using type = T; };

template <typename T> struct unwrap_reference<std::reference_wrapper<T>> {
  using type = T &;
};

template <typename T>
using special_decay_t =
    typename unwrap_reference<typename std::decay<T>::type>::type;

template <typename... Args> auto make_tuple(Args &&...args) {
  return Tuple<special_decay_t<Args>...>(std::forward<Args>(args)...);
}

}; // namespace sqrl

#endif