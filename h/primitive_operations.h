#pragma once
#include "lists.h"
#include "car_cdr.h"
#include "utils.h"
#include "atoms.h"


template <typename T>
constexpr bool is_prim_proc(T){
	return false;
}
// base case for apply_primitve_procedure
// template arguments are oveloaded in each macro, if the proc is not primitive, it returns with void
//
// !is_same_type<T,T> so the compiler does not know until template instantiation the value of the expression
// otherwise if i were to enter 0, even if the template is not instantiated, the value is known and the assert is in place
template <typename T, typename Arguments> 
auto constexpr apply_primitve_procedure(T,Arguments){
	//static_assert(!is_same_type<T,T>,"no such primitive procedure T");
	return;
}

#define PRIMITIVE_ARITHMETIC_OP(name,sign)                                          	\
																						\
struct name {}; /* identifying name, "quoted" name */									\
																						\
template <>																				\
constexpr bool is_prim_proc(name){														\
	return true;																		\
}																						\
template <typename Arguments> /* apply_primitve_procedure overload */					\
auto constexpr apply_primitve_procedure(name,Arguments){								\
	return apply_##name(Arguments{});													\
}																						\
																						\
template <int A, typename... Rest>                                                  	\
auto constexpr sub_apply_##name(list<integer<A>, Rest...>) {                            \
	if constexpr (sizeof...(Rest) > 0)                                              	\
		return A sign sub_apply_##name(list<Rest...>{});                                \
	else{ return A; }                                                               	\
}                                                                                   	\
																						\
template <int A, typename... Rest>                                                  	\
auto constexpr apply_##name(list<integer<A>,Rest...>) {                                 \
        static_assert((sizeof...(Rest) > 0), #name); /* no operands */              	\
		return integer<A sign sub_apply_##name(list<Rest...>{})>{};                     \
}                                                                                   	\

PRIMITIVE_ARITHMETIC_OP(addition,+);
PRIMITIVE_ARITHMETIC_OP(subtraction,-);
PRIMITIVE_ARITHMETIC_OP(multiplication,*);
PRIMITIVE_ARITHMETIC_OP(division,/);

using testaddev = decltype(apply_addition(list<integer<2>,integer<3>>{}));
using testaddevres = decltype(integer<5>{});
static_assert(is_same_type<testaddev,testaddevres>,"additon of two integers");

// since the constexpr discarded functions have to be syntactically correct
// it still cant deduct two different types, in the preprocessor it works flawlessly
// with me returning scm_false{} and integer<>{}, but the compioler will not allow that
// so i will have to establish and use integer edge cases, in this case number 99999 for a false return
#define PRIMITIVE_RELATIONAL_OP(name,sign)												\
																						\
struct name {}; /* identifying name, "quoted" name */									\
																						\
template <typename Arguments> /* apply_primitve_procedure overload */					\
auto constexpr apply_primitve_procedure(name,Arguments){								\
	return apply_##name(Arguments{});													\
}																						\
																						\
template <int A, typename... Rest>														\
auto constexpr sub_apply_##name(list<integer<A>,Rest...>){								\
	if constexpr (sizeof...(Rest) == 0) {												\
		return A;																		\
	} else {																			\
		auto constexpr second = sub_apply_##name(list<Rest...>{});						\
		if constexpr (second == 99999){													\
			return 99999;																\
		} else if constexpr (A sign second){											\
			return A;																	\
		} else {																		\
			return 99999;																\
		}																				\
	}																					\
}																						\
																						\
template <int A, typename... Rest>														\
auto constexpr apply_##name(list<integer<A>,Rest...>){									\
	static_assert((sizeof...(Rest) > 0),"no second operand, for relational operation"); \
	auto constexpr second = sub_apply_##name(list<Rest...>{});							\
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

//using testrelv24 = decltype(less(list<integer<3>>{}));
using testrelv2 = decltype(apply_less(list<integer<2>,integer<3>,integer<4>>{}));
using testrelres2 = decltype(scm_true{});
static_assert(is_same_type<testrelv2,testrelres2>,"rel of two integers: positive test");
using testrelv24 = decltype(apply_less(list<integer<3>,integer<2>>{}));
using testrelres24 = decltype(scm_false{});
static_assert(is_same_type<testrelv24,testrelres24>,"rel of two integers: negative test");

using testrelv2s = decltype(apply_more(list<integer<2>,integer<3>,integer<4>>{}));
using testrelres2s = decltype(scm_false{});
static_assert(is_same_type<testrelv2s,testrelres2s>,"rel of two integers: positive test");
using testrelv24aa = decltype(apply_more(list<integer<3>,integer<2>>{}));
using testrelres24aa = decltype(scm_true{});
static_assert(is_same_type<testrelv24aa,testrelres24aa>,"rel of two integers: negative test");

using testrelv2sa = decltype(apply_equal(list<integer<2>,integer<2>,integer<2>>{}));
using testrelres2sa = decltype(scm_true{});
static_assert(is_same_type<testrelv2sa,testrelres2sa>,"rel of two integers: positive test");
using testrelv24aaa = decltype(apply_equal(list<integer<3>,integer<2>>{}));
using dfff = decltype(scm_false{});
static_assert(is_same_type<testrelv24aaa,dfff>,"rel of two integers: negative test");

using testr22elv2 = decltype(apply_primitve_procedure(addition{},list<integer<2>,integer<3>,integer<4>>{}));

using testr22elv22 = decltype(apply_primitve_procedure(int{},list<integer<2>,integer<3>,integer<4>>{}));
static_assert(is_same_type<testr22elv22,void>,"not prim proc");