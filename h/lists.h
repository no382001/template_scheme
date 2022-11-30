#pragma once
#include "atoms.h"
#include "utils.h"

template < typename ...Types >
struct token_list
{
	static constexpr auto append(token_list<>)->token_list< Types... >;

	// append A to the end
	template < typename A >
	static constexpr auto append(A)->token_list< Types..., A >;
	template < typename A >
	static constexpr auto append(token_list< A >)
	{
		return decltype(append(A{})){};
	}

	template < typename A, typename B, typename ...Args >
	static constexpr auto append(token_list< A, B, Args... >)
	{
		using curr = decltype(token_list::append(A{}));
		return decltype(curr::append(token_list< B, Args... >{})){};
	}
};

template < typename T, typename ...Rest>
auto constexpr make_token_list(T, Rest...) -> decltype(token_list< T >::append(token_list< Rest... >{}));

auto constexpr make_token_list()->token_list<>;

template < typename ...Types >
using make_token_list_t = decltype(make_token_list(Types{}...));

template < typename ...Types >
using token_list_t = decltype(make_token_list());


template < typename ...Types >
struct c_list
{
	static constexpr auto append(c_list<>)->c_list< Types... >;

	// append A to the end
	template < typename A >
	static constexpr auto append(A)->c_list< Types..., A >;

	template < typename A >
	static constexpr auto append(c_list< A >)
	{
		return decltype(append(A{})){};
	}

	template < typename A, typename B, typename ...Args >
	static constexpr auto append(c_list< A, B, Args... >)
	{
		using curr = decltype(c_list::append(A{}));
		return decltype(curr::append(c_list< B, Args... >{})){};
	}
};

template < typename T, typename ...Rest>
auto constexpr make_c_list(T, Rest...) -> decltype(c_list< T >::append(c_list< Rest... >{}));

auto constexpr make_c_list()->c_list<>;

template < typename ...Types >
struct list
{

	static constexpr auto append(list<>)->list< Types... >;

	// append A to the end
	template < typename A >
	static constexpr auto append(A)->list< Types..., A >;

	template < typename A >
	static constexpr auto append(list< A >)
	{
		return decltype(append(A{})){};
	}

	template < typename A, typename B, typename ...Args >
	static constexpr auto append(list< A, B, Args... >)
	{
		using curr = decltype(list::append(A{}));
		return decltype(curr::append(list< B, Args... >{})){};
	}

};

template < typename T, typename ...Rest>
auto constexpr make_list(T, Rest...) -> decltype(list< T >::append(list< Rest... >{}));

auto constexpr make_list()->list<>;