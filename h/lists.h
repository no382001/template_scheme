#pragma once
#include "utils.h"

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

// generatates a ListType with the rules of `make`
template <template <typename...> class ListType>
struct ListOf {

  // base
  template <typename X >
  static auto constexpr make(X) -> ListType<X>;
  
  // rec
  template < typename X, typename ...Xs>
  static auto constexpr make(X, Xs...)
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


#define IS_X_LIST(name)							       \
template< typename T>							         \
auto constexpr is_##name(T t) {					   \
	return false;								             \
}												                   \
template< typename ...Args>						     \
auto constexpr is_##name(name<Args...> l) {\
	return true;								             \
}												                   \
template<>										             \
auto constexpr is_##name(name<> l) {			 \
	return false;								             \
}


#define CRTP_LIST(name)                   \
template <typename... Xs>                 \
struct name : ListBehavior<name,Xs...> {};\
template< typename... Xs>                 \
auto constexpr make_##name(Xs...) {       \
	return ListOf<name>::make(Xs{}...);     \
}                                         \
IS_X_LIST(name);

// - definitions

CRTP_LIST(cons_list);
CRTP_LIST(wrap);
CRTP_LIST(list);
CRTP_LIST(c_list);
CRTP_LIST(token_list);


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
