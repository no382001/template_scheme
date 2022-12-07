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

#define CAR_CDR(name)\
template <typename A, typename... Rest>     \
auto constexpr car(name<A,Rest...>){        \
    return A{};                             \
}                                           \
template <typename A, typename... Rest>     \
auto constexpr cdr(name<A,Rest...>){        \
    return name<Rest...>{};                 \
}

CAR_CDR(table);
CAR_CDR(token_list);
CAR_CDR(list);

IS_X_LIST(token_list);
IS_X_LIST(list);

// beginning of a new layer
template <typename A, typename ...Rest>
auto constexpr extract_symbols(list<A,Rest...>) {
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


// token list wrapper, there is really no need for this i could just make typecheck functions to reduce the codebase
template < typename A, typename ...Rest >
auto constexpr collect_entries(token_list< A, Rest... >) {
	if constexpr (sizeof...(Rest) >= 0) {
		return extract_symbols(A{});
	} else {
		return make_token_list();
	}
}
