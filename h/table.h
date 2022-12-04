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

template < typename ...Types >
struct table {
    LIST_BODY(table);

    template < typename candidate>
    static constexpr auto search() {
        return table_search<candidate,Types...>;
    }
};
MAKE_LIST_FUNCTIONS(table);