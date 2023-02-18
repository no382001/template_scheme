#pragma once
#include "utils.h"
#include "lists.h"

struct scm_false {};
struct scm_true {};

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


template< typename Test, template < int... > class Type >
struct is_templated_int_collection : std::false_type {};

template< template < int... > class Type, int ...Args >
struct is_templated_int_collection< Type< Args... >, Type > : std::true_type {};

template < typename T >
constexpr inline bool is_integer_v = is_templated_int_collection< T, integer >::value;

template <typename T>
constexpr inline bool is_char_v = is_templated_int_collection< T, c_ >::value;