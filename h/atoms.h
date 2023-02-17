#pragma once
#include "utils.h"
#include "lists.h"

struct list_start {};
struct list_end {};
struct double_quote {};
struct plus {};
struct minus {};
struct mul {};
struct div_ {};
struct term {};
struct whitespace {};
struct non_integer {};
struct non_character {};

struct equal {};
struct less {};
struct more {};


template <int Value>
struct c_ {};

template <int Value>
struct special_character {};

template <int Value>
struct integer{
	//template < int A >
	//static constexpr auto merge(integer<A>)->integer< _pow((Value * 10), (_log(10, A) + 1)) + A >; // int concatenation
	//static constexpr auto merge(non_integer)->integer<Value>;
	auto constexpr get_value() {return Value;};
};


