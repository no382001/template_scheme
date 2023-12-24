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

namespace what_is_happening_with_this {
	using testaddev = decltype(apply_subtraction(list<integer<2>,integer<3>>{}));
	using testaddevres = decltype(integer<-1>{});
	static_assert(is_same_type<testaddev,testaddevres>,"subtraction of two integers");
};

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
struct scm_cons {};
struct scm_list {};
struct scm_car {};
struct scm_cdr {};
struct scm_and {};
struct scm_or {};
struct scm_not {};

IS_SELF_EVALUATING(scm_if);
IS_SELF_EVALUATING(scm_true);
IS_SELF_EVALUATING(scm_false);
IS_SELF_EVALUATING(scm_define);
IS_SELF_EVALUATING(scm_lambda);
IS_SELF_EVALUATING(scm_cons);
IS_SELF_EVALUATING(scm_list);
IS_SELF_EVALUATING(scm_car);
IS_SELF_EVALUATING(scm_cdr);
IS_SELF_EVALUATING(scm_and);
IS_SELF_EVALUATING(scm_or);
IS_SELF_EVALUATING(scm_not);

template <typename Env,typename Predicate, typename Then, typename Else>
auto constexpr if_proc(Predicate, Then, Else){
	using res = decltype(IReval<Env>(Predicate{}));
	if constexpr (is_same_type<scm_true,res>){
		return IReval<Env>(wrap<Then>{});
	} else {
		return IReval<Env>(wrap<Else>{});
	}													
}

template <typename Env, typename A,typename B>
auto constexpr make_cons(A,B){
	using first = decltype(IReval<Env>(make_wrap(make_list(A{}))));
	using second = decltype(IReval<Env>(make_wrap(make_list(A{}))));
	return cons<first,second>{};
}

template <typename Env, typename A, typename... Args>
auto constexpr eval_every_member_for_cons_list(A,Args...){
    using curr = decltype(make_cons_list(IReval<Env>(make_wrap(A{}))));
    if constexpr (sizeof...(Args) == 0){
        return make_cons_list(curr{});
    } else {
        using second = decltype(IReval<Env>(make_wrap(Args{}...)));
        return make_cons_list(curr{},second{});
    }
}


template <typename A, typename B>
auto constexpr car(cons<A,B>) -> A {}

using cartest = decltype(car(cons<int,void>{}));
static_assert(is_same_type<cartest,int>,"");


template <typename A, typename B>
auto constexpr cdr(cons<A,B>) -> B {}

using cartest2 = decltype(cdr(cons<int,void>{}));
static_assert(is_same_type<cartest2,void>,"");

template <typename A, typename... Args>
auto constexpr car(cons_list<A,Args...>){
	if constexpr (sizeof...(Args) == 0){
		static_assert(DELAYED_FALSE,"car called with 1 argument");
	} else {
		return A{};
	}
}

using cartest3 = decltype(car(cons_list<int,void>{}));
static_assert(is_same_type<cartest3,int>,"");

template <typename A, typename... Args>
auto constexpr cdr(cons_list<A,Args...>){
	if constexpr (sizeof...(Args) == 0){
		static_assert(DELAYED_FALSE,"car called with 1 argument");
	} else {
		return cons_list<Args...>{};
	}
}

using cartest4 = decltype(cdr(cons_list<int,void>{}));
static_assert(is_same_type<cartest4,cons_list<void>>,"");
