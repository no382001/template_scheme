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
    using p = decltype(table_entry<A,variable,B>{});
	if constexpr (sizeof...(Args) == 0) {
		return make_environment(p{});
	} else {
		using second = decltype(map_pair_inner(One<Args...>{},Two<Brgs...>{}));
		return make_environment(p{},second{});
	}
}

// map two lists, create a table entry with variable as a tag
// return table_entries with env wrap
template <template <class,class> typename One, typename... Args, template <class,class> typename Two, typename... Brgs>
auto constexpr map_pair(One<Args...>,Two<Brgs...>){
    if constexpr (sizeof...(Args) != sizeof...(Brgs)){
        static_assert(!is_same_type<int,int>,"lists are not the same size");
    } else {
        return map_pair_inner(One<Args...>{},Two<Brgs...>{});
    }
}

// map pair is trying yo map each member of the clist to the members of the list

template <template <int...> typename One, int... A, template <int...> typename Two, int... B>
auto constexpr map_pair(One<A...>,Two<B...>){
    return make_environment(table_entry<One<A...>,variable,Two<B...>>{});
}


// needs refactoring
template <typename Env, typename A, typename... Args>
auto constexpr eval_members(quote<list<A,Args...>>){
    if constexpr (sizeof...(Args) == 0){
        using ev_curr = decltype(IReval<Env>(make_quote(A{})));
        return ev_curr{};
    } else {
        using ev_curr = decltype(IReval<Env>(make_quote(A{})));

        using ev_second = decltype(IReval<Env>(make_quote(make_list(Args{}...))));
        if constexpr (is_same_type<void,ev_curr>){
            return apply_compund_proc<Env>(A{},ev_second{});
        } else {
            return make_list(ev_curr{},ev_second{});
        }
    }
}

// needs refactoring
template <typename Env, typename A, typename... Args>
auto constexpr eval_members(list<A,Args...>){
    if constexpr (sizeof...(Args) == 0){
        using ev_curr = decltype(IReval<Env>(make_quote(A{})));
        return ev_curr{};
    } else {
        using ev_curr = decltype(IReval<Env>(make_quote(A{})));

        using ev_second = decltype(IReval<Env>(make_quote(make_list(Args{}...))));
        // (inc '1) is valid
        
        // if car of list is a procedure, (it also could be an unbound variable but the list serach should have already thrown an error)
        if constexpr (is_same_type<void,ev_curr>){
            return apply_compund_proc<Env>(A{},ev_second{});
        } else {
            return make_list(ev_curr{},ev_second{});
        }
    }
}

template <typename Env, template <int...> typename templated_int, int a>
auto constexpr eval_members(quote<templated_int<a>>){
    return IReval<Env>(quote<templated_int<a>>{});
}

struct apply {};
struct eval {};

IS_SELF_EVALUATING(apply);
IS_SELF_EVALUATING(eval);

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

template <typename arglist, typename Evaluated_opnds>
auto constexpr apply_compund_proc_pair_helper(arglist,Evaluated_opnds){
    if constexpr (is_c_list(arglist{}) || !is_list(arglist{})){
        return make_environment(table_entry<arglist,variable,Evaluated_opnds>{});
    } else {
        return map_pair(arglist{},Evaluated_opnds{});
    }
}

template <typename Env, typename Op, typename Evaluated_opnds >
auto constexpr apply_compund_proc(Op,Evaluated_opnds) {
    // its compound, look var in table
    using comp_proc_entry = decltype(list_search(Op{},Env{}));
    // caddr is arglist
    using arglist = decltype(IRcaddr(comp_proc_entry{}));
    // cadddr is expression
    using expression = decltype(IRcadddr(comp_proc_entry{}));

    // handle pairing differently
    using single_pair = decltype(apply_compund_proc_pair_helper(arglist{},Evaluated_opnds{}));
    // extend env with argument a operand pair
    using temp_ext_env = decltype(extend_environment<init_env>(single_pair{}));
    using result = decltype(IReval<temp_ext_env>(expression{}));

    if constexpr (is_integer_v<result>) { // TODO: add all self evaluating types
        return result{};
    } else {
        using op = decltype(IRcar(result{}));
        using opnds = decltype(IRcdr(result{})); 
        return IRapply(op{},quote<opnds>{});
    }
}

// returns with void if "self evaluating variable not found, or is a procedure"
template <typename Env, typename Exp>
auto constexpr IReval(quote<Exp>) {
    // this is the problem
    if constexpr (is_char_v<Exp> || is_c_list(Exp{})){
        // it is a proc or variable
        // look it up
        using var_res = decltype(list_search(Exp{},Env{}));
        using tag = decltype(IRcadr(var_res{}));

        if constexpr (is_same_type<tag,variable>){
            // return the value of the variable
            return IRcaddr(var_res{});
        } else {
            //static_assert(!is_same_type<int,int>,"self evaluating variable not found, or is a procedure");
            return;
        }


    } else if constexpr (is_self_evaluating(Exp{})){
        return Exp{};
    } else {
        using indicator = decltype(IRcar(Exp{}));
        // if it is an application
        if constexpr (is_same_type<apply,indicator>){
           return IReval<Env>(make_quote(IRcdr(Exp{})));

        } else if constexpr (is_prim_proc(indicator{})){
            using app_operands = decltype(eval_members<Env>(IRcdr(Exp{})));

            if constexpr (is_prim_proc(IRcar(app_operands{}))){

                using evaluated_just_in_case = decltype(IReval<Env>(quote<app_operands>{}));
                using proc_res = decltype(IRapply(indicator{},evaluated_just_in_case{}));
                return proc_res{};
            } else {

                using proc_res = decltype(IRapply(indicator{},quote<app_operands>{}));
                return proc_res{};
            }

        } else if constexpr (is_same_type<scm_if,indicator>){
            using pred = decltype(IRcadr(Exp{}));
            using then = decltype(IRcaddr(Exp{}));
            using el = decltype(IRcadddr(Exp{}));
            
            return if_proc<Env>(pred{},then{},el{});

        } else {
            if constexpr (is_same_type<Exp,list<>>){
                using curr = decltype(IRcar(Exp{}));
                return make_list();
            } else {

                using members = decltype(eval_members<Env>(Exp{}));

                return members{};
            // maybe here if its not a tagged list its a normal one right?s
            }
        }
    }
}

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