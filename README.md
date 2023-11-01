
A rudimentary scheme interpreter made with C++ templates. After reaching chapter 4 in SICP while messing around in CPP i had this bright idea, since templates are Turing complete why not try to make the Scheme meta-circular evaluator all in constexpr time.
<br><br>
I've seen some similar projects but they used metaprogramming techniques that were totally foreign to me then so the code is not uniform at all, sometimes i use `decltype` sometimes i dont, sometimes its <b>trailing type</b> sometimes its not, so that might make it hard to read (maybe I'll fix them sometime).
<br><br>
So yeah, no meta-circular evaluator (yet?), but I've got to the first big milestone.<br>
## goals
### [x] being able to evaluate Fibonacchi
#### steps required:
 - [x] implement a lisp `atoms` with templates
 - [x] implement primitive procedures
 - [x] lisp like helper functions for working with lists
 - [x] implement `eval/apply` based on the meta-circular evaluator in SICP C.4
 - [x] implement a tokenizer from string to types in compile time
 - [x] implement `define`
 - [x] plug them together

## Table of Contents
- the evaluator
  - [evaluating Fibonacchi](#fib)
  - [data structures](#datas)
    - [atoms](#atoms)
    - [primitive procedures](#primproc)
- the tokenizer
  - [working with strings in constexpr](#string)
  - [tokenizing with types](#token)
    - [n-digit integers](#n-digit)

<br>
<br>

# the evaluator

## <a name="fib">Fibonacchi</a>
<i>this is all based on working code (so if it doesnt work see `tests.h`)</i>

The goal is to evaluate something that looks like this in `mit-scheme`

```scheme
(define (fib x)
    (if (<= x 2)
    1
    (+ (fib (- x 1)) (fib (- x 2)))))
```
Should look something like this in its IR if you wanted to define `(fib n)` as a procedure<br>
```cpp
using fib_proc_body = decltype(
    wrap<list<scm_if,
        wrap<list<lesseq,c_<'x'>,integer<2>>>,
        integer<1>,
            list<addition,
                list<c_list<c_<'f'>,c_<'i'>,c_<'b'>>,,
                  wrap<list<subtraction,c_<'x'>,integer<1>>>>,
                list<c_list<c_<'f'>,c_<'i'>,c_<'b'>>,,
                  wrap<list<subtraction,c_<'x'>,integer<2>>>>>>>{});

using init_env = decltype(
    make_environment(
      table_entry<
        c_list<c_<'f'>,c_<'i'>,c_<'b'>>, // variable name
        procedure,  // variable type
        c_<'x'>, // arguments
        fib_proc_body>{})); // procedure body

using res =
    decltype(IReval<init_env>(wrap<
      list<c_list<c_<'f'>,c_<'i'>,c_<'b'>>,wrap<integer<30>>>>{}));

static_assert(is_same_type<res,integer<832040>>,"fib 30");
```

# data structures
## <a name="atoms">atoms</a>

here is our first type
```cpp
template <int>
class integer {};

// if i do this, i can store an int there
using i = integer<1>;
```
its still a type and doesnt do anything, but i have a way to store integers type variables, plus i can give them names

### <b>PROBLEM [1]: templated int collections cannot be compared with others bc the template argument designates an instance</b>
i can compare `integer<1>` with `integer<1>` but not with `integer<2>`, they are a different instance. <br>
okay but then how can i tell if two of my variables are the same type?

```cpp
template <typename A, typename B>
constexpr inline bool is_same_type = std::is_same<A, B>::value; // it really should be called is_same_type_v

auto constexpr b1 = is_same_type<integer<1>,integer<1>>; // true
auto constexpr b2 = is_same_type<integer<1>,bool>; // false
auto constexpr b3 = is_same_type<integer<1>,integer<2>>; // false
```
if we want our function `is_same_type` to work like "it should", we need to create a specialization, but since we want to think forward and we know that we might want similar data structures to `integer<int>`, lets define a "primitive" type checker for types like this

```cpp
// check identity
template< typename Test, template < int... > class Type >
struct is_templated_int : std::false_type {};

template< template < int... > class Type, int ...Args >
struct is_templated_int< Type< Args... >, Type > : std::true_type {};

template < typename T >
constexpr inline bool is_integer_t = is_templated_int< T, integer >::value;
```
lets start from the bottom, `is_integer_t` takes a typename and passes it on to `is_templated_int` with the struct name `integer` (passing `integer` like this is only possible bc `is_templated_int` expects `Type` in the second parameter and `Type` is defined `template < int... > class Type` and we know that `integer` takes one parameter `int`), if the deduction has succeeded the `value` of the struct `is_templated_int` is `std::true_type` otherwise the specialzation on the top turns it into `std::false_type`

## <a name="primproc">primitive procedures</a>

primitive operations each have a struct dedicated to them, these names are used in eval like tags in lisp, but since i must evaluate everything and cannot use quotes, they each have their own struct to make this possible

```scheme
(eval '(+ 2 3) initial-env)
;IReval<init_env>(wrap<list<addition,integer<2>,integer<3>>>{})

```
in the implementation of `IReval` (IR for Intermediate Represenation, so the name doesnt conflict with the procedure `eval` itself), `IReval` uses `is_prim_proc` to determine if the list is tagged with a name of a primitive procedure, it evaluates the operands with `eval_members` and passes them on to`IRapply`

```cpp
/* ... */
using tag = decltype(IRcar(Exp{})); // the tag at the beginning of the expression
/* ... */
else if constexpr (is_prim_proc(tag{})){
            using app_operands = decltype(eval_members<Env>(IRcdr(Exp{})));

            if constexpr (!is_self_evaluating(app_operands{})){
                if constexpr (is_prim_proc(IRcar(app_operands{}))){

                    using proc_res = decltype(IRapply(tag{},wrap<app_operands>{}));
                    return proc_res{};
                } else {

                    using proc_res = decltype(IRapply(tag{},wrap<app_operands>{}));
                    return proc_res{};
                }
            } else {
                using proc_res = decltype(IRapply(tag{},wrap<app_operands>{}));
                return proc_res{};
            }

        }
/* ... */
```
`IRapply` is a wrapper function that just calls `apply_primitve_procedure`
```cpp
template < typename Proc, typename Args >
auto constexpr IRapply(Proc,wrap<Args>) {

	// returns void if procedure was not found...
    // it means that its a compound proc, handled elsewhere
    using prim_proc_res = decltype(apply_primitve_procedure(Proc{},Args{}));
    
    // cant instantiate void, so:
    if constexpr (is_same_type<prim_proc_res,void>){
        return;
    } else {
        return prim_proc_res{};
    }
}
```
### arithmetic operations:
`apply_primitve_procedure` has template specializations for each primitive procedure, first lets look at the arithmetic operations, i put all the code in a macro called `PRIMITIVE_ARITHMETIC_OP` so that i dont need to write them a hundred times for each operation, for the operation `additon` the body of the macro after the preprocessor should look like this

```cpp
struct addition {}; /* identifying "quoted" name */

template <> /* overload */
constexpr bool is_prim_proc(addition){
	return true;
}

template <typename Arguments> /* apply_primitve_procedure overload */
auto constexpr apply_primitve_procedure(addition,Arguments){
	return apply_addition(Arguments{});
}

template <int A, typename... Rest>
auto constexpr sub_apply_addition(list<integer<A>, Rest...>) {
	if constexpr (sizeof...(Rest) > 0)
		return A + sub_apply_addition(list<Rest...>{});
	else{ return A; }
}

template <int A, typename... Rest>
auto constexpr apply_addition(list<integer<A>,Rest...>) {
        static_assert((sizeof...(Rest) > 0), "addtion with no operands");
		return integer<A + sub_apply_addition(list<Rest...>{})>{};
}
```
in order to be able to construct an `integer<int>` as a result of the `apply_addition` operation im using the `sub_apply_addition` subroutine so i handle other type of data for most of the calculation. 

### relational operations:
relation operations are the same for the most part, except that im working woth booleans
```cpp

template <int A, typename... Rest>
auto constexpr sub_apply_less(list<integer<A>,Rest...>){
	if constexpr (sizeof...(Rest) == 0) {
		return A;
	} else {
		auto constexpr second = sub_apply_less(list<Rest...>{});
		if constexpr (second == 99999){
			return 99999;
		} else if constexpr (A < second){
			return A;
		} else {
			return 99999;
		}
	}
}

template <int A, typename... Rest>
auto constexpr apply_less(list<integer<A>,Rest...>){
	static_assert((sizeof...(Rest) > 0),"no second operand, for relational operation");
	auto constexpr second = sub_apply_less(list<Rest...>{});
	if constexpr (second == 99999){
		return scm_false{};
	} else if constexpr (A < second){
		return scm_true{};
	} else {
		return scm_false{};
	}
}
```
sadly looks uglier than it should be, the first version didnt have `99999`, i was using VS Code and IntelliSense could sometimes evaluate things that it shouldnt have, like multiple return types for an auto function, which would have made this a lot more readable<br>
what i was trying to do is to return `scm_false{}` instead of `99999` because i thought that the way `if constexpr` works is like this
```cpp
if constexpr (1){
    // something
} else {
    // something else that is discarded
}
```
so this becomes this
```cpp
if constexpr (1){
    // something
} else {}
```
so for my use case it actually wouldnt matter that the return types are not the same, but as it turns out even <b>even discarded functions have to be syntactically correct</b>, but somehow this doesnt matter to IntelliSense

# The Tokenizer

## <a name="string">traversing a string in constexpr, how?</a>
<i> (from here this document is valid until `dc8078e` (HEAD of `old` branch), the repo is being reconstructed from the ground up, latest working example is `3d43df9` (HEAD of `2nd_revision` branch and currently `main`))</i>
<br>


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

## <a name="token">tokenizing with cpp types, how?</a>
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
$$ p||q = pb^{l(q)} $$
$$ l(q) = \lfloor \log_{b}(q) \rfloor + 1 $$
<br>
<i>(on high enough numbers, expect an integer overflow)</i>

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
  static constexpr auto merge(integer<A>)
    -> integer< _pow((Value * 10), (_log(10, A) + 1)) + A >;
};
```
```cpp
using i = decltype(integer<1>::merge(integer<1>{})); // integer<11>
```
