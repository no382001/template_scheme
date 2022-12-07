#pragma once
#include "atoms.h"
#include "utils.h"

// the basic attributes of a list object
#define LIST_BODY(list_type)													\
static constexpr auto append(list_type<>)->list_type< Types... >;				\
template < typename A >															\
static constexpr auto append(A)->list_type< Types..., A >;						\
template < typename A >															\
static constexpr auto append(list_type< A >) { return decltype(append(A{})){}; }\
template < typename A, typename B, typename ...Args >							\
static constexpr auto append(list_type< A, B, Args... >) {						\
	using curr = decltype(list_type::append(A{}));								\
	return decltype(curr::append(list_type< B, Args... >{})){};					\
}

// the basic functions to make a list object
#define MAKE_LIST_FUNCTIONS(list_type)																	\
template < typename T, typename ...Rest>																\
auto constexpr make_##list_type(T, Rest...) -> decltype(list_type< T >::append(list_type< Rest... >{}));\
auto constexpr make_##list_type()->list_type<>;															\
template < typename ...Types >																			\
using make_##list_type##_t = decltype(make_##list_type (Types{}...));									\
template < typename ...Types >																			\
using list_type##_t = decltype(make_##list_type());

// create a fully functional list struct
#define LIST(name)				\
template < typename ...Types >	\
struct name { LIST_BODY(name);};\
MAKE_LIST_FUNCTIONS(name);

LIST(token_list);
LIST(c_list);
LIST(list);