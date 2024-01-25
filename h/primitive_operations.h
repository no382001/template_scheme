#pragma once
#include "lists.h"
#include "car_cdr.h"
#include "atoms.h"
#include <algorithm>

template <typename Env, typename Exp>
auto constexpr IReval(wrap<Exp>);

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
template <int X, int... Xs>                                                  			\
auto constexpr apply_##name(list<integer<X>, integer<Xs>...>) {                   		\
	return integer<X sign (... sign Xs)>{};                                           	\
}

PRIMITIVE_ARITHMETIC_OP(addition,+);
PRIMITIVE_ARITHMETIC_OP(subtraction,-);
PRIMITIVE_ARITHMETIC_OP(multiplication,*);
PRIMITIVE_ARITHMETIC_OP(division,/);

// -----------------------------------------------

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
template <int X, int... Xs>																\
auto constexpr all_##name(list<integer<X>, integer<Xs>...>) {							\
	return (... && (X sign Xs));														\
}																						\
																						\
template <int X, int... Xs>	/* need another overload for scm_true and scm_false */		\
auto constexpr apply_##name(list<integer<X>, integer<Xs>...>) {							\
	auto constexpr all = all_##name(list<integer<X>, integer<Xs>...>{});				\
	if constexpr (all){																	\
		return scm_true{};																\
	} else {																			\
		return scm_false{};																\
	} 																					\
}

PRIMITIVE_RELATIONAL_OP(equal,==);
PRIMITIVE_RELATIONAL_OP(less,<);
PRIMITIVE_RELATIONAL_OP(more,>);
PRIMITIVE_RELATIONAL_OP(lesseq,<=);

// --------------------------------------

/*
1 ]=> (and #t #f)
;Value: #f
1 ]=> (and 1 0)
;Value: 0
*/

#define PRIMITIVE_BOOLEAN_OP(name,sign)													\
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
template <typename... Xs>						                                    	\
constexpr auto apply_##name(list<Xs...>) {                     							\
	auto constexpr result =  (Xs::value() sign ...);										\
	using first_type = decltype(IRcar(list<Xs...>{}));									\
	if constexpr (result) {																\
		if constexpr (is_integer_v<first_type>){										\
			return integer<1>{};														\
		} else {																		\
			return scm_true{};															\
		}																				\
	} else {																			\
		if constexpr (is_integer_v<first_type>){										\
			return integer<0>{};														\
		} else {																		\
			return scm_false{};															\
		}																				\
	}																					\
}

PRIMITIVE_BOOLEAN_OP(_and,&&);
PRIMITIVE_BOOLEAN_OP(_or,||);
PRIMITIVE_BOOLEAN_OP(_xor,^);
PRIMITIVE_BOOLEAN_OP(_not,!=);

// ------------------------

using andddd = decltype(apply__and(list<integer<1>,integer<1>>{}));
static_assert(is_same_type<andddd,integer<1>>,"");
using andddd2 = decltype(apply__and(list<integer<0>,integer<1>>{}));
static_assert(is_same_type<andddd2,integer<0>>,"");

using anddddf = decltype(apply__or(list<integer<1>,integer<1>>{}));
static_assert(is_same_type<anddddf,integer<1>>,"");
using andddd2f = decltype(apply__or(list<integer<0>,integer<1>>{}));
static_assert(is_same_type<andddd2f,integer<1>>,"");

using anddddff = decltype(apply__or(list<scm_true,scm_true>{}));
static_assert(is_same_type<anddddff,scm_true>,"");
using andddd2ff = decltype(apply__or(list<scm_false,scm_true>{}));
static_assert(is_same_type<andddd2ff,scm_true>,"");

// -----------------------------------------------

struct scm_if {};
struct scm_define {};
struct scm_let {};
struct scm_lambda {};
struct scm_cons {};
struct scm_list {};
struct scm_car {};
struct scm_cdr {};

IS_SELF_EVALUATING(scm_if);
IS_SELF_EVALUATING(scm_true);
IS_SELF_EVALUATING(scm_false);
IS_SELF_EVALUATING(scm_define);
IS_SELF_EVALUATING(scm_lambda);
IS_SELF_EVALUATING(scm_cons);
IS_SELF_EVALUATING(scm_list);
IS_SELF_EVALUATING(scm_car);
IS_SELF_EVALUATING(scm_cdr);

template <typename Env,typename Predicate, typename Then, typename Else>
auto constexpr if_proc(Predicate, Then, Else){
	using res = decltype(IReval<Env>(make_wrap(Predicate{}))); // make_wrap so it handles not wrapped predicates too
	if constexpr (is_same_type<scm_true,res> || is_same_type<integer<1>,res>){
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