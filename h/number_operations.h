#pragma once
template<int A, typename ...Rest>
auto constexpr e_add(integer<A>, Rest... r) {
	if constexpr (sizeof...(Rest) > 0)
		return A + e_add(r...);
	else
		return A;
}
template< typename ...Args,typename ...Rest>
auto constexpr e_add(list<Args...> l, Rest...) { // nested expression overload
	auto constexpr x = handle_prefix(l);
	return e_add(integer<x>{}, Rest{}...);
}


template<int A, typename ...Rest>
auto constexpr e_sub(integer<A>, Rest... r) {
	if constexpr (sizeof...(Rest) > 0)
		return A - e_sub(r...);
	else
		return A;
}
template< typename ...Args, typename ...Rest>
auto constexpr e_sub(list<Args...> l, Rest...) {
	auto constexpr x = handle_prefix(l);
	return e_sub(integer<x>{}, Rest{}...);
}

template<int A, typename ...Rest>
auto constexpr e_mul(integer<A>, Rest... r) {
	if constexpr (sizeof...(Rest) > 0)
		return A * e_mul(r...);
	else
		return A;
}

//FIXTHIS: something is wrong here but i should probably rewrite this in clisp first
template< typename ...Args, typename ...Rest>
auto constexpr e_mul(list<Args...> l, Rest...) {
	auto constexpr x = handle_prefix(l);
	return e_mul(integer<x>{}, Rest{}...);
}

template<int A, typename ...Rest>
auto constexpr e_div(integer<A>, Rest... r) {
	if constexpr (sizeof...(Rest) > 0)
		return A / e_div(r...);
	else
		return A;
}
template< typename ...Args, typename ...Rest>
auto constexpr e_div(list<Args...> l, Rest...) {
	auto constexpr x = handle_prefix(l);
	return e_div(integer<x>{}, Rest{}...);
}

template <typename A, typename ...Rest>
auto constexpr  handle_prefix(list<token_list<A,Rest...>>) {
	if constexpr (sizeof...(Rest) > 0) {
		if constexpr (is_same_type<A, plus>) { // if the prefix is valid
			return e_add(Rest{}...); // pass the rest of the parameters to the evaluator
		}
		else if constexpr (is_same_type<A, minus>) {
			return e_sub(Rest{}...);
		}
		else if constexpr (is_same_type<A, mul>) {
			return e_mul(Rest{}...);
		}
		else if constexpr (is_same_type<A, div_>) {
			return e_div(Rest{}...);
		}
	}
};


// (+ 3 (+ 3 (+ 2 2) 2) 2) the last 2 is completely ignored
// 	FIXTHIS:				the 2nd from theright gets a dulpicate token
