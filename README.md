## current capabilities
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
auto x = constexpr_string("(+ 3 (* 3 (+ 2 2) 2))");
using tokens = decltype(tokenize(x));
auto constexpr res = parse(tokens{});	
pretty_print(typeid(tokens).name());
std::cout << ";; " << res;

/** /  
token_list<
  list<
    token_list<
      plus,
      integer<3>,
      list<
        token_list<
          mul,
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
      >,
      integer<2>
    >
  >,
  integer<2>
>
;; 29
/**/
```
