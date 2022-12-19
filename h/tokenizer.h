#pragma once
#include <string_view>
#include <type_traits>
#include <cmath>
#include "atoms.h"
#include "utils.h"
#include "lists.h"
#include "lambda.h"

// tokenize does not handle (define ...) type expressions, if they are found, the node is terminated
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

			if constexpr (is_indicator(car(l{}))){ //indicating the return of the nested layers within nested lambda
					using arguments = decltype(car(l{}));
					using expression = decltype(car(cdr(car(l{}))));
					using param = decltype(remove_outer(cdr(cdr(car(l{})))).append(second{}));

					//using arg_x_parameter_table = decltype(map_pair_l(arguments{},param{}));
					//using result = decltype(substitute(arg_x_parameter_table{},expression{}));

					return make_table(arguments{},expression{},param{});
			} else if constexpr (is_lambda(car(l{}))){ // remove the list wrapper and check if its a lambda
				
				using stripped_l = decltype(car(car(l{})));
				using arguments = decltype(car(car(stripped_l{})));
				using expression = decltype(car(car(cdr(stripped_l{}))));
				using parameters = decltype(second{});

				if constexpr (is_table(car(cdr(stripped_l{})))){ //if returned from no param node
					using table = decltype(car(cdr(stripped_l{})));
					
					// append args to curr args
					using appended = decltype(make_token_list(arguments{}).append(car(table{})));
					using expression = decltype(cdr(table{}));

					return make_indicator(appended{},expression{},parameters{}); // pass things up for handling

				} else if constexpr (is_empty_list(second{})){
					return make_table(arguments{},expression{}); // if nested lambda, ergo no param, return the expression and the args
				}


			} else if constexpr (is_empty_list(l{})){ // drop empty expressions, define handling
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