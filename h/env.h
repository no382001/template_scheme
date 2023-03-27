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

template <typename Env, typename Entry>
auto constexpr extend_environment(Entry){
    return Env::append(Entry{});
}

// search a list of lists, where the car of entry list is the identifier
// returns void if entry was not found
// log(n)
template <typename candidate, typename A, typename... Args>
auto constexpr list_search(candidate,environment<A,Args...>){
    using car_of_entry = decltype(IRcar(A{}));
    if constexpr (is_same_type<candidate,car_of_entry>){
        return A{};
	} else if constexpr (sizeof...(Args) > 0) { // if there are still entries left to search
        return list_search(candidate{},environment<Args...>{});
    } else {
        return;
    }
}


// (define (inc a) (+ a 1))
// (inc 1) ; 2

// c_list<c_<105>, c_<110>, c_<99>>
using inc_c_list = decltype(c_list<c_<'i'>,c_<'n'>,c_<'c'>>{});
using sum_of_2 = decltype(c_list<c_<'s'>,c_<'u'>,c_<'m'>>{});

using fib_name = decltype(c_list<c_<'f'>,c_<'i'>,c_<'b'>>{});
using fib_proc_body = decltype(
    quote<list<scm_if,
        quote<list<lesseq,c_<'x'>,integer<2>>>,
        integer<1>,
            list<addition,
                list<fib_name,quote<list<subtraction,c_<'x'>,integer<1>>>>,
                list<fib_name,quote<list<subtraction,c_<'x'>,integer<2>>>>>>>{});
// the problem here is that i should somehow replace the value of x, 
// the deeper i get, the more it decreases, maybe create data with random names?
// its tail recursion, 

// the inner arg should pass the arg rn and the equation

// if before the fib is expanded, all its arguments are evaluated then this should work

using init_env = decltype(
    make_environment(table_entry<c_<110>,variable,integer<1>>{},  //));
                    table_entry<inc_c_list,procedure,c_<'a'>,quote<list<addition,c_<'a'>,integer<1>>>>{},
                    table_entry<sum_of_2,procedure,list<c_<'a'>,c_<'b'>>,quote<list<addition,c_<'a'>,c_<'b'>>>>{},
                    table_entry<fib_name,procedure,c_<'x'>,fib_proc_body>{}));
