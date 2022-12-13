#pragma once
#include "lists.h"
#include "atoms.h"
#include "car_crd.h"


// naturally after define the content of the whole list is saved here, i guess ill parse it elsewhere
template <typename list>
struct table_entry {};



// positive search
template <typename candidate, template<class,class> class T, typename A, typename... Args>
auto constexpr table_search(candidate,T<A,Args...>){
    if constexpr (sizeof...(Args) > 0) {
		using curr = decltype(car(car(A{}))); // entry/token_list
		if constexpr (is_same_type<candidate,curr>){
			return A{};
		}else {
			return table_search(candidate{},T<Args...>{});
		}
    } else {
        return A{};
    }
}

LIST(table)

IS_X_LIST(token_list);
IS_X_LIST(list);

// gather_table_entries only handles (define ...) type expressions, if any other is found, the node is terminated
// no scoping yet, everything is global
template <typename Lambda, size_t Index = 0>
constexpr auto gather_table_entries(Lambda str_lambda) {
	constexpr auto str = str_lambda();
	if constexpr (Index < str.size()) {
		using curr = decltype(deduce_token_type< str[Index] >());
		if constexpr (is_char_v<curr>) {
			//if something starts with a character, find the next non character
			constexpr auto end_of_char_list = find_first_non_c< Index >(str_lambda);
			// tokenize the contents of the list and return it in a wrapper
			if constexpr (end_of_char_list > 0) {
				using char_list = decltype(tokenize_char_list< Lambda, Index, end_of_char_list >(str_lambda));
				
				// if its a define expression, ignore node, another tokenizing process deals with that
				if constexpr (is_same_type<_define,char_list>){
					using second = decltype(tokenize< Lambda, end_of_char_list >(str_lambda));
					return table_entry<second>{};
				} else {
					return make_table();
				}
			} else {
				using second = decltype(tokenize< Lambda, Index + 1 >(str_lambda));
				return make_token_list(curr{}, second{});
			}
		} else if constexpr (is_same_type<curr, list_start>) { // handle nested lists
			constexpr auto end_of_list = find_end_of_list< Index >(str_lambda);
			// tokenize the contents of the list and return it in a wrapper
			using list = decltype(gather_table_entries< Lambda, Index + 1>(str_lambda));
			using second = decltype(gather_table_entries<Lambda, end_of_list + 1>(str_lambda));
			return make_table(list{}, second{});
		} else if constexpr (is_same_type<curr, list_end>) { //return cond for EOList
			return make_table();

		} else {
			return gather_table_entries< Lambda, Index + 1 >(str_lambda);
		}
	} else {
		return make_table();
	}
}

template <typename  Table,typename Lambda, size_t Index>
//pass a stringview return type lambda that passes the arguments with __VA_ARGS__
constexpr auto tokenize_list_w_table(Lambda str_lambda) {
	using curr = decltype(tokenize_w_table<Table, Lambda, Index + 1 >(str_lambda));
	return make_list(curr{});
}

template <typename Table, typename Lambda, size_t Index = 0>
constexpr auto tokenize_w_table(Lambda str_lambda) {
	constexpr auto str = str_lambda();

	if constexpr (Index < str.size()) {
		// deduce the type of the current char
		using curr = decltype(deduce_token_type< str[Index] >());

		if constexpr (is_same_type<curr, list_start>) {
			constexpr auto end_of_list = find_end_of_list< Index >(str_lambda);
			// tokenize the contents of the list and return it in a wrapper
			using l = decltype(tokenize_list_w_table<Table, Lambda, Index>(str_lambda));
			using second = decltype(tokenize_w_table<Table, Lambda, end_of_list + 1>(str_lambda));

			// drop empty expressions, define handling
			if constexpr (is_same_type<_empty_list,l>){
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
			using second = decltype(tokenize_w_table<Table, Lambda, first_non_integer >(str_lambda));
			return make_token_list(integer_type{}, second{});
		} else if constexpr (is_char_v<curr>) {
			//if something starts with a character, find the next non character
			constexpr auto end_of_char_list = find_first_non_c< Index >(str_lambda);
			// tokenize the contents of the list and return it in a wrapper
			if constexpr (end_of_char_list > 0) {
				using char_list = decltype(tokenize_char_list< Lambda, Index, end_of_char_list >(str_lambda));
				
				// if its a define expression, ignore node, another tokenizing process deals with that
				if constexpr (is_same_type<_define,char_list>){
					return make_token_list();
				} else {
					using entry = decltype(table_search(char_list{},Table{}));
					using entry_value = decltype(car(cdr(car(entry{})))); // ugly

					using second = decltype(tokenize_w_table<Table, Lambda, end_of_char_list >(str_lambda));
					return make_token_list(entry_value{}, second{});
				}		

			} else {
				using second = decltype(tokenize_w_table<Table, Lambda, Index + 1 >(str_lambda));
				return make_token_list(curr{}, second{});
			}
		} else if constexpr (is_same_type<curr,whitespace>) {
			// if its not a specially handled token
			return tokenize_w_table<Table, Lambda, Index + 1 >(str_lambda);
		} else {
			// if its not a specially handled token
			using next = decltype(tokenize_w_table<Table, Lambda, Index + 1 >(str_lambda));
			return make_token_list(curr{}, next{});
		}
	} else {
		return make_token_list();
	}
}