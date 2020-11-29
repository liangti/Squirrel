#ifndef INCLUDED_ATTRIBUTE_H
#define INCLUDED_ATTRIBUTE_H

#ifndef SQL_CHECK_ATTRIBUTE
#error Must define SQL_CHECK_ATTRIBUTE macro in order to use hasattr template class
#endif

#include <utility>

namespace sql{

template<class ...>
using void_t = void;

template <typename T, typename=void>
struct checkattr: std::false_type{};

template <typename T>
struct checkattr<T, void_t<decltype(std::declval<T>().SQL_CHECK_ATTRIBUTE)>>
: std::true_type{};

};

#endif