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

// environment table is a list of lists, or pairs
CRTP_LIST(environment);

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