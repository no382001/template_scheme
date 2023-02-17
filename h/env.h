#pragma once
#include "lists.h"
#include "car_cdr.h"
#include "utils.h"
#include "atoms.h"

// -----------------------------------------------------------------------------------------------------
// setting up an init-env for eval

// environment table is a list of lists, or pairs
LIST(environment);

template <typename... columns>
struct table_entry {};

template <typename Env, typename Entry>
auto constexpr extend_environment(Entry){
    return Env::append(make_environment(Entry{}));
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

