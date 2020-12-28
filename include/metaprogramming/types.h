#ifndef INCLUDED_TYPES_H
#define INCLUDED_TYPES_H

#include <type_traits>

namespace sql{

// same_t check types equivalent
template<typename T, typename U>
struct same_t: std::false_type{};

template<typename T>
struct same_t<T, T>: std::true_type{};

// remove_cv extract type from const volatile

template<typename T>
struct remove_cv{
    using type = T;
};

template<typename T>
struct remove_cv<const T>{
    using type = T;
};

template<typename T>
struct remove_cv<volatile T>{
    using type = T;
};
template<typename T>
struct remove_cv<const volatile T>{
    using type = T;
};

// conditional <bool, T, U> gives T if bool=True, else U
template<bool, typename T, typename F>
struct conditional{
    using type = T;
};

template<typename T, typename F>
struct conditional<false, T, F>{
    using type = F;
};

// remove_reference <T> remove & && from a type
template<typename T>
struct remove_reference{
    using type = T;
};

template<typename T>
struct remove_reference<T&>{
    using type = T;
};

template<typename T>
struct remove_reference<T&&>{
    using type = T;
};

// remove_array <T> remove [] from a type
template<typename T>
struct remove_array{
    using type = T;
};

template<typename T>
struct remove_array<T[]>{
    using type = T;
};

template<typename T, int N>
struct remove_array<T[N]>{
    using type = T;
};

// is_array determine if a type is an array
template<typename T>
struct is_array: std::false_type{};

template<typename T>
struct is_array<T[]>: std::true_type{};

template<typename T, int N>
struct is_array<T[N]>: std::true_type{};

// is_function determine if a type is a function, TODO see cppreference for full support
template<typename T>
struct is_function: std::false_type{};

template<typename Ret, typename... Args>
struct is_function<Ret(Args...)>: std::true_type{};

// ...

// decay <T> remove array, reference, pointer, cv from a type
template<typename T>
struct decay{
private:
    using U = typename remove_reference<T>::type;
public:
    using type = typename conditional<
        is_array<U>::value,
        typename remove_array<U>::type*,
        typename conditional<
            std::is_function<U>::value,
            typename std::add_pointer<U>::type,
            typename remove_cv<U>::type
        >::type
    >::type;
};

// enable_if<bool, T> contains type=T if bool=true
template<bool, typename T=void>
struct enable_if{};

template<typename T>
struct enable_if<true, T>{
    using type = T;
};

// result_of<T> contains the type of T's return type
template<typename T>
struct result_of{};

template<typename F, typename... Args>
struct result_of<F(Args...)>{
    using type = decltype(std::declval<F>()(std::declval<Args>()...));
};

}; // namespace sql

#endif