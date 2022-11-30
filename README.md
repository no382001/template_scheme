- [current capabilities](#capab)
- [how](#how)


# <a name="capab">current capabilities</a>
### parsing char* in constexpr

``` cpp
auto x = constexpr_string("(ab 12) (as 123 +)");
using res = decltype(tokenize(x));

/** /
  token_list<
    list<
    token_list<
      c_list<c<97>, c<98>>,
      whitespace,
      integer<12>
    >
  >,
  whitespace,
  list<
    token_list<
      c_list<c<97>, c<115>>,
      whitespace,
      integer<123>,
      whitespace,
      plus>
    >
   >
/**/

```
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
# <a name="how">traversing a string in constexpr, how?</a>
how? very easy, since we cant really manipulate or even look at std::string or char* in constexpr (they only work in runtime which is not our thing now) our only option is std::string_view which basically is just a constexpr char ptr, for our case anyways. <br><br>
using this macro, we have our lambda object that returns its arguments in constexpr
```cpp
#define constexpr_string(...) ([]() constexpr -> std::string_view { return __VA_ARGS__; })
```
by initializing the object and indexing after, we can get the character we are looking for, all in constexpr 

```cpp
auto x = constexpr_string("abc");

static_assert('a' == x()[0]);
static_assert('b' == x()[1]);
```