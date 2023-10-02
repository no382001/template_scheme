#pragma once
#include "lists.h"


struct not_found {};

template <int N,int Acc,typename A,typename... Args>
auto constexpr impl_return_nth_element(list<A,Args...>){
    if constexpr (N == Acc){
        return A{};
    } else if constexpr (sizeof...(Args) == 0){
       return not_found{};
    } else {
        return impl_return_nth_element<N,Acc +1>(list<Args...>{});
    }
}


template <int N,typename... Args>
auto constexpr return_nth_element(list<Args...>){
    static_assert(sizeof...(Args) > 0,"return_nth_element on empty list!");
    return impl_return_nth_element<N,1>(list<Args...>{});
}

using second_retn = decltype(return_nth_element<2>(list<void,int,char>{}));
using second_retn2 = decltype(return_nth_element<2>(list<void>{}));
//using second_retn3 = decltype(return_nth_element<2>(list<>{}));

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