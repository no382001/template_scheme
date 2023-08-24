#pragma once
#include "utils.h"
#include "lists.h"
/**
 * \brief recursive power function in constexpr
 * \param[in] base base
 * \param[in] exp exponent
 * \param[in] result the accumulator
 * \return returns the power of result to the exp on base
 */
constexpr int _pow(int base, int exp, int result = 1) {
	return exp < 1 ? result : _pow(base * base, exp / 2, (exp % 2) ? result * base : result);
}
/**
 * \brief recursive log function in constexpr
 * \param[in] b base
 * \param[in] n the accumulator
 * \return returns the log of n on base b
 */
constexpr int _log(int b, int n) {
	return n < b ? 0 : _log(b, n / b) + 1;
}


/** \brief false type */
struct scm_false {};
/** \brief true type */
struct scm_true {};


/** \brief single character type */
template <int Value>
struct c_ {};

/** \brief placeholder for special character */
template <int Value>
struct special_character {};

/** \brief placeholder for non integer */
struct non_integer {};

/** \brief integer type, is able to concatinate with other integers */
template <int Value>
struct integer{
	template < int A >
	static constexpr auto merge(integer<A>)->integer< _pow((Value * 10), (_log(10, A) + 1)) + A >; // int concatenation
	static constexpr auto merge(non_integer)->integer<Value>;
	auto constexpr get_value() {return Value;};
};



/** \brief no match for templated int */
template< typename Test, template < int... > class Type >
struct is_specific_templated_int_collection : std::false_type {};

/** \brief match for templated int */
template< template < int... > class Type, int ...Args >
struct is_specific_templated_int_collection< Type< Args... >, Type > : std::true_type {};

/** \brief helper value function for templated int */
template < typename T >
constexpr inline bool is_integer_v = is_specific_templated_int_collection< T, integer >::value;

/** \brief helper value function for char */
template <typename T>
constexpr inline bool is_char_v = is_specific_templated_int_collection< T, c_ >::value;


template <typename A, typename B>
struct is_same_type_impl {
	static constexpr auto value = std::is_same<A, B>::value;
}; 

template <template <int...> class Type, int... Args>
struct is_same_type_impl<Type<Args...>, Type<Args...>> : std::true_type {};

/** \brief spec for lists */
template <typename A, typename B>
constexpr inline bool is_same_type = is_same_type_impl<A, B>::value;

static_assert(is_same_type<bool,bool>,"");
static_assert(!is_same_type<integer<1>,bool>,"");
static_assert(!is_same_type<bool,integer<1>>,"");
static_assert(is_same_type<integer<1>,integer<1>>,"");
static_assert(!is_same_type<integer<1>,c_<1>>,"");

/** \brief determines if its self evaluating */
template <typename T>
auto constexpr is_self_evaluating(T) { 
	if constexpr (is_integer_v<T> || is_char_v<T>){
		return true;
	} else {
		return false;
	}
}
/** \brief macro for self evaluation "self-test" */
#define IS_SELF_EVALUATING(name)							\
template <>													\
auto constexpr is_self_evaluating(name) { return true; }


