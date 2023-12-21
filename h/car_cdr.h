#pragma once
#include "lists.h"

// -----------------------------------------------------------------------
// for IR use only, accepts any type

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

template <template<class> class T, typename... Args>
auto constexpr IRcadr(T<Args...>){
    static_assert(sizeof...(Args) > 0,"IRcadr on empty list");
    using the_cdr = decltype(IRcdr(T<Args...>{}));
    return IRcar(the_cdr{});
}

template <template<class> class T, typename... Args>
auto constexpr IRcddr(T<Args...>){
    static_assert(sizeof...(Args) > 0,"IRcddr on empty list");
    using the_cdr = decltype(IRcdr(T<Args...>{}));
    return IRcdr(the_cdr{});
}

template <template<class> class T, typename... Args>
auto constexpr IRcaddr(T<Args...>){
    static_assert(sizeof...(Args) > 0,"IRcaddr on empty list");
    using the_cdr = decltype(IRcdr(T<Args...>{}));
    return IRcar(IRcdr(the_cdr{}));
}

template <template<class> class T, typename... Args>
auto constexpr IRcadddr(T<Args...>){
    static_assert(sizeof...(Args) > 0,"IRcadddr on empty list");
    using the_cdr = decltype(IRcdr(T<Args...>{}));
    return IRcar(IRcdr(IRcdr(the_cdr{})));
}