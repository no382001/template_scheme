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
auto constexpr IReval(wrap<Exp>);


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
        static_assert(DELAYED_FALSE,"lists are not the same size");
    } else {
        return map_pair_inner(One<Args...>{},Two<Brgs...>{});
    }
}

template <template <int...> typename One, int... A, template <int...> typename Two, int... B>
auto constexpr map_pair(One<A...>,Two<B...>){
    return make_environment(table_entry<One<A...>,variable,Two<B...>>{});
}

template <typename Env, typename A, typename... Args>
auto constexpr eval_members(list<A,Args...>); // fwd dclr

template <typename Env, typename A, typename... Args>
auto constexpr eval_members(wrap<list<A,Args...>>){
    return eval_members<Env>(list<A,Args...>{});
}

// this helped solve a problem with the c_list bug
template <typename A>
auto constexpr define_var_name_helper_char(A){
    if constexpr (is_char_v<A> || is_c_list(A{})){
        return A{};
    } else {
        return IRcar(IRcar(A{}));
    }
}

template <typename A>
auto constexpr define_var_name_helper_integer(A){
    if constexpr (is_integer_v<A>){
        return A{};
    } else {
        return IRcar(A{});
    }
}

// really stupid, instead of car but whatever
template <typename A,typename B,typename... Params>
auto constexpr define_arg_number_helper(wrap<list<A,B,Params...>>){
    if constexpr (sizeof...(Params) == 0){
        return B{};
    } else {
        return list<B,Params...>{};                
    }
}

template <typename Env, typename A, typename... Args>
auto constexpr eval_members(list<A,Args...>){
    
    using ev_curr = decltype(IReval<Env>(make_wrap(A{})));
    
    if constexpr (sizeof...(Args) == 0){
        return ev_curr{};
    } else {
        if constexpr (is_same_type<ev_curr,scm_define>) {
            return define_tag{};
        } else if constexpr (is_same_type<ev_curr,define_tag>){
            // handle define and extend env here
            
            using without_wrapper = decltype(IRcar(A{}));
            using params = decltype(IRcadr(without_wrapper{}));
            using body = decltype(IRcddr(without_wrapper{}));
            using extracted_body = decltype(define_var_name_helper_integer(body{}));

            // if there is no argument, only a name, then its a variable, otherwise a procedure
            using name = decltype(define_var_name_helper_char(params{}));
            
            if constexpr (is_c_list(params{}) || is_char_v<params>){ // variable
                
                // whenever a variable gets replaced the expression does not evaluate but waits for this layer to do that
                // the procedure has apply and is collapsed to the result
                // so evaluate again, just to be sure

                using entry = table_entry<name,variable,extracted_body>;
                using extended_environment = decltype(extend_environment<Env>(entry{}));
                
                using return_value = decltype(eval_members<extended_environment>(make_wrap(make_list(Args{}...))));
                return IReval<Env>(make_wrap(return_value{}));

            } else { // procedure
                using arguments = decltype(define_arg_number_helper(params{}));
                using entry = table_entry<name,procedure,arguments,extracted_body>;

                using extended_environment = decltype(extend_environment<Env>(entry{}));
                return eval_members<extended_environment>(make_wrap(make_list(Args{}...)));
                
                // maybe i should return a result line by line? could be good for debugging
            }

        } else {
            using ev_curr = decltype(IReval<Env>(make_wrap(A{})));

            using ev_second = decltype(IReval<Env>(make_wrap(make_list(Args{}...))));
            // (inc '1) is valid
            
            // if car of list is a procedure, (it also could be an unbound variable but the list serach should have already thrown an error)
            if constexpr (is_same_type<void,ev_curr>){
                return apply_compund_proc<Env>(A{},ev_second{});
            } else {
                return make_list(ev_curr{},ev_second{});
            }
        } 
    }
}

template <typename Env, template <int...> typename templated_int, int a>
auto constexpr eval_members(wrap<templated_int<a>>){
    return IReval<Env>(wrap<templated_int<a>>{});
}

struct apply {};
struct eval {};

IS_SELF_EVALUATING(apply);
IS_SELF_EVALUATING(eval);

template < typename Proc, typename Args >
auto constexpr IRapply(Proc,wrap<Args>) {

    using evaluated_operands = decltype(IReval<init_env>(wrap<Args>{}));
	using prim_proc = decltype(apply_primitve_procedure(Proc{},evaluated_operands{}));
    
    // cant instantiate void, so:
    if constexpr (is_same_type<prim_proc,void>){
        return;
    } else {
        return prim_proc{};
    }
}

// if its a one to one scenario just make the env, bc they are not lists they dont fall under map
template <typename arglist, typename Evaluated_opnds>
auto constexpr apply_compund_proc_pair_helper(arglist,Evaluated_opnds){
    if constexpr (is_c_list(arglist{}) || !is_list(arglist{})){
        return make_environment(table_entry<arglist,variable,Evaluated_opnds>{});
    } else {
        return map_pair(arglist{},Evaluated_opnds{});
    }
}

// during recursive replacement, the opnds dont actually get evaluated just replaced
// so this fixes that
template <typename Env, typename Evaluated_opnds>
auto constexpr recursion_helper(Evaluated_opnds){
    if constexpr (!is_self_evaluating(Evaluated_opnds{})){
        if constexpr (is_prim_proc(IRcar(Evaluated_opnds{}))){
            return IReval<Env>(wrap<Evaluated_opnds>{});
        } else {
            return Evaluated_opnds{};
        }
    } else {
        return Evaluated_opnds{};
    }
}


template <typename Env, typename Op, typename Evaluated_opnds >
auto constexpr apply_compund_proc(Op,Evaluated_opnds) {

    // its compound, look var in table
    using comp_proc_entry = decltype(list_search(Op{},Env{}));
    // caddr is arglist
    using arglist = decltype(IRcaddr(comp_proc_entry{}));
    
    using helped_arglist = decltype(recursion_helper<Env>(arglist{}));
    // cadddr is expression
    using expression = decltype(IRcadddr(comp_proc_entry{}));

    using evald_opnds = decltype(recursion_helper<Env>(Evaluated_opnds{}));
    // handle pairing differently
    using single_pair = decltype(apply_compund_proc_pair_helper(helped_arglist{},evald_opnds{}));
    // extend env with argument a operand pair
    using temp_ext_env = decltype(extend_environment<init_env>(single_pair{}));
    using result = decltype(IReval<temp_ext_env>(expression{}));

    if constexpr (is_integer_v<result>) { // TODO: add all self evaluating types
        return result{};
    } else {
        using op = decltype(IRcar(result{}));
        using opnds = decltype(IRcdr(result{})); 
        return IRapply(op{},wrap<opnds>{});
    }
}

// returns with void if "self evaluating variable not found, or is a procedure"
template <typename Env, typename Exp>
auto constexpr IReval(wrap<Exp>) {

    // if it is a proc or variable
    if constexpr (is_char_v<Exp> || is_c_list(Exp{})){
        // look it up in env
        using var_res = decltype(list_search(Exp{},Env{}));
        // get the type of expression
        using tag = decltype(IRcadr(var_res{}));

        if constexpr (is_same_type<tag,variable>){
            // return the value of the variable
            return IRcaddr(var_res{});
        } else {
            //static_assert(DELAYED_FALSE,"self evaluating variable not found, or is a procedure");
            return;
        }

    } else if constexpr (is_self_evaluating(Exp{})){
        return Exp{};
    } else if constexpr (is_same_type<Exp,define_tag>) { // never hit
        static_assert(DELAYED_FALSE,""); 
        return make_list();
    } else {
        using indicator = decltype(IRcar(Exp{}));
        // if it is an application
        if constexpr (is_same_type<apply,indicator>){
           return IReval<Env>(make_wrap(IRcdr(Exp{})));

        } else if constexpr (is_prim_proc(indicator{})){
            using app_operands = decltype(eval_members<Env>(IRcdr(Exp{})));

            if constexpr (!is_self_evaluating(app_operands{})){
                if constexpr (is_prim_proc(IRcar(app_operands{}))){

                    using evaluated_just_in_case = decltype(IReval<Env>(wrap<app_operands>{}));
                    using proc_res = decltype(IRapply(indicator{},wrap<evaluated_just_in_case>{}));
                    return proc_res{};
                } else {

                    using proc_res = decltype(IRapply(indicator{},wrap<app_operands>{}));
                    return proc_res{};
                }
            } else {
                using proc_res = decltype(IRapply(indicator{},wrap<app_operands>{}));
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
                // maybe here if its not a tagged list its a normal one right?
            }
        }
    }
}

// (list-of-values exps env)
template <typename Env, typename A, typename... Args>
auto constexpr list_of_values(A,Args...){
    if constexpr (sizeof...(Args) == 0){
        using only_operand = decltype(IReval<Env>(wrap<A>{}));
        return only_operand{};
    } else {
        using first_operand = decltype(IReval<Env>(wrap<A>{}));
        using second = decltype(list_of_values(Args{}...));
        return make_list(first_operand{},second{});
    }
}