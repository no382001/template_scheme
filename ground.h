#pragma once
#include "tests.h"
#include "h/lists.h"
#include "h/car_cdr.h"
#include "h/utils.h"
#include "h/atoms.h"
#include "h/primitive_operations.h"
#include "tests.h"
#include "h/list_of_values.h"

using init_env = decltype(make_environment(table_entry<c_<110>,integer<1>>{}));
// fwd declare
template <typename Env = init_env, typename A, typename... Args>
auto constexpr list_of_values(A,Args...);

struct apply {};
struct eval {};

template < typename Proc, typename Args >
auto constexpr IRapply(Proc,quote<Args>) {
    // addition '(1 2 3)
    // if its not a prim it could be a compound
	using prim_proc = decltype(apply_primitve_procedure(Proc{},Args{}));

    return prim_proc{};

}

// (apply addtion '(1 2 3))
using appltest1 = decltype(IRapply(addition{},quote<list<integer<1>,integer<2>,integer<3>>>{}));




template < typename Exp, typename Env = init_env >
auto constexpr IReval(quote<Exp>) {
    // this is the problem
    if constexpr (is_char_v<Exp> || is_c_list(Exp{})){
        // it is a variable
        using var_res = decltype(list_search(Exp{},Env{}));
        // return the value of the variable
        return IRcadr(var_res{});

    } else if constexpr (is_same_type<apply,Exp> || is_same_type<addition,Exp> || is_integer_v<Exp>){
        // self-evaluating
        return Exp{};
    } else {
        using indicator = decltype(IRcar(Exp{}));
        // if it is an application
        if constexpr (is_same_type<apply,indicator>){
            using app_operator = decltype(IRcadr(Exp{}));
            using evaluated_op = decltype(IReval(quote<app_operator>{}));

            using app_operands = decltype(IRcaddr(Exp{}));

            // the list here should be evaluated member by member but its a list
            // and i cannot explicitly make a spec for a list, what do?

            using evaluated_operands = decltype(list_of_values(app_operands{})); // work in progress

            return IRapply(app_operator{},app_operands{});
        } else {
            if constexpr (is_same_type<Exp,list<>>){
                using curr = decltype(IRcar(Exp{}));
                return make_list();
            } else {
                using curr = decltype(IRcar(Exp{}));
                using second = decltype(IRcdr(Exp{}));
                return make_list(curr{},second{});
            // maybe here if its not a tagged list its a normal one right?s
            }
        }
    }
}

// list of values goes here when ready
// (list-of-values exps env)
template <typename Env, typename A, typename... Args>
auto constexpr list_of_values(A,Args...){
    if constexpr (sizeof...(Args) == 0){
        using only_operand = decltype(IReval(quote<A>{}));
        return only_operand{};
    } else {
        using first_operand = decltype(IReval(quote<A>{}));
        using second = decltype(list_of_values(Args{}...));
        return make_list(first_operand{},second{});
    }
}

using list_of_test = decltype(list_of_values(c_<110>{},c_<110>{}));
static_assert(is_same_type<list_of_test,list<integer<1>, integer<1>>>,"serach and replace, with init env containing n as 1");

using sssss = decltype(IReval(quote<c_<110>>{}));
static_assert(is_same_type<sssss,integer<1>>,"ireval on variable, with init env containing n as 1");

// (eval '(apply addition '(1 2 3)))
using evaltest1 = decltype(IReval(quote<list<apply,addition,quote<list<integer<1>,integer<2>,integer<3>>>>>{}));
using evaltest21 = decltype(integer<6>{});
static_assert(is_same_type<evaltest1,evaltest21>,"(eval '(apply addition '(1 2 3)))");

// (eval '(apply proc 'body 'param '(1 2 3) 'env))
using proctest1 = decltype(IReval(quote<list<apply,addition,quote<list<integer<1>,integer<2>,integer<3>>>>>{}));
//static_assert(is_same_type<proctest1,evaltest21>,"(eval '(apply addition '(1 2 3)))");

//using vartest = decltype(IReval(quote<list<apply,addition,quote<list<c_<110>,integer<2>>>>>{}));