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
// when a branch needs to fail but not bring the whole tree with it
#define DELAYED_FALSE !is_same_type<int,int>