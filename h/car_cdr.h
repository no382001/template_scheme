#pragma once
#include "lists.h"

// car and cdr only accepts quoted lists as arguments

template <typename A, typename... Args>
auto constexpr car_inner(A,Args...){
    return A{};
}

template <template<class> class T, typename... Args>
auto constexpr car(quote<T<Args...>>){
    static_assert(sizeof...(Args) > 0,"car on empty list");
    return car_inner(Args{}...);
}

template <template<class> class T, typename A, typename... Args>
auto constexpr cdr_inner(A,Args...){
    return T<Args...>{};
}

template <template<class> class T, typename... Args>
auto constexpr cdr(quote<T<Args...>>){
    static_assert(sizeof...(Args) > 0,"cdr on empty list");
    return cdr_inner<T>(Args{}...);
}

template <template<class> class T, typename... Args>
auto constexpr cadr(quote<T<Args...>>){
    static_assert(sizeof...(Args) > 0,"cadr on empty list");
    using the_cdr = decltype(cdr(quote<T<Args...>>{}));
    return car(make_quote(the_cdr{}));
}