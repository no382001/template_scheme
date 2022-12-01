- [current capabilities](#capab)
- [1. how? working with string in constexpr](#string)
- [2. how? tokenizing with types](#token)


# <a name="capab">current capabilities</a>
### evaluating simple expressions
```cpp
using tokens = decltype(tokenize(x));
auto constexpr res = parse(tokens{});
//pretty_print(typeid(tokens).name()); //with msvc
auto str = std::string(demangle<tokens>());
pretty_print(str);
std::cout << ";; " << res << "\n";

/** /  
token_list<
 list<
  token_list<
   plus,
   integer<3>,
   list<
    token_list<
     plus,
     integer<3>,
     list<
      token_list<
       plus,
       integer<2>,
       integer<2>
      >
     >,
     integer<2>
    >
   >
  >
 >
>
;; 12
/**/
```
# <a name="string">traversing a string in constexpr, how?</a>
how? very easy, since we cant really manipulate or even look at std::string or char* in constexpr (they only work in runtime which is not our thing now) our only option is std::string_view which basically is just a constexpr char ptr, for our case anyways. <br><br>

using this macro, we have our lambda object that returns its arguments in constexpr
```cpp
#define constexpr_string(...) ([]() constexpr -> std::string_view { return __VA_ARGS__; })
```
(`__VA_ARGS__` is a variadic macro that refers to whatever occupies the place of `...` in the argument list)<br><br>

by initializing the object and indexing after, we can get the character we are looking for, all in constexpr 
```cpp
auto str = constexpr_string("abc");

static_assert('a' == str()[0]);
static_assert('b' == str()[1]);
```
now that we have all we need, we can top it off with another macro that enables us to handle files in constexpr
```cpp
static constexpr const char from_file[] = { 
  #include "directives.scm" 
};
```
we just have to make sure that our source file is formatted appropriately, in the case of `(+ 1 1)` the formatting is the following:
```cpp
R"=====(+ 1 1)====="
```

# <a name="token">tokenizing with cpp types, how?</a>
lets start easy, we'd like to parse `(+ 1 1)`, how do we begin? <br>
define the atoms as such
```cpp
struct list_start {};
struct list_end {};
struct plus {};
struct whitespace {};
```
so far its trivial, all of these tokens correspond to only one character, however how can we represent an integer? 
```cpp
template <int value>
struct integer {};
```
like so the template parameter `value` will simply hold onto our number <i>(note that this can only hold one digit integers at a time, so its not really useful, i will explain the multi digit solution later on)</i> <br><br>

lets determine the corresponding char tokens for our types and create a function that can tell us what kind of token is it
```cpp
template <int C>
constexpr auto deduce_token_type() {
  if constexpr (C == '(') {
    return list_start{};
  } else if constexpr (C == ')') {
    return list_end{};
  } else if constexpr (C == '+') {
    return plus{};
  } else if constexpr (C == ' ') {
    return whitespace{};
  } else if constexpr (C >= '0' && C <= '9') {
    return integer<C - '0'>{}; //dont forget, its ascii char
  }
}
```
and there we have it, we now now the individual types of our tokens
```cpp
auto str = constexpr_string("(+ 1 1)");

using token0 = decltype(deduce_token_type<str()[0]>());
using token1 = decltype(deduce_token_type<str()[1]>());
using token2 = decltype(deduce_token_type<str()[2]>());
using token3 = decltype(deduce_token_type<str()[3]>());
```
but this isnt something we can work with yet, in order to feed this to a parser and get an evaluation, we need a list of tokens <br><br>

(syntax highlighting will get worse from here with md)
<br><br>

nothing is easier, here it is
```cpp
template < typename ...Types >
struct list{
  // base case, when append recieves an emtpy list, return the list that it was given (simple trailing return types dont need a body in these cases)
  static constexpr auto append(list<>)->list< Types... >;
  // append type A to the end of the list
  template < typename A >
  static constexpr auto append(A)->list< Types..., A >;
  // single parameter specialization
  template < typename A >
  static constexpr auto append(list< A >){
    return decltype(append(A{})){};
  }
  // 2+ parameter specialization
  template < typename A, typename B, typename ...Args >
  static constexpr auto append(list< A, B, Args... >){
    using curr = decltype(list::append(A{}));
    return decltype(curr::append(list< B, Args... >{})){};
  }
};
// list wrapper
template < typename T, typename ...Rest>
auto constexpr make_list(T, Rest...) -> decltype(list< T >::append(list< Rest... >{}));
// base case for the list wrapper
auto constexpr make_list()->list<>;
```
using this list construction we can loop thru the string
```cpp
template <typename Lambda, size_t Index = 0>
constexpr auto tokenize(Lambda str_lambda) {
  constexpr auto str = str_lambda();
  if constexpr (Index < str.size()) {
    using curr = decltype(deduce_token_type< str[Index] >());
    using second = decltype(tokenize< Lambda, Index + 1 >(str_lambda));
    return make_list(curr{},second{});
  } else {
    return make_list();
  }
}
```
and get our list of tokens the following way
```cpp
using tokens = decltype(tokenize(str));
```
