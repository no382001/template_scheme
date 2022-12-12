#pragma once
#include "lists.h"
#include "atoms.h"
#include "car_crd.h"
// after tokenization a resolve_symbols function should be run
// that rearranges the tokens ina  way that symboos are resolved and the parser can work in peace

// entry datatype: symbol, //layer, //argnum, expression
template <typename symbol,/*int layer, int argnum,*/ typename expression>
struct table_entry {
    static constexpr auto get_symbol() {return symbol{};}
};

// positive search
template < typename candidate, typename A, typename... Rest>
constexpr auto table_search(candidate c, A a, Rest ...rest) {
    if constexpr (sizeof...(Rest) > 0){
        using curr = A::get_symbol(); 
        if constexpr (is_same_type<curr,candidate>) {
            return A{};
        } else {
            return table_search(c,rest...);
        }
    } else {
        return A{};
    }
}

LIST(table);

IS_X_LIST(token_list);
IS_X_LIST(list);

template <typename Lambda, size_t Index = 0>
//pass a stringview return type lambda that passes the arguments with __VA_ARGS__
constexpr auto tokenize_table_entries(Lambda str_lambda) {
	constexpr auto str = str_lambda();
	if constexpr (Index < str.size()) {
		
		using curr = decltype(deduce_token_type< str[Index] >());
		
		if constexpr (is_char_v<curr>) {
			//if something starts with a character, find the next non character
			constexpr auto end_of_char_list = find_first_non_c< Index >(str_lambda);
			// tokenize the contents of the list and return it in a wrapper
			if constexpr (end_of_char_list > 0) {
				using char_list = decltype(tokenize_char_list< Lambda, Index, end_of_char_list >(str_lambda));
				using second = decltype(tokenize_table_entries< Lambda, end_of_char_list >(str_lambda));
				return make_table(char_list{}, second{});
			} else {
				using second = decltype(tokenize< Lambda, Index + 1 >(str_lambda));
				return make_table(curr{}, second{});
			}
		} else if constexpr (is_same_type<curr, list_start>) { // handle nested lists
			constexpr auto end_of_list = find_end_of_list< Index >(str_lambda);
			// tokenize the contents of the list and return it in a wrapper
			using list = decltype(tokenize_table_entries< Lambda, Index + 1>(str_lambda));
			using second = decltype(tokenize_table_entries<Lambda, end_of_list + 1>(str_lambda));
			return make_table(list{}, second{});
		
		} else if constexpr (is_same_type<curr, list_end>) { //return cond for EOList
			return make_table();

		} else {
			return tokenize_table_entries< Lambda, Index + 1 >(str_lambda);
		}
	} else {
		return make_table();
	}
}