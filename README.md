# Compile-Time Scheme Interpreter in C++

This project utilizes C++ template metaprogramming to interpret Scheme code. <b>It is by no means complete, or claims to be.</b> The process is executed entirely at compile-time, transforming Scheme code into an AST and then evaluating it, all within the C++ template system.

## Structure

### 1. Parsing (Lexical Analysis)

- The input Scheme program is encapsulated in a constexpr lambda for compile-time availability.
- A template-based recursive tokenizer processes each character, skipping whitespaces (in cases where they are not needed anymore) and categorizing tokens.
- These tokens are used to construct an AST, represented using variadic template classes to model Scheme's list structures.

### 2. Evaluation & Execution

- The AST is evaluated following the semantics of a language resembling Scheme.
- The computation is performed at compile-time, leveraging C++'s template instantiation mechanism.

## Supported Expressions

    PRIMITIVE PROCEDURES:

    +       (+ ... )    addition
    -       (- ... )    subtraction
    *       (* ... )    multiplication
    /       (/ ... )    division
    =       (= ... )    equality
    <       (< ... )    less than
    >       (> ... )    more than
    
    and     (and ... )  and
    or      (or  ... )  or

    BUILT IN PROCEDURES:

    cons    (cons {1} {2})
    list    (list ... )
    car     (car {1} {2} ... )
    cdr     (cdr {1} {2} ... )
    if      (if {pred} {then} {else})
    apply   (apply {proc} {args})
    eval    (eval 'proc) ;still lacks an env, in my implementation

## Some Examples
### a complex looking expression, taken from `tests.h`
```cpp
auto input = constexpr_string(R"(
    (cons (car 
        (list 1 
            (cdr (list
                1 (+ 1 2))))) 1)
)");

using tokenization_result = decltype(IRcar(tokenizer(input)));
using res = decltype(IReval<environment<>>(tokenization_result{}));

static_assert(
    std::is_same<
        res,
        cons<integer<1>,integer<1>>
    >::value,"");
```
### demonstration of `define` and `comments`

lets pretend that the next code is inside the `constexpr_string` function for syntax highlighting reasons

```scheme
(define (add-two x)
    ((define y 2) ; extra quote on the body, kind of like begin
        (+ x y)))
(add-two 2)
```

```cpp
auto input = constexpr_string(prev_code);

using tokenization_result = decltype(IRcar(tokenizer(input)));
using res = decltype(IReval<environment<>>(tokenization_result{}));

static_assert(
    std::is_same<
        res,
        integer<4>>
    >::value,"");
```
### demonstration of recursion
```scheme
(
    (define (fib x)
        (if (< x 3)
        1
        (+ (fib (- x 1)) (fib (- x 2)))))
    
    (fib 10)
    ; should be 55
)
```

```cpp
auto input = constexpr_string(prev_code);
using tokenization_result = decltype(IRcar(tokenizer(input)));
using res = decltype(IReval<environment<>>(tokenization_result{}));

static_assert(
    std::is_same<
        res,
        integer<55>
    >::value);
```
## Storing Scheme Expressions

- Scheme expressions are transformed into specialized C++ template structures.
- The environment and other necessary elements are represented within the type system of C++ templates.

## Running the project

run `make && ./build/a.out` in the root directory, if your code is valid you should see the debug utilities writing something like this to the console:
```
:::: string to parse ::::
(
  (define (sum-up-to n)
  (if (= n 0) ;this is a comment
      0
      (+ n (sum-up-to (- n 1)))))
  (sum-up-to 2)
)

:::: formatted tokenization result ::::

token_list<
list<token_list<
  list<token_list<scm_define list<token_list<sum-up-to n>>
  list<token_list<scm_if list<token_list<equal n integer<0>>> 
      integer<0>
      list<token_list<addition n list<token_list<sum-up-to list<token_list<subtraction n integer<1>>>>>>>>>>>
  list<token_list<sum-up-to integer<2>>>
>>
>

:::: eval result ::::

integer<3>
```

## Working with templates
...