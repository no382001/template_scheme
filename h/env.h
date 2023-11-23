#pragma once
#include "lists.h"
#include "car_cdr.h"
#include "utils.h"
#include "atoms.h"
#include "primitive_operations.h"

// self evaluating symbol
struct variable {};
// takes atleast one argument, applied
struct procedure {};

// -----------------------------------------------------------------------------------------------------
// setting up an init-env for eval

// environment table is a list of lists, or pairs
LIST(environment);

template <typename... columns>
struct table_entry {};
IS_X_LIST(table_entry);

// push back
template <typename Env, typename Entry>
auto constexpr extend_environment(Entry){
    return Env::append(Entry{});
}


// push front
template <typename Env, typename Entry>
auto constexpr extend_environment_front(Entry){
    return Entry::append(Env{});
}

// search a list of lists, where the car of entry list is the identifier
// returns void if entry was not found
template <typename candidate,template <class,class> typename T, typename A, typename... Args>
auto constexpr list_search(candidate,T<A,Args...>){

    using car_of_entry = decltype(IRcar(A{}));

    if constexpr (is_same_type<candidate,car_of_entry>){
        return A{};
	} else if constexpr (sizeof...(Args) > 0) { // if there are still entries left to search
        return list_search(candidate{},T<Args...>{});
    } else {
        //static_assert(DELAYED_FALSE,"not found");
        return;
    }
}


// (define (inc a) (+ a 1))
// (inc 1) ; 2

// c_list<c_<105>, c_<110>, c_<99>>
using inc_c_list = decltype(c_list<c_<'i'>,c_<'n'>,c_<'c'>>{});
using sum_of_2 = decltype(c_list<c_<'s'>,c_<'u'>,c_<'m'>>{});
using sub_c_list = decltype(c_list<c_<'s'>,c_<'u'>,c_<'b'>>{});

using fib_name = decltype(c_list<c_<'f'>,c_<'i'>,c_<'b'>>{});
using fib_proc_body = decltype(
    wrap<list<scm_if,
        wrap<list<lesseq,c_<'x'>,integer<2>>>,
        integer<1>,
            list<addition,
                list<fib_name,wrap<list<subtraction,c_<'x'>,integer<1>>>>,
                list<fib_name,wrap<list<subtraction,c_<'x'>,integer<2>>>>>>>{});

using init_env = decltype(
    make_environment(table_entry<c_<110>,variable,integer<1>>{},  //));
                    table_entry<inc_c_list,procedure,c_<'a'>,wrap<list<addition,c_<'a'>,integer<1>>>>{},
                    table_entry<sub_c_list,procedure,c_<'a'>,wrap<list<subtraction,c_<'a'>,integer<1>>>>{},
                    table_entry<sum_of_2,procedure,list<c_<'a'>,c_<'b'>>,wrap<list<addition,c_<'a'>,c_<'b'>>>>{},
                    table_entry<fib_name,procedure,c_<'x'>,fib_proc_body>{}));


// if it does not have an argument then its a variable, basically