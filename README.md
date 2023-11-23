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

    BUILT IN PROCEDURES:

    cons    (cons {1} {2})
    list    (list ... )
    car     (car {1} {2} ... )
    cdr     (cdr {1} {2} ... )
    if      (if pred then else)
    apply   (apply proc args)
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
;you do have to use an extra quote for the body of the define whenever there are more than one things are in the body (this comment also works)
(define (add-two x)
    ((define y 2)
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
        integer<2>>
    >::value,"");
```
### demonstration of recursion
lets pretend again... but also note that `(...)` is kind of like `begin`
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
    std::same<
        res,
        integer<55>
    >::value);
```
## Storing Scheme Expressions

- Scheme expressions are transformed into specialized C++ template structures.
- The environment and other necessary elements are represented within the type system of C++ templates.

## Working with templates


### Basics
If we want to work in `constexpr` time, we could start with this but its not that exciting
```cpp
auto constexpr i = 1;
```
this is going to evaluate in compile time, so basically every occurrence of `i` is getting replaced by `1` but its done one layer after the preprocessor so its not like a macro

knowing that you can put `constexpr` near a return type, you could write any algorithm in `purely functional` form (technically)
```cpp
constexpr int _log(int b, int n) {
	return n < b ? 0 : _log(b, n / b) + 1;
}

auto constexpr r = _log(2,8); // 3
```
okay, now we know this. lets talk about `template arguments`, you can store `typenames` in them with no problem, let me show you an example

<i>(still restricted to compile time)</i>
```cpp
template <int T>
struct atom {};

// after declaring this i could say
using a = atom<int>;
```
yeah cool i can store types, but how do i make meaningful arthimetric with those? with `non-type templates` that must be of `integral type`

```cpp
template <int T>
struct atom {};

using a = atom<1>;
```