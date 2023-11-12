#pragma once
#include <type_traits>

#include "atoms.h"


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

// preproc eval defer
#define EMPTY()
#define DEFER_INNER(m) m EMPTY EMPTY EMPTY()
#define DEFER(m) DEFER_INNER(m)

// a compile time warning, problem wont come up until the branch is preprocessed
#define COMPILE_TIME_WARNING "Compile time warning"
// struct COMPILE_TIME_WARNING {};


#include <string_view>
#define constexpr_string(str) ([]() constexpr -> std::string_view { return str; })