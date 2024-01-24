#pragma once

#include "lists.h"
#include "car_cdr.h"
#include "utils.h"
#include "atoms.h"
#include "primitive_operations.h"
#include "env.h"

template <typename Env, typename Op, typename Evaluated_opnds >
auto constexpr apply_compund_proc(Op,Evaluated_opnds);

// fwd declare, bc recursively included
template <typename Env, typename A, typename... Args>
auto constexpr list_of_values(A,Args...);

template <typename Env, typename Exp>
auto constexpr IReval(wrap<Exp>);

template <typename Env, typename Proc, typename Args >
auto constexpr IRapply(Proc,wrap<Args>);

// temp location for map_pair
CRTP_LIST(pair);


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

// this helped solve a problem with the c_list bug (which?)
template <typename A>
auto constexpr define_var_name_helper_char(A){
    if constexpr (is_char_v<A> || is_c_list(A{})){
        return A{};
    } else {
        return IRcar(IRcar(A{}));
    }
}

/** / // define is broken so use an extra expression when doing more than one thing in the body()
// list<
//    wrap<list<scm_define, int, integer<2>>>,
//    wrap<list<addition, int, int>>> 
template <typename A,typename... Args> // in case of define under the scope of another
auto constexpr define_var_name_helper_integer(list<A,Args...>){
    return list<A,Args...>{};
}
/**/


template <typename A>
auto constexpr define_var_name_helper_integer(A){
// probably pass the operands so it can evaluate in the context of the define, it its a define_tag
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

// -- very ugly instantiation delay, i did this somewhere already but in a different way
template <typename T>
struct delayed_application_helper;

// if ev_curr is scm_apply
template <>
struct delayed_application_helper<scm_apply> {
    template <typename Env, typename A,typename... Args>
    static auto apply(A,quote,wrap<list<Args...>>) {
        return IRapply<Env>(A{},wrap<list<Args...>>{});
    }
    template <typename Env, typename A,typename... Args>
    static auto apply(A,wrap<list<Args...>>) {
        static_assert(DELAYED_FALSE,"(apply procedure '(list ...)) QUOTE IS MISSING FROM LIST");
    }
};

// if ev_curr is anything else
template <typename T>
struct delayed_application_helper {
    template <typename Env,typename... Args>
    static auto apply(Args... args) {
        return;
    }
};

template <typename Env, typename T, typename... Args>
auto constexpr delayed_application(Args... args) {
    return delayed_application_helper<T>::template apply<Env>(args...);
    // wth? `template indicates a template method`
}
// -- 

template <typename Env, typename... Args>
auto constexpr scm_eval_helper(quote,wrap<list<Args...>>) {
    return IReval<Env>(wrap<list<Args...>>{});
}

template <typename Env, typename... Args>
auto constexpr scm_eval_helper(wrap<list<Args...>>) {
    static_assert(DELAYED_FALSE,"(eval '(list ...)) QUOTE IS MISSING FROM LIST");
}

template <typename Env, typename A, typename... Args>
auto constexpr eval_members(list<A,Args...>);

// this should be extended for more than one list<...>
// possibly to wrap<list<...>,...> in which case the 2nd ... could be a list or not
// but in any case, the last procedure is the return thing
// without it having to be wrapped in something else like, right now.
template <typename Env, typename A, typename... Args>
auto constexpr eval_define_impl(list<A,Args...>){
    // handle define and extend env here
            
    using without_wrapper = decltype(IRcar(A{}));
    using params = decltype(IRcadr(without_wrapper{}));
    using body = decltype(IRcddr(without_wrapper{}));
    using extracted_body = decltype(define_var_name_helper_integer(body{}));

    // if there is no argument, only a name, then its a variable, otherwise a procedure
    using name = decltype(define_var_name_helper_char(params{}));

    if constexpr (is_c_list(params{}) || is_char_v<params>){ // variable

        // whenever a variable gets replaced the expression gets passed back to this layer
        // the procedure will have apply and is collapsed as the result
        // so we need evaluate again, just to be sure

        using entry = table_entry<name,variable,extracted_body>;
        using extended_environment = decltype(extend_environment<Env>(entry{}));
        
        using return_value = decltype(eval_members<extended_environment>(make_wrap(make_list(Args{}...))));
        return IReval<Env>(make_wrap(return_value{}));

    } else { // procedure
        using arguments = decltype(define_arg_number_helper(params{}));
        using entry = table_entry<name,procedure,arguments,extracted_body>;

        using extended_environment = decltype(extend_environment<Env>(entry{}));
        return eval_members<extended_environment>(make_wrap(make_list(Args{}...)));
        //return IReval<extended_environment>(make_wrap(return_value{}));
        // this goes void bc operation cant be made on define_tag?
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
        } else if constexpr (is_same_type<ev_curr,scm_cons>){
            if constexpr (sizeof...(Args) != 2){
                static_assert(DELAYED_FALSE,"cons called with more or less than 2 args");
            } else {
                return make_cons<Env>(Args{}...);
            }
        } else if constexpr (is_same_type<ev_curr,scm_list>){
            return eval_every_member_for_cons_list<Env>(Args{}...);
        } else if constexpr (is_same_type<ev_curr,scm_car>){
            if constexpr (sizeof...(Args) == 1){
                using second = decltype(IReval<Env>(make_wrap(Args{}...)));
                return car(second{});
            } else {
                static_assert(DELAYED_FALSE,"car applied with more or less than 1 arg");
            }
        } else if constexpr (is_same_type<ev_curr,scm_cdr>){
            if constexpr (sizeof...(Args) == 1){
                using second = decltype(IReval<Env>(make_wrap(Args{}...)));
                return cdr(second{});
            } else {
                static_assert(DELAYED_FALSE,"cdr applied with less or more than 1 args");
            }
        } else if constexpr (is_same_type<ev_curr,define_tag>){
            // the tag returns so i can catch it here
            // i should also check if there are nested defines here, shouldnt i
            // i could enforce a `begin` type thing, where the last procedure
            // should always return something that is not a definiton
            return eval_define_impl<Env>(list<A,Args...>{});
            // it should apply itself to each next member until it returns
            // so each member really should be evaluated and then return here to extend_env
            // and do this until i can return without a define_tag
            // -- i could enforce, procedures to always end in returns,
            //    and that no define body can exist without a return
        } else {
            using ev_curr = decltype(IReval<Env>(make_wrap(A{})));
            
            // if ev_curr is an explicit call to apply
            using delayed_app_res = decltype(delayed_application<Env,ev_curr>(Args{}...));

            if constexpr (!is_same_type<void,delayed_app_res>){
                return delayed_app_res{};
            } else if constexpr (is_same_type<ev_curr,scm_eval>){
                // strip the quote and eval
                return scm_eval_helper<Env>(Args{}...);
            } else if constexpr (is_same_type<ev_curr,quote>){
                // return the body
                return make_wrap(Args{}...);
            } else {
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
}

template <typename Env, template <int...> typename templated_int, int a>
auto constexpr eval_members(wrap<templated_int<a>>){
    return IReval<Env>(wrap<templated_int<a>>{});
}



template <typename Env, typename Proc, typename Args >
auto constexpr IRapply(Proc,wrap<Args>) {

    using evaluated_operands = decltype(IReval<Env>(wrap<Args>{}));
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

    using temp_ext_env = decltype(extend_environment_front<Env>(single_pair{}));
    using result = decltype(IReval<temp_ext_env>(expression{}));

    if constexpr (is_integer_v<result>) { // TODO: add all self evaluating types
        return result{};
    } else {
        using op = decltype(IRcar(result{}));
        using opnds = decltype(IRcdr(result{})); 
        return IRapply<Env>(op{},wrap<opnds>{});
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
                    using proc_res = decltype(IRapply<Env>(indicator{},wrap<evaluated_just_in_case>{}));
                    return proc_res{};
                } else {

                    using proc_res = decltype(IRapply<Env>(indicator{},wrap<app_operands>{}));
                    return proc_res{};
                }
            } else {
                using proc_res = decltype(IRapply<Env>(indicator{},wrap<app_operands>{}));
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
        using second = decltype(list_of_values<Env>(Args{}...));
        return make_list(first_operand{},second{});
    }
}