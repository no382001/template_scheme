#pragma once
#include "atoms.h"
#include "lists.h"
#include "primitive_operations.h"
#include "utils.h"
#include "car_cdr.h"

struct list_start {};
struct list_end {};

struct quote_start {};
struct quote_end {};

struct whitespace {};

// this could be replaced with the KEYWORD in tokenizer.h
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