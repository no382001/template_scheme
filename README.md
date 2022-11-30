## current capabilities
- parsing char* in constexpr

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
