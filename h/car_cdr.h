#pragma once
#include "lists.h"


/** \brief helper function of car (car and cdr only accepts quoted lists as arguments) */
template <typename A, typename... Args>
auto constexpr car_inner(A,Args...){
    return A{};
}

/** \brief lisp like car (car and cdr only accepts quoted lists as arguments) */
template <template<class> class T, typename... Args>
auto constexpr car(quote<T<Args...>>){
    static_assert(sizeof...(Args) > 0,"car on empty list");
    return car_inner(Args{}...);
}

/** \brief helper function of cdr (car and cdr only accepts quoted lists as arguments) */
template <template<class> class T, typename A, typename... Args>
auto constexpr cdr_inner(A,Args...){
    return T<Args...>{};
}
/** \brief lisp like cdr (car and cdr only accepts quoted lists as arguments) */
template <template<class> class T, typename... Args>
auto constexpr cdr(quote<T<Args...>>){
    static_assert(sizeof...(Args) > 0,"cdr on empty list");
    return cdr_inner<T>(Args{}...);
}
/** \brief lisp like cadr (car and cdr only accepts quoted lists as arguments) */
template <template<class> class T, typename... Args>
auto constexpr cadr(quote<T<Args...>>){
    static_assert(sizeof...(Args) > 0,"cadr on empty list");
    using the_cdr = decltype(cdr(quote<T<Args...>>{}));
    return car(make_quote(the_cdr{}));
}

/** \brief helper function of IRcar (accepts anything)*/
template <typename A, typename... Args>
auto constexpr IRcar_inner(A,Args...){
    return A{};
}

/** \brief lisp like car (accepts anything)*/
template <template<class> class T, typename... Args>
auto constexpr IRcar(T<Args...>){
    static_assert(sizeof...(Args) > 0,"IRcar on empty list");
    return IRcar_inner(Args{}...);
}

/** \brief helper function of IRcdr (accepts anything)*/
template <template<class> class T, typename A, typename... Args>
auto constexpr IRcdr_inner(A,Args...){
    return T<Args...>{};
}

/** \brief lisp like car (accepts anything)*/
template <template<class> class T, typename... Args>
auto constexpr IRcdr(T<Args...>){
    static_assert(sizeof...(Args) > 0,"IRcdr on empty list");
    return IRcdr_inner<T>(Args{}...);
}

/** \brief lisp like cadr (accepts anything)*/
template <template<class> class T, typename... Args>
auto constexpr IRcadr(T<Args...>){
    static_assert(sizeof...(Args) > 0,"IRcadr on empty list");
    using the_cdr = decltype(IRcdr(T<Args...>{}));
    return IRcar(the_cdr{});
}

/** \brief lisp like cddr (accepts anything)*/
template <template<class> class T, typename... Args>
auto constexpr IRcddr(T<Args...>){
    static_assert(sizeof...(Args) > 0,"IRcddr on empty list");
    using the_cdr = decltype(IRcdr(T<Args...>{}));
    return IRcdr(the_cdr{});
}

/** \brief lisp like caddr (accepts anything)*/
template <template<class> class T, typename... Args>
auto constexpr IRcaddr(T<Args...>){
    static_assert(sizeof...(Args) > 0,"IRcaddr on empty list");
    using the_cdr = decltype(IRcdr(T<Args...>{}));
    return IRcar(IRcdr(the_cdr{}));
}

/** \brief lisp like cadddr (accepts anything)*/
template <template<class> class T, typename... Args>
auto constexpr IRcadddr(T<Args...>){
    static_assert(sizeof...(Args) > 0,"IRcadddr on empty list");
    using the_cdr = decltype(IRcdr(T<Args...>{}));
    return IRcar(IRcdr(IRcdr(the_cdr{})));
}