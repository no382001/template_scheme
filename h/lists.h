#pragma once
#include "atoms.h"
#include "utils.h"




// the basic attributes of a list object
#define LIST_BODY(list_type)													\
static constexpr auto append(list_type<>)->list_type< Types... >;				\
static constexpr auto append(list_type<>,list_type<>)->list_type< Types... >;	\
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
template < typename T, typename ...Rest>																\
auto constexpr make_##list_type(list_type<T>, Rest...) -> decltype(list_type<T>::append(list_type< Rest... >{}));\
auto constexpr make_##list_type()->list_type<>;															\
template <typename... Args>																				\
auto constexpr make_##list_type(list_type<Args...>) -> decltype(make_##list_type (Args{}...));			\
template < typename ...Types >																			\
using make_##list_type##_t = decltype(make_##list_type (Types{}...));									\
template < typename ...Types >																			\
using list_type##_t = decltype(make_##list_type());														\
template < typename ...Rest>																			\
auto constexpr make_##list_type(list_type<>)->list_type<>;

// lists have all the same functionality, names are just typename wrappers around the list data structure
#define LIST(name)									\
template < typename ...Types >						\
struct name { LIST_BODY(name);};					\
MAKE_LIST_FUNCTIONS(name);

LIST(token_list);
LIST(c_list);
LIST(list);

using test1 = decltype(make_list());
using test2 = decltype(make_list(make_list()));
using test3 = decltype(make_list(make_list(int{})));

using test23 = decltype(make_list(make_list(int{}),make_list()));

template <template <class> typename A, template <class> typename B, typename... First, typename... Second>
bool constexpr is_same_list_t(A<First...> a,B<Second...> b){ 
	using derivedA = decltype(A{});
	using derivedB = decltype(B{});
	
	if constexpr (is_same_type<derivedA,derivedB>){
		return true;
	} else {
		return false;
	}
}

static_assert( false == is_same_list_t(list<int>{},token_list<>{}));
static_assert( true == is_same_list_t(list<int>{},list<>{}));

template <template <class> typename A, typename... Args>
auto constexpr simplify(A<A<Args...>>){
	return A<Args...>{};
}
template <typename A>
auto constexpr simplify(A){
	return A{};
}

template <template <class> typename A, template <class> typename B, typename... Args>
auto constexpr remove_outer(A<B<Args...>>){
	return B<Args...>{};
}


template <template <class> typename A, typename... Rest>
bool constexpr is_empty_list(A<Rest...>){
	if constexpr (sizeof...(Rest) > 0) {
        return false;
    } else {
        return true;
    }
}

using test4 = decltype(make_list(list<char>{},list<>{},list<char>{},list<char>{})); // collapse