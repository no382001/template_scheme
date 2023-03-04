#pragma once
#include "lists.h"
#include "car_cdr.h"
#include "utils.h"
#include "atoms.h"
#include "primitive_operations.h"

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

using init_env = decltype(
    make_environment(table_entry<c_<110>,integer<1>>{},  //));
                    table_entry<inc_c_list,c_<'a'>,quote<list<addition,c_<'a'>,integer<1>>>>{}));


//environment<table_entry<c_<110>, integer<1>>,
//          table_entry<inc_c_list, c_<97>, list<addition, c_<97>, integer<1>>>,
//          table_entry<c_<97>, integer<1>>>