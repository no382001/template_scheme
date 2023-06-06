#pragma once
#include "utils.h"
#include "lists.h"


constexpr int _pow(int base, int exp, int result = 1) {
	return exp < 1 ? result : _pow(base * base, exp / 2, (exp % 2) ? result * base : result);
}

constexpr int _log(int b, int n) {
	return n < b ? 0 : _log(b, n / b) + 1;
}


struct scm_false {};
struct scm_true {};

template <int Value>
struct c_ {};

template <int Value>
struct special_character {};

struct non_integer {};

template <int Value>
struct integer{
	template < int A >
	static constexpr auto merge(integer<A>)->integer< _pow((Value * 10), (_log(10, A) + 1)) + A >; // int concatenation
	static constexpr auto merge(non_integer)->integer<Value>;
	auto constexpr get_value() {return Value;};
};



// overload for specifics
template< typename Test, template < int... > class Type >
struct is_specific_templated_int_collection : std::false_type {};

template< template < int... > class Type, int ...Args >
struct is_specific_templated_int_collection< Type< Args... >, Type > : std::true_type {};

template < typename T >
constexpr inline bool is_integer_v = is_specific_templated_int_collection< T, integer >::value;

template <typename T>
constexpr inline bool is_char_v = is_specific_templated_int_collection< T, c_ >::value;


template <typename A, typename B>
struct is_same_type_impl {
	static constexpr auto value = std::is_same<A, B>::value;
}; 

template <template <int...> class Type, int... Args>
struct is_same_type_impl<Type<Args...>, Type<Args...>> : std::true_type {};

template <typename A, typename B>
constexpr inline bool is_same_type = is_same_type_impl<A, B>::value;

static_assert(is_same_type<bool,bool>,"");
static_assert(!is_same_type<integer<1>,bool>,"");
static_assert(!is_same_type<bool,integer<1>>,"");
static_assert(is_same_type<integer<1>,integer<1>>,"");
static_assert(!is_same_type<integer<1>,c_<1>>,"");

template <typename T>
auto constexpr is_self_evaluating(T) { 
	if constexpr (is_integer_v<T> || is_char_v<T>){
		return true;
	} else {
		return false;
	}
}

#define IS_SELF_EVALUATING(name)							\
template <>													\
auto constexpr is_self_evaluating(name) { return true; }