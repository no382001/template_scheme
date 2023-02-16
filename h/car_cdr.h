#pragma once
#include "lists.h"

// car and cdr only accepts quoted lists as arguments
//template <typename T>
//concept car_cdr_operable_list = is_same_list_t<T,table_entry> || is_quoted(T{}); // howtf do i enforce this in the template argument? ohwell, lets just use list instead of table ernty

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


template <typename A, typename... Args>
auto constexpr IRcar_inner(A,Args...){
    return A{};
}

template <template<class> class T, typename... Args>
auto constexpr IRcar(T<Args...>){
    static_assert(sizeof...(Args) > 0,"IRcar on empty list");
    return IRcar_inner(Args{}...);
}

template <template<class> class T, typename A, typename... Args>
auto constexpr IRcdr_inner(A,Args...){
    return T<Args...>{};
}

template <template<class> class T, typename... Args>
auto constexpr IRcdr(T<Args...>){
    static_assert(sizeof...(Args) > 0,"IRcdr on empty list");
    return IRcdr_inner<T>(Args{}...);
}