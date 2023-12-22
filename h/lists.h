#pragma once
#include "utils.h"

// REFACTOR, this looks horrible



// specifies append behaviour to a ListType
template <template <typename...> class ListType, typename... Content>
struct ListBehavior {
    static constexpr auto append(ListType<>)
      -> ListType< Content... >;
    static constexpr auto append(ListType<>,ListType<>)
      -> ListType< Content... >;
    
    template < typename A >
    static constexpr auto append(A)
      -> ListType< Content..., A >;
    
    template < typename A > // merge list<...> with list<1>
    static constexpr auto append(ListType< A >)
      -> decltype(ListBehavior::append(A{}));
    
    template < typename A, typename B, typename ...Args >
    static constexpr auto append(ListType< A, B, Args... >) {
      using next = decltype(ListBehavior::append(A{}));
      return next::append(ListType< B, Args... >{});
    }
};

template <typename... Args>
struct KindOfList : ListBehavior<KindOfList,Args...>{};

using ssssdasda = decltype(KindOfList<int,int>::append(int{}));
static_assert(is_same_type<ssssdasda,KindOfList<int,int,int>>);


// generatates a ListType with the rules of `make`
template <template <typename...> class ListType>
struct ListOf {

  // base
  template <typename X >
  static auto constexpr make(X) -> ListType<X>;
  
  // rec
  template < typename X, typename ...Xs>
  auto constexpr make(X, Xs...)
    -> decltype(ListType< X >::append(ListType< Xs... >{}));

  // list<1> w/ ... elements
  template <typename X, typename... Ys> 
  static auto constexpr make(ListType< X >, Ys...)
    -> decltype(ListType< X >::append(ListType< Ys... >{}));

  // list<> w/ ... elements
  template <typename... Xs>
  static auto constexpr make(ListType<>, Xs...)
    -> decltype(make(Xs{}...));
  
  // void
  static auto constexpr make() -> ListType<>;

  // empty ListType
  static auto constexpr make(ListType<>) -> ListType<>;
  
  // i might need these for whatever reason
  template <typename... Xs>
  using make_t = decltype(make(Xs{}...));
  using list_type_t = decltype(make());

};



/** /
namespace ListRefactor {
  
  template < typename T, typename ...Rest>
  auto constexpr make_list(T, Rest...)
    -> decltype(list< T >::append(list< Rest... >{}));

  template < typename T, typename ...Rest>
  auto constexpr make_list(list<T>, Rest...) //
    -> decltype(list<T>::append(list< Rest... >{}));

  template <typename ...Rest>
  auto constexpr make_list(list<>, Rest...) //
    -> decltype(make_list(Rest{}...));
  auto constexpr make_list() -> list<>; //

  template < typename ...Types >
  using make_list_t = decltype(make_list (Types{}...));
  template < typename ...Types >
  using list_type_t = decltype(make_list());

  template < typename ...Rest>
  auto constexpr make_list(list<>)->list<>; //

  using normal_make = decltype(ListRefactor::make_list(int{},int{},int{}));
  using factory_make = decltype(ListOf<list>::make(int{},int{},int{}));
  static_assert(is_same_type<normal_make,factory_make>,"");
  
  using normal_l1_l_make = decltype(ListRefactor::make_list(list<int>{},int{},int{}));
  using factory_l1_l_make = decltype(ListOf<list>::make(list<int>{},int{},int{}));
  static_assert(is_same_type<normal_l1_l_make,factory_l1_l_make>,"");


  using normal_empty_w_elem_make = decltype(ListRefactor::make_list(list<>{},int{},int{}));
  using factory_empty_w_elem_make = decltype(ListOf<list>::make(list<>{},int{},int{}));
  static_assert(is_same_type<normal_empty_w_elem_make,factory_empty_w_elem_make>,"");

  using normal_voi_make = decltype(ListRefactor::make_list());
  using factory_voi_make = decltype(ListOf<list>::make());
  static_assert(is_same_type<normal_voi_make,factory_voi_make>,"");

}
/**/



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
/*template <typename... Args>																							*/\
/*auto constexpr make_##list_type(list_type<Args...>) -> decltype(make_##list_type (Args{}...));*/						\
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


template <typename... Xs>
struct AnotherList : ListBehavior<AnotherList,Xs...> {};

#define CRTP_LIST(name)                   \
template <typename... Xs>                 \
struct name : ListBehavior<name,Xs...> {};\
template< typename... Xs>                 \
auto constexpr make_##name(Xs...) {       \
	return ListOf<name>::make(Xs{}...);     \
}

CRTP_LIST(cons_list);

LIST(wrap);
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
