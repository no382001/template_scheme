#pragma once
#include "if.h"


#define NUM_OP(name,sign)															\
template<int A, typename ...Rest>													\
auto constexpr e_##name(integer<A>, Rest... r) {									\
	if constexpr (sizeof...(Rest) > 0)												\
		return A sign e_##name(r...);												\
	else{ return A; }																\
}																					\
template< typename ...Args,typename ...Rest>										\
auto constexpr e_##name(list<Args...> l, Rest...) { /* nested expression overload*/ \
	auto constexpr x = handle_prefix(l);											\
	return e_##name(integer<x>{}, Rest{}...);										\
}

NUM_OP(add,+);
NUM_OP(sub,-);
NUM_OP(mul,*);
NUM_OP(div,/);
NUM_OP(equal,==);
NUM_OP(less,<);
NUM_OP(more,>);


template <typename A, typename ...Rest>
auto constexpr  handle_prefix(list<token_list<A,Rest...>>) {
	if constexpr (sizeof...(Rest) > 0) {
		if constexpr (is_same_type<A, plus>) { // if the prefix is valid
			return e_add(Rest{}...); // pass the rest of the parameters to the evaluator
		} else if constexpr (is_same_type<A, minus>) {
			return e_sub(Rest{}...);
		} else if constexpr (is_same_type<A, mul>) {
			return e_mul(Rest{}...);
		} else if constexpr (is_same_type<A, div_>) {
			return e_div(Rest{}...);
		} else if constexpr (is_same_type<A, equal>) {
			return e_equal(Rest{}...);
		} else if constexpr (is_same_type<A, less>) {
			return e_less(Rest{}...);
		} else if constexpr (is_same_type<A, more>) {
			return e_more(Rest{}...);
		} else if constexpr (is_same_type<A,_if>){
			return e_if(Rest{}...);
		}
	} else if constexpr (is_integer_v<A>){
		return A{}.get_value();
	}
};

template <typename A, typename ...Rest>
auto constexpr  handle_prefix(A,Rest...) {
	if constexpr (sizeof...(Rest) > 0) {
		if constexpr (is_same_type<A, plus>) { // if the prefix is valid
			return e_add(Rest{}...); // pass the rest of the parameters to the evaluator
		} else if constexpr (is_same_type<A, minus>) {
			return e_sub(Rest{}...);
		} else if constexpr (is_same_type<A, mul>) {
			return e_mul(Rest{}...);
		} else if constexpr (is_same_type<A, div_>) {
			return e_div(Rest{}...);
		}
	} else if constexpr (is_integer_v<A>){
		return A{}.get_value();
	}
};