#pragma once
#include "utils.h"

struct list_start {};
struct list_end {};
struct double_quote {};
struct quote {};
struct plus {};
struct minus {};
struct mul {};
struct div_ {};
struct term {};
struct whitespace {};
struct non_integer {};

template <int Value>
struct lower_case {};

template <int Value>
struct upper_case {};

template <int Value>
struct special_character {};

template <int Value>
struct integer
{

	template < int A >
	static constexpr auto merge(integer<A>)->integer< _pow((Value * 10), (_log(10, A) + 1)) + A >; //https://mathworld.wolfram.com/Concatenation.html

	static constexpr auto merge(non_integer)->integer<Value>;

	constexpr operator int() const
	{
		return Value;
	}
};

template <int C>
constexpr auto deduce_token_type()
{
	if constexpr (C == '(')
	{
		return list_start{};
	}
	else if constexpr (C >= '0' && C <= '9')
	{
		return integer<C - '0'>{};
	}
	else if constexpr (C == ')')
	{
		return list_end{};
	}
	else if constexpr (C >= 'a' && C <= 'z')
	{
		return lower_case<C>{};
	}
	else if constexpr (C >= 'A' && C <= 'Z')
	{
		return upper_case< C >{};
	}
	else if constexpr (C == '"')
	{
		return double_quote{};
	}
	else if constexpr (C == '\'')
	{
		return quote{};
	}
	else if constexpr (C == '+')
	{
		return plus{};
	}
	else if constexpr (C == '-')
	{
		return minus{};
	}
	else if constexpr (C == '*')
	{
		return mul{};
	}
	else if constexpr (C == '/')
	{
		return div_{};
	}
	else if constexpr (C == ' ')
	{
		return whitespace{};
	}
}
// --------------------------------------------- INTEGER START

template< typename Test, template < int... > class Type >
struct is_templated_int_collection : std::false_type {};

template< template < int... > class Type, int ...Args >
struct is_templated_int_collection< Type< Args... >, Type > : std::true_type {};

template < typename T >
constexpr inline bool is_integer_v = is_templated_int_collection< T, integer >::value;

template < int Index, typename Lambda >
constexpr auto find_first_non_integer(Lambda lambda)
{
	constexpr auto str = lambda();
	using type = decltype(deduce_token_type< str[Index] >());
	if constexpr (!is_integer_v<type>)
	{
		return Index;
	}
	else
	{
		return find_first_non_integer< Index + 1 >(lambda);
	}
}

template < int Start, int End, typename Lambda >
constexpr auto make_integer(Lambda str_lambda)
{
	constexpr auto str = str_lambda();
	if constexpr (Start < End)
	{
		constexpr auto value = str[Start] - '0';
		return integer< value >::merge(make_integer< Start + 1, End >(str_lambda));
	}
	else {
		return non_integer{};
	}
}



// --------------------------------------------- INTEGER END