#pragma once
#include "atoms.h"
#include "parser.h"
#include "lists.h"

#define _if c_list<c_<105>, c_<102>>

template <typename Condition, typename Then, typename Else>
auto constexpr e_if(Condition, Then, Else){
    auto constexpr cond_res = parse(token_list<Condition>{});
    if constexpr (cond_res > 0){
        return parse(token_list<Then>{});
    } else {
        return parse(token_list<Else>{});
    }
}