#pragma once
#include <string_view>
#include <type_traits>
#include <cmath>
#include "atoms.h"
#include "utils.h"
#include "lists.h"
#include "deduce.h"
#include "car_cdr.h"
#include "pretty_print.h"


// REFACTOR
// its a templated int so refactor later
template < int Index, typename Lambda>
constexpr auto find_first_non_integer(Lambda lambda) {
	constexpr auto str = lambda();
	using type = decltype(deduce_token_type< str[Index] >());
	if constexpr (!is_integer_v<type>) {
		return Index;
	} else {
		return find_first_non_integer< Index + 1 >(lambda);
	}
}

// REFACTOR
template < int Index, typename Lambda >
constexpr auto find_first_non_c(Lambda lambda) {
	constexpr auto str = lambda();
	using type = decltype(deduce_token_type< str[Index] >());
	
	// extended for `-` in procedure name, as it is the subtraction sybmol on its own
	// tokenize_char_list should handle it being not appended as type `subtraction` but char `-`
	if constexpr (!is_char_v<type> && !is_same_type<subtraction,type>) {
		return Index;
	} else {
		return find_first_non_c< Index + 1 >(lambda);
	}
}

// REFACTOR
template < int Index, typename Lambda >
constexpr auto find_first_newline(Lambda lambda) {
	constexpr auto str = lambda();
	using type = decltype(deduce_token_type< str[Index] >());
	if constexpr (is_same_type<type,whitespace<'\n'>>) {
		return Index;
	} else {
		return find_first_newline< Index + 1 >(lambda);
	}
}

// REFACTOR, this is basically find_first_occurence
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



// only c_ is a character, ints are currently not, even if they are in the same stream as the char list
template <typename Lambda, size_t Index = 0, size_t end_of_char_list>
constexpr auto tokenize_char_list(Lambda str_lambda) {
	constexpr auto str = str_lambda();
	if constexpr (Index < end_of_char_list) {
		using curr = decltype(deduce_token_type< str[Index] >());

		using second = decltype(tokenize_char_list< Lambda, Index + 1, end_of_char_list >(str_lambda));
		
		if constexpr (is_same_type<curr,subtraction>){ // special case for `-` in procedure name
			return make_c_list(c_<'-'>{}, second{});
		} else {
			return make_c_list(curr{}, second{});
		}
		
	} else {
		return make_c_list();
	}
}

// KEYWORDS ----

template <typename Lambda>
constexpr auto make_keyword_name(Lambda str_lambda) {
    auto constexpr end = str_lambda().size();
	return tokenize_char_list<Lambda,0,end>(str_lambda);
}

// check somewhere if the comp proc is a keyword
// expect void for return
template <typename... Args>
constexpr auto deduce_keyword_type(c_list<Args...>) {
	return;	
}

#define KEYWORD(name,corresponding_type) 	                                                \
using keyword_##corresponding_type = decltype(make_keyword_name(constexpr_string(name)));   \
template <> 				                                                                \
constexpr auto deduce_keyword_type(keyword_##corresponding_type) { 	                        \
	return corresponding_type{};			                                                \
}

KEYWORD("if",scm_if);
KEYWORD("define",scm_define);
KEYWORD("cons",scm_cons);
KEYWORD("list",scm_list);
KEYWORD("car",scm_car);
KEYWORD("cdr",scm_cdr);
KEYWORD("apply",scm_apply);
KEYWORD("eval",scm_eval);

// KEYWORDS ----



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

template <typename Lambda, size_t Index = 0>
constexpr auto tokenize(Lambda str_lambda) {
	constexpr auto str = str_lambda();

	if constexpr (Index < str.size()) {
		// deduce the type of the current char
		using curr = decltype(deduce_token_type< str[Index] >());

		if constexpr (is_same_type<curr,comment_start>){ // skip comments
			auto constexpr next_newline = find_first_newline<Index>(str_lambda);
			using second = decltype(tokenize<Lambda, next_newline>(str_lambda));
			return make_token_list(second{});
		} else if constexpr (is_whitespace_v<curr>) { // include whitespaces for later formatting	
			using second = decltype(tokenize<Lambda, Index + 1>(str_lambda));
			return make_token_list(curr{}, second{});
		} else {
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
				// if something starts with a character, find the next non character
				constexpr auto end_of_char_list = find_first_non_c< Index + 1 >(str_lambda);

				constexpr auto one_char = Index == end_of_char_list - 1;

				// the tokeniser does not know anything about

				if constexpr (one_char){
					using second = decltype(tokenize< Lambda, Index + 1 >(str_lambda));
					return make_token_list(curr{}, second{});
				} else {
					using second = decltype(tokenize< Lambda, end_of_char_list >(str_lambda));
					
					using char_list = decltype(tokenize_char_list< Lambda, Index, end_of_char_list >(str_lambda));
					using query_result = decltype(deduce_keyword_type(char_list{}));
					
					// check if the string is a keyword
					if constexpr (!is_same_type<void,query_result>){
						return make_token_list(query_result{}, second{});
					} else {
						return make_token_list(char_list{}, second{});
					}
				}

			} else {
				// if its not a specially handled token
				using next = decltype(tokenize< Lambda, Index + 1 >(str_lambda));
				return make_token_list(curr{}, next{});
			}
		}
		
	} else {
		return make_token_list();
	}
}


// replace outer list with another supplied in the parameter
template <template<class> class T, template<class> class Replacement, typename... Args,typename... Brgs>
constexpr auto replace_wrapper(T<Args...>,Replacement<Brgs...>){
	return Replacement<Args...>{};
}

LIST(tokenized);

template <typename Lambda, size_t Index = 0>
constexpr auto tokenizer(Lambda str_lambda) {
	using result = decltype(tokenize<Lambda, Index>(str_lambda));
	using tokens = decltype(clean_whitespaces(result{}));
	using clean_expression = typename replace_nested_list<tokens>::type; // convert list
	return replace_wrapper(clean_expression{},tokenized{});
}
