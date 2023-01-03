#pragma once
#include "atoms.h"
#include "utils.h"
#include "lists.h"
#include "number_operations.h"


// the outer layer
template < typename A, typename ...Rest >
auto constexpr parse(token_list< A, Rest... >) {
	// extract the first argument of token_list
	if constexpr (sizeof...(Rest) >= 0) {
		return handle_prefix(A{});
		//return define_atom(constexpr_string("abcd")) == A;
	} else {
		return 0;
	}
}