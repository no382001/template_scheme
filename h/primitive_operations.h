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
template <typename T, typename Arguments> 
auto constexpr apply_primitve_procedure(T,Arguments){
	//static_assert(DELAYED_FALSE,"no such primitive procedure T");
	return;
}

#define PRIMITIVE_ARITHMETIC_OP(name,sign)                                          	\
																						\
struct name {}; /* identifying, "quoted" name */										\
																						\
IS_SELF_EVALUATING(name);																\
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
IS_SELF_EVALUATING(name);																\
template <>																				\
constexpr bool is_prim_proc(name){														\
	return true;																		\
}																						\
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
PRIMITIVE_RELATIONAL_OP(lesseq,<=);

// --------------------------------------

struct scm_if {};
struct scm_define {};
struct scm_let {};
struct scm_lambda {};

IS_SELF_EVALUATING(scm_if);
IS_SELF_EVALUATING(scm_true);
IS_SELF_EVALUATING(scm_false);
IS_SELF_EVALUATING(scm_define);
IS_SELF_EVALUATING(scm_lambda);


template <typename Env,typename Predicate, typename Then, typename Else>
auto constexpr if_proc(Predicate, Then, Else){
	using res = decltype(IReval<Env>(Predicate{}));
	if constexpr (is_same_type<scm_true,res>){
		return IReval<Env>(wrap<Then>{});
	} else {
		return IReval<Env>(wrap<Else>{});
	}													
}