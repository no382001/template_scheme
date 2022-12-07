#pragma once
#include "utils.h"
#include "lists.h"

struct list_start {};
struct list_end {};
struct double_quote {};
struct quote {};
struct plus {};
struct minus {};
struct mul {};
struct div_ {};
struct term {};
struct whitespace {};
struct non_integer {};
struct non_character {};

template <int Value>
struct c_ {};

template <int Value>
struct special_character {};

template <int Value>
struct integer{
	template < int A >
	static constexpr auto merge(integer<A>)->integer< _pow((Value * 10), (_log(10, A) + 1)) + A >; // int concatenation
	static constexpr auto merge(non_integer)->integer<Value>;
};

template <int C>
constexpr auto deduce_token_type() {
	if constexpr (C == '(') {
		return list_start{};
	} else if constexpr (C >= '0' && C <= '9') {
		return integer<C - '0'>{};
	} else if constexpr (C == ')') {
		return list_end{};
	} else if constexpr (C >= 'a' && C <= 'z') {
		return c_<C>{};
	} else if constexpr (C >= 'A' && C <= 'Z') {
		return c_<C>{};
	} else if constexpr (C == '"') {
		return double_quote{};
	} else if constexpr (C == '\'') {
		return quote{};
	} else if constexpr (C == '+') {
		return plus{};
	} else if constexpr (C == '-') {
		return minus{};
	} else if constexpr (C == '*') {
		return mul{};
	} else if constexpr (C == '/') {
		return div_{};
	} else if constexpr (C == ' ') {
		return whitespace{};
	}
}

template< typename Test, template < int... > class Type >
struct is_templated_int_collection : std::false_type {};

template< template < int... > class Type, int ...Args >
struct is_templated_int_collection< Type< Args... >, Type > : std::true_type {};

template < typename T >
constexpr inline bool is_integer_v = is_templated_int_collection< T, integer >::value;

template <typename T>
constexpr inline bool is_char_v = is_templated_int_collection< T, c_ >::value;

#define IS_X_LIST(name)							\
template< typename T>							\
auto constexpr is_##name(T t) {						\
	return false;								\
}												\
template< typename ...Args>						\
auto constexpr is_##name(name<Args...> l) {		\
	return true;								\
}

IS_X_LIST(c_list);


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

// whitespaces are not being tokenized currently, so wro symbols next to each other will merge
template <typename Lambda, int Index = 0>
constexpr auto define_atom(Lambda lambda) {
	constexpr auto str = lambda();
	if constexpr (Index < str.size()) {
		using curr = decltype(deduce_token_type< str[Index] >());
		using second = decltype(define_atom< Lambda, Index + 1>(lambda));
		return make_c_list(curr{}, second{});
	} else {
		return make_c_list();
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