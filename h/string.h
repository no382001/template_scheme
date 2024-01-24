#pragma once
#include <type_traits>

// C++17 constexpr string, to replace C++20 constexpr lambda solution
// https://stackoverflow.com/questions/15858141/conveniently-declaring-compile-time-strings-in-c
// https://ideone.com/8Ft2xu

namespace  variadic_toolbox
{
  template<unsigned  count,
    template<unsigned...> class  meta_functor, unsigned...  indices>
  struct  apply_range
  {
    typedef  typename apply_range<count - 1, meta_functor, count - 1, indices...>::result  result;
  };
 
  template<template<unsigned...> class  meta_functor, unsigned...  indices>
  struct  apply_range<0, meta_functor, indices...>
  {
    typedef  typename meta_functor<indices...>::result  result;
  };
}


namespace  compile_time
{

  template<char...  str>
  struct  string
  {
    static constexpr const char chars[] = { str...,'\0' };
    
  };

  template<char...  str>
  constexpr  const char string<str...>::chars[];
 
  template<typename  lambda_str_type>
  struct  string_builder
  {
    template<unsigned... indices>
    struct  produce
    {
      typedef  string<lambda_str_type {}.chars[indices]...>  result;
    };
  };

}

  template <int N, char X, char... Xs>
  constexpr auto nth_element() {
    if constexpr (N == 0) {
      return X;
    } else {
      return nth_element<N-1,Xs...>();
    }
  }

  template <int N, char... Cs>
  constexpr auto get_index(compile_time::string<Cs...>) {
    static_assert(N < sizeof...(Cs), "index out of bounds");
    return nth_element<N,Cs...>();
  }

  template <char... Cs>
  constexpr char get_size(compile_time::string<Cs...>) {
      return sizeof...(Cs);
  }



#define  CSTRING(string_literal)                                                        \
    []{                                                                                 \
        struct  constexpr_string_type { const char * chars = string_literal; };         \
        return  variadic_toolbox::apply_range<sizeof(string_literal)-1,                 \
            compile_time::string_builder<constexpr_string_type>::produce>::result{};    \
    }()

namespace testing {

  auto constexpr str_hello = CSTRING("hello");
  auto constexpr str_world = CSTRING(" world");
  using t = decltype(str_world);
  auto constexpr ssss = get_size<>(t{});
  auto constexpr char_w = get_index<ssss-1>(t{});
  static_assert(char_w == 'd');

};

/*
auto constexpr str_hello = CSTRING("hello");
auto constexprstr_world = CSTRING(" world");
auto constexpr concat = (str_hello + str_world);
*/