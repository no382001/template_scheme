#pragma once
#include "lists.h"
#include "atoms.h"
#include "car_crd.h"
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

IS_X_LIST(token_list);
IS_X_LIST(list);

template <typename A>
auto constexpr extract_symbols(A) {
    if constexpr (!is_same_type<A,list<>> || !is_same_type<A,token_list<>> || !is_same_type<A,c_list<>>){ //len
		if constexpr (is_token_list(car(A{}))) {
            using curr = decltype(extract_symbols(car(A{})));
            using second = decltype(extract_symbols((cdr(curr{}))));
            return make_list(curr{},second{});
        } else if constexpr (is_list(car(A{}))) {
            using curr = decltype(extract_symbols(car(A{})));
            using second = decltype(extract_symbols(cdr(curr{})));
            return make_list(curr{},second{});
        } else if constexpr (is_c_list(car(A{}))) {
            using second = decltype(extract_symbols(cdr(A{})));
            return make_list(A{},second{});
        } else if constexpr (is_token_list(cdr(A{}))){
            return extract_symbols(cdr(A{}));
        } else {
            return make_list();
        }
    } else {
        return make_list();
    }
};