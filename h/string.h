#pragma once

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
    static constexpr const char chars[] = { str..., '\0' };

    template <int N, char X, char... Xs>
    static constexpr auto nth_element() {
      if constexpr (N == 0) {
        return X;
      } else {
        return nth_element<N-1,Xs...>();
      }
    }
    
    template <int N>
    static constexpr char get() {
        static_assert(N < sizeof...(str), "index out of bounds");
        return nth_element<N, str...>();
    }
    
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
  auto constexpr char_w = t::get<2>();
  static_assert(char_w == 111);

};

/*
auto constexpr str_hello = CSTRING("hello");
auto constexprstr_world = CSTRING(" world");
auto constexpr concat = (str_hello + str_world);
*/