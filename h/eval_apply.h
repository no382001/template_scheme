#pragma once

#include "lists.h"
#include "car_cdr.h"
#include "utils.h"
#include "atoms.h"
#include "primitive_operations.h"
#include "list_of_values.h"
#include "env.h"

// fwd declare, bc recursively included
template <typename Env = init_env, typename A, typename... Args>
auto constexpr list_of_values(A,Args...);

template < typename Exp, typename Env = init_env >
auto constexpr IReval(quote<Exp>);


// needs refactoring
template <typename A, typename... Args>
auto constexpr eval_members(quote<list<A,Args...>>){
        if constexpr (sizeof...(Args) == 0){
            using ev_curr = decltype(IReval(make_quote(A{})));
            return make_list(ev_curr{});
        } else {
            using ev_curr = decltype(IReval(make_quote(A{})));
            using ev_second = decltype(IReval(make_quote(make_list(Args{}...))));
            return make_list(ev_curr{},ev_second{});
        }
}

// needs refactoring
template <typename A, typename... Args>
auto constexpr eval_members(list<A,Args...>){
    if constexpr (sizeof...(Args) == 0){
        using ev_curr = decltype(IReval(make_quote(A{})));
        return make_list(ev_curr{});
    } else {
        using ev_curr = decltype(IReval(make_quote(A{})));

        using ev_second = decltype(IReval(make_quote(make_list(Args{}...))));
        return make_list(ev_curr{},ev_second{});
    }
}


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

template < typename Exp, typename Env = init_env>
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

            using app_operands = decltype(eval_members(IRcaddr(Exp{})));

            // the list here should be evaluated member by member but its a list
            // and i cannot explicitly make a spec for a list, what do?

            using evaluated_operands = decltype(list_of_values(app_operands{})); // work in progress

            return IRapply(app_operator{},quote<app_operands>{});
        } else {
            if constexpr (is_same_type<Exp,list<>>){
                using curr = decltype(IRcar(Exp{}));
                return make_list();
            } else {
                //using curr = decltype(IRcar(Exp{}));
                //using second = decltype(IRcdr(Exp{}));

                using members = decltype(eval_members(Exp{}));

                return members{};
            // maybe here if its not a tagged list its a normal one right?s
            }
        }
    }
}

// variable arg template for IReval


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