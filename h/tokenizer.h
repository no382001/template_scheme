#pragma once
#include <string_view>
#include <type_traits>
#include <cmath>
#include "atoms.h"
#include "utils.h"
#include "lists.h"
#include "deduce.h"
#include "car_cdr.h"

template <template <class> typename A, typename... Rest>
bool constexpr is_empty_list(A<Rest...>){
	if constexpr (sizeof...(Rest) > 0) {
        return false;
    } else {
        return true;
    }
}

template < int Index, typename Lambda >
constexpr auto find_first_non_integer(Lambda lambda) {
	constexpr auto str = lambda();
	using type = decltype(deduce_token_type< str[Index] >());
	if constexpr (!is_integer_v<type>) {
		return Index;
	} else {
		return find_first_non_integer< Index + 1 >(lambda);
	}
}

template < int Index, typename Lambda >
constexpr auto find_first_non_c(Lambda lambda) {
	constexpr auto str = lambda();
	using type = decltype(deduce_token_type< str[Index] >());
	if constexpr (!is_char_v<type>) {
		return Index;
	} else {
		return find_first_non_c< Index + 1 >(lambda);
	}
}

// returns the index of the end of the list, layer safe
template < int Index, int layer = 0, typename Lambda>
constexpr auto find_end_of_list(Lambda lambda){
	constexpr auto str = lambda();
	using type = decltype(deduce_token_type< str[Index] >());
	if constexpr (is_same_type<type,list_start>){
		return find_end_of_list<Index+1, layer+1>(lambda);
	} else if constexpr (is_same_type<type,list_end>) {
		if constexpr (layer - 1 <= 0){
			return Index;
		} else {
			return find_end_of_list< Index + 1 , layer - 1>(lambda);
		}
	} else {
		return find_end_of_list< Index + 1 , layer>(lambda);
	}
}
  
// only c_ is a character, ints are currently not even if they are in the same stream as the char list
template <typename Lambda, size_t Index = 0, size_t end_of_char_list>
constexpr auto tokenize_char_list(Lambda str_lambda) {
	constexpr auto str = str_lambda();
	if constexpr (Index < end_of_char_list) {
		using curr = decltype(deduce_token_type< str[Index] >());
		using second = decltype(tokenize_char_list< Lambda, Index + 1, end_of_char_list >(str_lambda));
		return make_c_list(curr{}, second{});
	} else {
		return make_c_list();
	}
}

template < int Start, int End, typename Lambda >
constexpr auto make_integer(Lambda str_lambda) {
	constexpr auto str = str_lambda();
	if constexpr (Start < End) {
		constexpr auto value = str[Start] - '0';
		return integer< value >::merge(make_integer< Start + 1, End >(str_lambda));
	} else {
		return non_integer{};
	}
}

// list parsing works
// quote parsing works
#include <string_view>
#define constexpr_string(...) ([]() constexpr -> std::string_view { return __VA_ARGS__; })

// tokenize does not handle (define ...) type expressions, if they are found, the node is terminated, see table.h
template <typename Lambda, size_t Index = 0>
constexpr auto tokenize(Lambda str_lambda) {
	constexpr auto str = str_lambda();

	if constexpr (Index < str.size()) {
		// deduce the type of the current char
		using curr = decltype(deduce_token_type< str[Index] >());

		if constexpr (is_same_type<curr, list_start>) {
			constexpr auto end_of_list = find_end_of_list< Index >(str_lambda);
			// tokenize the contents of the list and return it in a wrapper
			using l = decltype(make_list(tokenize< Lambda, Index + 1>(str_lambda)));
			using second = decltype(tokenize<Lambda, end_of_list + 1>(str_lambda));

			return make_token_list(l{}, second{});

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
			constexpr auto end_of_char_list = find_first_non_c< Index >(str_lambda);
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


auto str = constexpr_string("(+ 11 11)");
using tokens = decltype(tokenize(str));

