#ifndef INCLUDED_TYPES_H
#define INCLUDED_TYPES_H

#include <type_traits>

namespace sql{

template<typename T, typename U>
struct same_t: std::false_type{};

template<typename T>
struct same_t<T, T>: std::true_type{};

}; // namespace sql

#endif