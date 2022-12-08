#pragma once
#include "lists.h"
#include "atoms.h"

// after tokenization a resolve_symbols function should be run
// that rearranges the tokens ina  way that symboos are resolved and the parser can work in peace

// entry datatype: symbol, //layer, //argnum, expression
template <typename symbol,/*int layer, int argnum,*/ typename expression>
struct table_entry {
    static constexpr auto get_symbol() {return symbol{};}
};

//LIST(table);

// positive search
template < typename candidate, typename A, typename... Rest>
constexpr auto table_search(candidate c, A a, Rest ...rest) {
    if constexpr (sizeof...(Rest) > 0){
        using curr = A::get_symbol(); 
        if constexpr (is_same_type<curr,candidate>) {
            return A{};
        } else {
            return table_search(c,rest...);
        }
    } else {
        return A{};
    }
}

LIST(table);

template <typename A, typename... Args>
auto constexpr car_inner(A,Args...){
    return A{};
}

template <template<class> class T, typename... Args>
auto constexpr car(T<Args...>){
    if constexpr (sizeof...(Args) > 0) {
        return car_inner(Args{}...);
    } else {
        return T{};
    }
}

template <template<class> class T, typename A, typename... Args>
auto constexpr cdr_inner(A,Args...){
    return T<Args...>{};
}

template <template<class> class T, typename... Args>
auto constexpr cdr(T<Args...>){
    if constexpr (sizeof...(Args) > 0) {
        return cdr_inner<T>(Args{}...);
    } else {
        return T{};
    }
}

IS_X_LIST(token_list);
IS_X_LIST(list);

template <typename A>
auto constexpr extract_symbols(A) {
    using curr = A;
    if constexpr (!is_same_type<curr,list<>> || !is_same_type<curr,token_list<>> || !is_same_type<curr,c_list<>>){ //len
		if constexpr (is_token_list(car(curr{}))) {
            using curr = decltype(extract_symbols(car(curr{})));
            using second = decltype(extract_symbols((cdr(curr{}))));
            return make_list(curr{},second{});
        } else if constexpr (is_list(car(curr{}))) {
            using curr = decltype(extract_symbols(car(curr{})));
            using second = decltype(extract_symbols(cdr(curr{})));
            return make_list(curr{},second{});
        } else if constexpr (is_c_list(car(curr{}))) {
            using second = decltype(extract_symbols(cdr(curr{})));
            return make_list(curr{},second{});
        } else {
            return extract_symbols(cdr(curr{}));
        }
    } else {
        return make_list();
    }
};