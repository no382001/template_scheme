this document is valid until `dc8078e` (HEAD of `old` branch), the repo is being reconstructed from the ground up, latest working example is `3d43df9` (HEAD of `2nd_revision` branch and currently `main`)<br>
<br>
all im using to compile is `g++ -std=c++20 ...`, which is the CPP20 standard, i believe this is when they introduced the constexpr lambdas that im using (question mark), i dont remember, i will look it up

- [working with strings in constexpr](#string)
- [tokenizing with types](#token)
  - [n-digit integers](#n-digit)

# <a name="string">traversing a string in constexpr, how?</a>
how? very easy, since we cant really manipulate or even look at std::string or char* in constexpr (they only work in runtime which is not our thing now) our only option is std::string_view which basically is just a constexpr char ptr, for our case anyways. <br><br>

using this macro, we have our lambda object that returns its arguments in constexpr
```cpp
#include <string_view>

#define constexpr_string(...) ([]() constexpr -> std::string_view { return __VA_ARGS__; })
```
(`__VA_ARGS__` is a variadic macro that refers to whatever occupies the place of `...` in the argument list)<br><br>

by calling the lambda and indexing after, we get the character were looking for 
```cpp
auto letters = constexpr_string("abc");

static_assert('a' == letters()[0]);
static_assert('b' == letters()[1]);
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
so far its trivial, all of these tokens correspond to only one character, however how can we represent an integer? we have template parameters to help us with that
```cpp
template <int value>
struct integer {};
```
like so, the template parameter `value` will simply hold onto our number<br><br>

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

using token0 = decltype(deduce_token_type<str()[0]>()); // list_start
using token1 = decltype(deduce_token_type<str()[1]>()); // plus
using token2 = decltype(deduce_token_type<str()[2]>()); // whitespace
using token3 = decltype(deduce_token_type<str()[3]>()); // integer<1>
```
but this isnt something we can work with yet, in order to feed this to a parser, we need a list of tokens <br><br>

so lets introduce variadic template parameters and abuse them a bit <i>(this example is not complete and has some side effects, look for the real thing in the headers if you are interested)</i>
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

using this previous list construction we can loop thru our string
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
// list<list_start, plus, whitespace, integer<1>, whitespace, integer<1>, list_end> 
```
## <a name="n-digit">n-digit integer token as type</a>
to tokenize an n digit integer we just need to implement the following expression<br>
<img src="https://user-images.githubusercontent.com/102482527/205129800-c9465d3b-91b0-46b0-a601-ca6c4a43eb4e.svg" width="25%"></img><br>
where<br>
<img src="https://user-images.githubusercontent.com/102482527/205129790-5dd31fe9-2f07-4f29-b561-7e5b8609684d.svg" width="25%"></img>
<br>
<i>(this was very fun to do, but on certain numbers in the template argument it just fails, i have yet to figure out why, it works on most smaller than 4-5 digit numbers tho)</i>


sadly we cant use the C implementation of `log` and `pow` as we need a constexpr solution which is not yet available. We need to implement it ourselves, and the solution can only be purely functional.
```cpp
constexpr int _pow(int base, int exp, int result = 1) {
  return exp < 1 ? result : _pow(base * base, exp / 2, (exp % 2) ? result * base : result);
}

constexpr int _log(int b, int n) {
  return n < b ? 0 : _log(b, n / b) + 1;
}
```
now we can finally implement the concatenation itself
```cpp
template <int Value>
struct integer{
  template < int A >
  static constexpr auto merge(integer<A>)->integer< _pow((Value * 10), (_log(10, A) + 1)) + A >;
};
```
```cpp
using i = decltype(integer<1>::merge(integer<1>{})); // integer<11>
```
