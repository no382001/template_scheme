#pragma once
#include "atoms.h"
#include "lists.h"
#include "primitive_operations.h"
#include "utils.h"
#include "car_cdr.h"

struct list_start {};
struct list_end {};

template <int I>
struct whitespace {};

// whitespace is a special _c<...>
template <typename T>
constexpr inline bool is_whitespace_v = is_specific_templated_int_collection< T, whitespace >::value;


// this could be replaced with the something like the KEYWORD in tokenizer.h
// - special characters in procedure names that are used as other symbols, 
//   are handled by tokenize_char_list and find_first_non_c
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
	} else if constexpr (C == '?') { // special character probably in a proc name
		return c_<C>{};
	} else if constexpr (C == '\'') {
		return quote{};
	} else if constexpr (C == '+') {
		return addition{};
	} else if constexpr (C == '-') {
		return subtraction{};
	} else if constexpr (C == '*') {
		return multiplication{};
	} else if constexpr (C == '/') {
		return division{};
	} else if constexpr (C == ' ') {
		return whitespace<C>{};
	} else if constexpr (C == '<') {
		return less{};
	} else if constexpr (C == '>') {
		return more{};
	} else if constexpr (C == '=') {
		return equal{};
	} else if constexpr (C == '\n') { // needed for multiline string compatibility like R"()" 
		return whitespace<C>{};
	} else if constexpr (C == ';') {
		return comment_start{};
	} else if constexpr (C == ',') { // probably in comments
		return whitespace<C>{};
	} else if constexpr (C == '#') {
		return boolean_start{};
	} else {
		static_assert(DELAYED_FALSE,"symbol not supported");
	}
}