#pragma once
#include "lists.h"

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

template <typename A, typename... Rest>
auto constexpr car(table<A,Rest...>){
    return A{};
}
template <typename A, typename... Rest>
auto constexpr cdr(table<A,Rest...>){
    return table<Rest...>{};
}

/*
(define (foldl f acc xs)
  (if (null? xs)
      acc
      (foldl f
             (f acc (car xs))
             (cdr xs))))
template <typename F, typename A, typename... Rest>
auto constexpr foldl(){}

template <typename F, typename A, typename... Rest>
auto constexpr foldl(F,table<A,Rest...>){
    if constexpr (sizeof...(Rest) > 0){
        return foldl(F{},
              F<A,car(Rest{}...)>{},
              cdr(Rest{}...));
    } else {
        return A{};
    }
}
*/

// beginning of a new layer
template <typename A, typename ...Rest, typename ...Chars>
auto constexpr extract_symbols(list<token_list<A,Rest...>>) {
	if constexpr (sizeof...(Rest) > 0) {
		if constexpr (is_char_list(A{})) {
            using second = decltype(extract_symbols(Rest{}...));
            return make_token_list(A{},second{});
        }
	} else {
        return A{};
    }
};

// multiple char_l and non char_l overload
template <typename A, typename ...Rest, typename ...Chars>
auto constexpr extract_symbols(A,Rest...) {
	if constexpr (sizeof...(Rest) > 0) {
		if constexpr (is_char_list(A{})) {
            using second = decltype(extract_symbols(Rest{}...));
            return make_token_list(A{},second{});
        } else {
            return extract_symbols(Rest{}...);
        }
	} else {
        return A{};
    }
};


// token list wrapper
template < typename A, typename ...Rest >
auto constexpr collect_entries(token_list< A, Rest... >) {
	if constexpr (sizeof...(Rest) >= 0) {
		return extract_symbols(A{});
	} else {
		return;
	}
}