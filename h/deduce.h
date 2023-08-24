#pragma once
#include "atoms.h"
#include "lists.h"
#include "primitive_operations.h"
#include "utils.h"

/** \brief list_start token*/
struct list_start {};
/** \brief list_end token*/
struct list_end {};

/** \brief quote_start token*/
struct quote_start {};
/** \brief quote_end token*/
struct quote_end {};

/** \brief whitespace token*/
struct whitespace {};

// TODO: handle multi character symbols

/** \brief given an character, return the token type*/
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
	} else if constexpr (C == '\'') {
		return quote_start{};
	} else if constexpr (C == '+') {
		return addition{};
	} else if constexpr (C == '-') {
		return subtraction{};
	} else if constexpr (C == '*') {
		return multiplication{};
	} else if constexpr (C == '/') {
		return division{};
	} else if constexpr (C == ' ') {
		return whitespace{};
	} else if constexpr (C == '<') {
		return less{};
	} else if constexpr (C == '>') {
		return more{};
	} else if constexpr (C == '=') {
		return equal{};
	} else {
		static_assert(DELAYED_FALSE,"symbol not supported");
	}
}