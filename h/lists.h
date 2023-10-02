#pragma once
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
#define MAKE_LIST_FUNCTIONS(list_type)																				\
template < typename T, typename ...Rest>																			\
auto constexpr make_##list_type(T, Rest...) -> decltype(list_type< T >::append(list_type< Rest... >{}));			\
template < typename T, typename ...Rest>																			\
auto constexpr make_##list_type(list_type<T>, Rest...) -> decltype(list_type<T>::append(list_type< Rest... >{}));	\
template <typename ...Rest>																							\
auto constexpr make_##list_type(list_type<>, Rest...) -> decltype(make_##list_type(Rest{}...));						\
auto constexpr make_##list_type()->list_type<>;																		\
template <typename... Args>																							\
auto constexpr make_##list_type(list_type<Args...>) -> decltype(make_##list_type (Args{}...));						\
template < typename ...Types >																						\
using make_##list_type##_t = decltype(make_##list_type (Types{}...));												\
template < typename ...Types >																						\
using list_type##_t = decltype(make_##list_type());																	\
template < typename ...Rest>																						\
auto constexpr make_##list_type(list_type<>)->list_type<>;


#define IS_X_LIST(name)							\
template< typename T>							\
auto constexpr is_##name(T t) {					\
	return false;								\
}												\
template< typename ...Args>						\
auto constexpr is_##name(name<Args...> l) {		\
	return true;								\
}												\
template<>										\
auto constexpr is_##name(name<> l) {			\
	return false;								\
}


// names are just typename wrappers around the list data structure
#define LIST(name)									\
template < typename ...Types >						\
struct name { LIST_BODY(name);};					\
MAKE_LIST_FUNCTIONS(name);                          \
IS_X_LIST(name);

LIST(wrap);

template <typename T>
constexpr inline bool is_wrapped(T){
    return is_same_list_t(T{},wrap<>{});
}

template <typename ...Types>
auto constexpr make_wrap(Types... types){
    static_assert(sizeof...(Types) > 0,"make_wrap has no arguments");
    return wrap<Types...>{};
}


LIST(list);
LIST(c_list);

LIST(token_list);


// janky unreadable but compact recursive replacement of list wrappers list<token_list<...>> -> wrap<list<...>>
template<typename T>
struct replace_nested_list {
    using type = T;
};

// list<token_list<...>> -> wrap<list<...>>
template<typename T> struct replace_nested_list<list<T>> {
    using type = wrap<typename replace_nested_list<T>::type>;
};

// list<token_list<...>> -> wrap<list<...>>
template<typename... Args> struct replace_nested_list<token_list<Args...>> {
    using type = list<typename replace_nested_list<Args>::type...>;
};


template <typename A, typename... Args>
auto constexpr count_list_helper(A,Args...){
    if constexpr (sizeof...(Args) == 0){
        return 0;
    } else {
        return count_list_helper(Args{}...) + 1;
    }
}

template <template <class> typename T, typename... Args>
auto constexpr count_list(T<Args...>){
    return count_list_helper(Args{}...);
}


template <template <class> typename A, typename... Rest>
bool constexpr is_empty_list(A<Rest...>){
	if constexpr (sizeof...(Rest) > 0) {
        return false;
    } else {
        return true;
    }
}