#pragma once
#include "lists.h"
#include "car_cdr.h"
#include "utils.h"
#include "atoms.h"

#define PRIMITIVE_ARITHMETIC_OP(name,sign)                                                     \
template <int A, typename... Rest>                                                  \
auto constexpr sub_##name(list<integer<A>, Rest...>) {                              \
	if constexpr (sizeof...(Rest) > 0)                                              \
		return A sign sub_##name(list<Rest...>{});                                  \
	else{ return A; }                                                               \
}                                                                                   \
template <int A, typename... Rest>                                                  \
auto constexpr name(list<integer<A>,Rest...>) {                                     \
        static_assert((sizeof...(Rest) > 0), #name); /* no operands */              \
		return integer<A sign sub_##name(list<Rest...>{})>{};                       \
}                                                                                   \

PRIMITIVE_ARITHMETIC_OP(addition,+);
PRIMITIVE_ARITHMETIC_OP(subtraction,-);
PRIMITIVE_ARITHMETIC_OP(multiplication,*);
PRIMITIVE_ARITHMETIC_OP(division,/);

/** /
#define PRIMITIVE_RELATIONAL_OP(name,sign)                                                     \
template <int A, typename... Rest>                                                  \
auto constexpr sub_##name(list<integer<A>, Rest...>) {                              \
	if constexpr (sizeof...(Rest) > 0)                                              \
		return A sign sub_##name(list<Rest...>{});                                  \
	else{ return A; }                                                               \
}                                                                                   \
template <int A, typename... Rest>                                                  \
auto constexpr name(list<integer<A>,Rest...>) {                                     \
        static_assert((sizeof...(Rest) > 0), #name);             \
		return integer<A sign sub_##name(list<Rest...>{})>{};                       \
}
PRIMITIVE_RELATIONAL_OP(equal,==);
PRIMITIVE_RELATIONAL_OP(less,<);
PRIMITIVE_RELATIONAL_OP(more,>);

/**/
using testaddev = decltype(addition(list<integer<2>,integer<3>>{}));
using testaddevres = decltype(integer<5>{});
static_assert(is_same_type<testaddev,testaddevres>,"additon of two integers");


struct scm_false {};
struct scm_true {};

// since the constexpr discarded parts of the function have to be syntactically correct
// it still cant deduct two different types, in the preprocessor it works flawlessly
// with me returning two different types from the same function, but the compiler fails
// so i will have to use and establish integer edge cases, reserved for the relational operations, like number 99999
template <int A, typename... Rest>
auto constexpr sub_less(list<integer<A>,Rest...>){
	if constexpr (sizeof...(Rest) == 0) {
		return A;
	} else {
		auto constexpr second = sub_less(list<Rest...>{});
		if constexpr (second == 99999 ^ A > second){
			return 99999;
		} else {
			return A;
		}
	}

}

template <int A, typename... Rest>
auto constexpr less(list<integer<A>,Rest...>){
	static_assert((sizeof...(Rest) > 0),"no second operand, for relational operation"); 
	
	auto constexpr second = sub_less(list<Rest...>{});
	if constexpr (second == 99999 ^ A > second){
		return scm_false{};
	} else {
		return scm_true{};
	}
}

using testrelv2 = decltype(less(list<integer<2>,integer<3>,integer<4>>{}));
using testrelres2 = decltype(scm_true{});
static_assert(is_same_type<testrelv2,testrelres2>,"rel of two integers: positive test");

using testrelv24 = decltype(less(list<integer<3>,integer<2>>{}));
using testrelres24 = decltype(scm_false{});
static_assert(is_same_type<testrelv24,testrelres24>,"rel of two integers: negative test");

//using testrelv24 = decltype(less(list<integer<3>>{}));