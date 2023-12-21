#pragma once
#include <type_traits>

#include "atoms.h"

// REFACTOR, this is retarded
template <template <class> typename A, template <class> typename B, typename... First, typename... Second>
bool constexpr is_same_list_t(A<First...> a,B<Second...> b){ 
	using derivedA = decltype(A{});
	using derivedB = decltype(B{});
	
	if constexpr (is_same_type<derivedA,derivedB>){
		return true;
	} else {
		return false;
	}
}

// for static asserts that need delayed constexpr evaluation
//  - static assert willl always fail with a constexpr konw value during template generation
//  - even when the branch is not used and will be omitted
//  - delay the evaluation of the false value to throw a static assert only when the branch is evaluated
#define DELAYED_FALSE !is_same_type<int,int>

#include <string_view>
#define constexpr_string(str) ([]() constexpr -> std::string_view { return str; })