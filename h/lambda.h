#pragma once
#include "atoms.h"
#include "lists.h"
#include "utils.h"
#include "tokenizer.h"


#define _lambda c_list<c_<108>, c_<97>, c_<109>, c_<98>, c_<100>, c_<97>>


LIST(lambda);
IS_X_LIST(lambda);
LIST(pair_l);


LIST(indicator);
IS_X_LIST(indicator);

template <template <class,class> typename One, typename A, typename... Args, template <class,class> typename Two, typename B, typename... Brgs>
auto constexpr map_pair_l(One<A,Args...>,Two<B,Brgs...>){
		using p = decltype(make_pair_l(A{},B{}));
	if constexpr (sizeof...(Args) == 0) {
		return make_token_list(p{});
	} else {
		using second = decltype(map_pair_l(One<Args...>{},Two<Brgs...>{}));
		return make_token_list(p{},second{});
	}
}


// search for the key of a pair list, get the value back
// - returns void when not found
template <typename candidate, typename... Args>
auto constexpr search_pair_l(candidate,Args...){
    if constexpr (sizeof...(Args) == 0) {
		return make_token_list();
    } else {
		using curr_t = decltype(car(Args{}...));
		using to_compare = decltype(car(car(Args{}...)));
		if constexpr (is_same_type<candidate,to_compare>){
			return cdr(car(Args{}...));
		} else if constexpr (!is_same_type<token_list<>,curr_t>){
			return search_pair_l(candidate{},cdr(Args{}...));
		}
    }
}

template < typename table,typename... Args>
auto constexpr substitute(table,Args...){
    if constexpr (is_same_type<token_list<>,Args...>) {
		return make_token_list();
    } else {
		using curr = decltype(car(Args{}...));
		if constexpr (is_c_list(curr{})){
            using value = decltype(search_pair_l(curr{},table{}));
            using second = decltype(substitute(table{},cdr(Args{}...)));
            return make_token_list(car(value{}),second{});
        } else {
            using second = decltype(substitute(table{},cdr(Args{}...)));
			return make_token_list(curr{},second{});
		}
    }
}

template <typename Lambda, size_t Index>
constexpr auto tokenize_list_for_lambda(Lambda str_lambda) {
	using curr = decltype(tokenize< Lambda, Index + 1 >(str_lambda));
	return make_token_list(curr{});
}

template <typename table, template <class,class> typename T, typename A, typename... Args>
auto constexpr subst_helper(table, T<A,Args...>){
	return substitute(table{},A{},Args{}...);
}