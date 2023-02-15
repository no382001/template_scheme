#pragma once
#include "lists.h"

template <typename A, typename... Args>
auto constexpr car_inner(A,Args...){
    return A{};
}

template <template<class> class T, typename... Args>
auto constexpr car(quote<T<Args...>>){
    static_assert(sizeof...(Args) > 0,"empty list");
    return car_inner(Args{}...);
}

template <template<class> class T, typename A, typename... Args>
auto constexpr cdr_inner(A,Args...){
    return T<Args...>{};
}

template <template<class> class T, typename... Args>
auto constexpr cdr(quote<T<Args...>>){
    static_assert(sizeof...(Args) > 0,"empty list");
    return cdr_inner<T>(Args{}...);
}