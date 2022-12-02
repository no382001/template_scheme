#pragma once
#include <string_view>
#include <type_traits>
#include <cmath>
#include "atoms.h"
#include "utils.h"
#include "lists.h"

template <typename Lambda, size_t Index = 0>
//pass a stringview return type lambda that passes the arguments with __VA_ARGS__
constexpr auto tokenize(Lambda str_lambda) {
	constexpr auto str = str_lambda();

	if constexpr (Index < str.size()) {
		// deduce the type of the current char
		using curr = decltype(deduce_token_type< str[Index] >());

		if constexpr (is_same_type<curr, list_start>) {
			constexpr auto end_of_list = find_end_of_list< Index >(str_lambda);
			// tokenize the contents of the list and return it in a wrapper
			using list = decltype(tokenize_list< Lambda, Index, end_of_list>(str_lambda));
			using second = decltype(tokenize<Lambda, end_of_list + 1>(str_lambda));
			return make_token_list(list{}, second{});
		} else if constexpr (is_same_type<curr, list_end>) {
			// base case for tokenize_list
			return make_token_list();
		} else if constexpr (is_integer_v<curr>) {
			// make a multi character integer if possible
			constexpr auto first_non_integer = find_first_non_integer< Index + 1 >(str_lambda);
			using integer_type = decltype(make_integer< Index, first_non_integer >(str_lambda));
			using second = decltype(tokenize< Lambda, first_non_integer >(str_lambda));
			return make_token_list(integer_type{}, second{});
		} else if constexpr (is_char_v<curr>) {
			//if something starts with a character, find the next non character
			constexpr auto end_of_char_list = find_end_of_char_list< Index >(str_lambda);
			// tokenize the contents of the list and return it in a wrapper
			if constexpr (end_of_char_list > 0) {
				using char_list = decltype(tokenize_char_list< Lambda, Index, end_of_char_list >(str_lambda));
				using second = decltype(tokenize< Lambda, end_of_char_list >(str_lambda));
				return make_token_list(char_list{}, second{});
			} else {
				using second = decltype(tokenize< Lambda, Index + 1 >(str_lambda));
				return make_token_list(curr{}, second{});
			}
		} else if constexpr (is_same_type<curr,whitespace>) {
			// if its not a specially handled token
			return tokenize< Lambda, Index + 1 >(str_lambda);
		} else {
			// if its not a specially handled token
			using next = decltype(tokenize< Lambda, Index + 1 >(str_lambda));
			return make_token_list(curr{}, next{});
		}
	} else {
		return make_token_list();
	}
}