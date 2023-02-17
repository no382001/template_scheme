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

using testaddev = decltype(addition(list<integer<2>,integer<3>>{}));
using testaddevres = decltype(integer<5>{});
static_assert(is_same_type<testaddev,testaddevres>,"additon of two integers");

struct scm_false {};
struct scm_true {};

// since the constexpr discarded functions have to be syntactically correct
// it still cant deduct two different types, in the preprocessor it works flawlessly
// with me returning scm_false{} and integer<>{}
// so i will have to use presumed integer edge cases, like number 99999
#define PRIMITIVE_RELATIONAL_OP(name,sign)												\
template <int A, typename... Rest>														\
auto constexpr sub_##name(list<integer<A>,Rest...>){									\
	if constexpr (sizeof...(Rest) == 0) {												\
		return A;																		\
	} else {																			\
		auto constexpr second = sub_##name(list<Rest...>{});							\
		if constexpr (second == 99999){													\
			return 99999;																\
		} else if constexpr (A sign second){											\
			return A;																	\
		} else {																		\
			return 99999;																\
		}																				\
	}																					\
}																						\
template <int A, typename... Rest>														\
auto constexpr name(list<integer<A>,Rest...>){											\
	static_assert((sizeof...(Rest) > 0),"no second operand, for relational operation"); \
	auto constexpr second = sub_##name(list<Rest...>{});								\
	if constexpr (second == 99999){														\
		return scm_false{};																\
	} else if constexpr (A sign second){												\
		return scm_true{};																\
	} else {																			\
		return scm_false{};																\
	}																					\
}

PRIMITIVE_RELATIONAL_OP(equal,==);
PRIMITIVE_RELATIONAL_OP(less,<);
PRIMITIVE_RELATIONAL_OP(more,>);

using testrelv2 = decltype(less(list<integer<2>,integer<3>,integer<4>>{}));
using testrelres2 = decltype(scm_true{});
static_assert(is_same_type<testrelv2,testrelres2>,"rel of two integers: positive test");
using testrelv24 = decltype(less(list<integer<3>,integer<2>>{}));
using testrelres24 = decltype(scm_false{});
static_assert(is_same_type<testrelv24,testrelres24>,"rel of two integers: negative test");

using testrelv2s = decltype(more(list<integer<2>,integer<3>,integer<4>>{}));
using testrelres2s = decltype(scm_false{});
static_assert(is_same_type<testrelv2s,testrelres2s>,"rel of two integers: positive test");
using testrelv24aa = decltype(more(list<integer<3>,integer<2>>{}));
using testrelres24aa = decltype(scm_true{});
static_assert(is_same_type<testrelv24aa,testrelres24aa>,"rel of two integers: negative test");

using testrelv2sa = decltype(equal(list<integer<2>,integer<2>,integer<2>>{}));
using testrelres2sa = decltype(scm_true{});
static_assert(is_same_type<testrelv2sa,testrelres2sa>,"rel of two integers: positive test");
using testrelv24aaa = decltype(equal(list<integer<3>,integer<2>>{}));
using dfff = decltype(scm_false{});
static_assert(is_same_type<testrelv24aaa,dfff>,"rel of two integers: negative test");

//using testrelv24 = decltype(less(list<integer<3>>{}));