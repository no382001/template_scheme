#pragma once

#include "lists.h"
#include "car_cdr.h"
#include "utils.h"
#include "atoms.h"
#include "primitive_operations.h"
#include "env.h"

// fwd declare, bc recursively included
template <typename Env = init_env, typename A, typename... Args>
auto constexpr list_of_values(A,Args...);

template <typename Env, typename Exp>
auto constexpr IReval(quote<Exp>);


// temp location for map_pair
LIST(pair);


template <template <class,class> typename One, typename A, typename... Args, template <class,class> typename Two, typename B, typename... Brgs>
auto constexpr map_pair_inner(One<A,Args...>,Two<B,Brgs...>){
    using p = decltype(table_entry<A,B>{});
	if constexpr (sizeof...(Args) == 0) {
		return make_environment(p{});
	} else {
		using second = decltype(map_pair_inner(One<Args...>{},Two<Brgs...>{}));
		return make_environment(p{},second{});
	}
}

// return table_entries with env wrap
template <template <class,class> typename One, typename... Args, template <class,class> typename Two, typename... Brgs>
auto constexpr map_pair(One<Args...>,Two<Brgs...>){
    if constexpr (sizeof...(Args) != sizeof...(Brgs)){
        static_assert(!is_same_type<int,int>,"lists are not the same size");
    } else {
        return map_pair_inner(One<Args...>{},Two<Brgs...>{});
    }
}

template <template <int...> typename One, int... A, template <int...> typename Two, int... B>
auto constexpr map_pair(One<A...>,Two<B...>){
    return make_environment(table_entry<One<A...>,Two<B...>>{});
}


using map_pair_test = decltype(map_pair(list<integer<1>>{},list<integer<1>>{}));
static_assert(is_same_type<map_pair_test,environment<table_entry<integer<1>,integer<1>>>>,"map_pair_test");

//using map_pair_test2 = decltype(map_pair(list<integer<1>>{},list<integer<1>,integer<1>>{}));



// needs refactoring
template <typename Env, typename A, typename... Args>
auto constexpr eval_members(quote<list<A,Args...>>){
        if constexpr (sizeof...(Args) == 0){
            using ev_curr = decltype(IReval<Env>(make_quote(A{})));
            return make_list(ev_curr{});
        } else {
            using ev_curr = decltype(IReval<Env>(make_quote(A{})));
            using ev_second = decltype(IReval<Env>(make_quote(make_list(Args{}...))));
            return make_list(ev_curr{},ev_second{});
        }
}

// needs refactoring
template <typename Env, typename A, typename... Args>
auto constexpr eval_members(list<A,Args...>){
    if constexpr (sizeof...(Args) == 0){
        using ev_curr = decltype(IReval<Env>(make_quote(A{})));
        return make_list(ev_curr{});
    } else {
        using ev_curr = decltype(IReval<Env>(make_quote(A{})));

        using ev_second = decltype(IReval<Env>(make_quote(make_list(Args{}...))));
        return make_list(ev_curr{},ev_second{});
    }
}

template <typename Env, template <int...> typename templated_int, int a>
auto constexpr eval_members(quote<templated_int<a>>){
    return IReval<Env>(quote<templated_int<a>>{});
}

struct apply {};
struct eval {};

template < typename Proc, typename Args >
auto constexpr IRapply(Proc,quote<Args>) {

	using prim_proc = decltype(apply_primitve_procedure(Proc{},Args{}));
    
    // cant instantiate void, so:
    if constexpr (is_same_type<prim_proc,void>){
        return;
    } else {
        return prim_proc{};
    }
}

// (apply addtion '(1 2 3))
using appltest1 = decltype(IRapply(addition{},quote<list<integer<1>,integer<2>,integer<3>>>{}));

template <typename Env, typename Exp>
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
            using evaluated_op = decltype(IReval<Env>(quote<app_operator>{}));

            using app_operands = decltype(eval_members<Env>(IRcaddr(Exp{})));

            // the list here should be evaluated member by member but its a list
            // and i cannot explicitly make a spec for a list, what do?

            using evaluated_operands = decltype(list_of_values<Env>(app_operands{}));


            // if apply returns with void, exec comp proc branch

            using proc_res = decltype(IRapply(app_operator{},quote<app_operands>{}));

            if constexpr (!is_same_type<void,proc_res>){
                return proc_res{};
            } else {
                // its compound, look var in table
                using comp_proc_entry = decltype(list_search(app_operator{},Env{}));
                // cadr is arglist
                using arglist = decltype(IRcadr(comp_proc_entry{}));
                // caddr is expression
                using expression = decltype(IRcaddr(comp_proc_entry{}));
                // make arg and operand pair
               
                // make map_pair return table_entry with env wrap
                using arg_x_op_env = decltype(map_pair(arglist{},evaluated_operands{}));
                // extend env with argument a operand pair
                using temp_ext_env = decltype(extend_environment<init_env>(arg_x_op_env{}));

                // eval members in expression
                // wtf is the order? if env first then cant deduct exp, vice versa
                using result = decltype(IReval<temp_ext_env>(expression{}));

                using op = decltype(IRcar(result{}));
                using opnds = decltype(IRcdr(result{}));

                return IRapply(op{},quote<opnds>{});
            }

        } else {
            if constexpr (is_same_type<Exp,list<>>){
                using curr = decltype(IRcar(Exp{}));
                return make_list();
            } else {
                //using curr = decltype(IRcar(Exp{}));
                //using second = decltype(IRcdr(Exp{}));

                using members = decltype(eval_members<Env>(Exp{}));

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
        using only_operand = decltype(IReval<Env>(quote<A>{}));
        return only_operand{};
    } else {
        using first_operand = decltype(IReval<Env>(quote<A>{}));
        using second = decltype(list_of_values(Args{}...));
        return make_list(first_operand{},second{});
    }
}