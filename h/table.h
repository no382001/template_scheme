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
		using curr = decltype(car(car(A{})));
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

template <typename Lambda, size_t Index = 0>
//pass a stringview return type lambda that passes the arguments with __VA_ARGS__
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