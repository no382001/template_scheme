
#include <string_view>
#include <type_traits>
#include <cmath>
#include "atoms.h"
#include "utils.h"
#include "lists.h"
#include "pretty_print.h"


template <typename Lambda, size_t Index = 0>
//pass a stringview return type lambda that passes the arguments with __VA_ARGS__
constexpr auto tokenize(Lambda str_lambda)
{
	constexpr auto str = str_lambda();

	if constexpr (Index < str.size()) {
		// deduce the type of the current char
		using curr = decltype(deduce_token_type< str[Index] >());

		if constexpr (is_same_type<curr, list_start>) {
			// if something starts with a character, find the next non character
			constexpr auto end_of_list = find_end_of_list< Index >(str_lambda);
			// tokenize the contents of the list and return it in a wrapper
			using list = decltype(tokenize_list< Lambda, Index, end_of_list>(str_lambda));
			using second = decltype(tokenize<Lambda, end_of_list + 1>(str_lambda));
			return make_token_list(list{}, second{});

		}
		else if constexpr (is_same_type<curr, list_end>) {
			// base case for tokenize_list
			return make_token_list();
		}
		else if constexpr (is_integer_v<curr>) {
			// make a multi character integer
			constexpr auto first_non_integer = find_first_non_integer< Index + 1 >(str_lambda);
			using integer_type = decltype(make_integer< Index, first_non_integer >(str_lambda));
			using second = decltype(tokenize< Lambda, first_non_integer >(str_lambda));
			return make_token_list(integer_type{}, second{});
		}
		else if constexpr (is_char_v<curr>) {
			//if something starts with a character, find the next non character
			constexpr auto end_of_char_list = find_end_of_char_list< Index >(str_lambda);
			// tokenize the contents of the list and return it in a wrapper
			if constexpr (end_of_char_list > 0) {
				using char_list = decltype(tokenize_char_list< Lambda, Index, end_of_char_list >(str_lambda));
				using second = decltype(tokenize< Lambda, end_of_char_list >(str_lambda));
				return make_token_list(char_list{}, second{});
			}
			else {
				using second = decltype(tokenize< Lambda, Index + 1 >(str_lambda));
				return make_token_list(curr{}, second{});
			}
		}
		else if constexpr (is_same_type<curr,whitespace>) {
			// if its not a specially handled token
			return tokenize< Lambda, Index + 1 >(str_lambda);
		}
		else {
			// if its not a specially handled token
			using next = decltype(tokenize< Lambda, Index + 1 >(str_lambda));
			return make_token_list(curr{}, next{});
		}
	}
	else {
		return make_token_list();
	}
}
// ---------------------------------------- number ops
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
// ---------------------------------------- number ops

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

// the outer layer
template < typename A, typename ...Rest >
auto constexpr parse(token_list< A, Rest... >) {
	// extract the first argument of token_list
	if constexpr (sizeof...(Rest) >= 0) {
		return handle_prefix(A{});
		//return define_atom(constexpr_string("abcd")) == A;
	}
	else {
		return 0;
	}
}




int main()
{

	auto x = constexpr_string("(+ 3 (* 3 (+ 2 2) 2))");
	//auto x = constexpr_string("( abcd )");
	
	using tokens = decltype(tokenize(x));
	auto constexpr res = parse(tokens{});
		
	pretty_print(typeid(tokens).name());
	std::cout << ";; " << res;
}