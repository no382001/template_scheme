#pragma once
#include <string_view>
#include <type_traits>
#include <cmath>
#include "atoms.h"
#include "utils.h"
#include "lists.h"
#include "lambda.h"


template <typename... Args>
auto constexpr is_token_list_list(token_list<list<Args...>>){
	return true;
}
template <typename T>
auto constexpr is_token_list_list(T){
	return false;
}



template <typename Lambda>
constexpr auto tokenizer(Lambda str_lambda) {
	using list_of_tokens = decltype(tokenize<Lambda,0>(str_lambda));

	// plain lambda expressions like "((lambda (x y) (+ x y)) 2 1)" will return in a token_list when resolved,
	// but the parser only takes token_list<list<...>>, if this is the only parsed expression it needs to be wrapped in tokenlist<list<...>>
	if constexpr (!is_token_list_list(list_of_tokens{})){
		return make_token_list(make_list(list_of_tokens{})); //wrap in list and token_list so the parser is happy
	} else {
		return list_of_tokens{};
	}
}

struct i_am_l {};

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
			using l = decltype(tokenize_list< Lambda, Index>(str_lambda));
			using second = decltype(tokenize<Lambda, end_of_list + 1>(str_lambda));

			if constexpr (is_empty_list(l{})){ // drop empty expressions, define handling
				return make_token_list(second{});
			} else {
				return make_token_list(l{}, second{});
			} 

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
				
				if constexpr (is_same_type<_lambda,char_list>){// extract args and expressions to the outer layer
					using second = decltype(tokenize< Lambda, end_of_char_list >(str_lambda));
					return make_lambda(second{});
				} else if constexpr (is_same_type<_define,char_list>){// if its a define expression, ignore node, another tokenizing process deals with that
					return make_token_list();
				} else {
					using second = decltype(tokenize< Lambda, end_of_char_list >(str_lambda));
					return make_token_list(char_list{}, second{});
				}		
				
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