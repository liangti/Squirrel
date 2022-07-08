#ifndef INCLUDED_HASH_H
#define INCLUDED_HASH_H

#include <functional>

namespace sqrl {

struct pair_hash {
  template <class T1, class T2>
  std::size_t operator()(const std::pair<T1, T2> &p) const {
    auto h1 = std::hash<T1>{}(p.first);
    auto h2 = std::hash<T2>{}(p.second);

    // order does matter
    return h1 ^ h2 + h1;
  }
};

struct pair_hash_unordered {
  template <class T1, class T2>
  std::size_t operator()(const std::pair<T1, T2> &p) const {
    auto h1 = std::hash<T1>{}(p.first);
    auto h2 = std::hash<T2>{}(p.second);

    // order doesn't matter
    return h1 ^ h2;
  }
};

}; // namespace sqrl

#endif