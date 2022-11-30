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