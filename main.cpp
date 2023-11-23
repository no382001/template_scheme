#include "tests.h"
#include "h/eval_apply.h"
#include "h/tokenizer.h"
#include "h/pretty_print.h"

#include <type_traits>
#include <utility>

auto main_str = constexpr_string(R"(
(
  ;this is a comment within the midst of tokens
  (+ 1 1)
)
)");

auto constexpr r = _log(2,8);

/*
  (define (negative x) (- 0 x))
  
  (define (quotient a b)
    (- a (* (/ a b) b)))
  
  (define (abs x)
    (if (< x 0)
      (- x (* 2 x))
      x))
    
  (define precision 1000)

  (define (improve n guess)
    (/ (+ guess (/ (* n precision) guess)) 2))

  (define (good-enough? n guess)
    (< (abs (- (* guess guess) (* n precision))) precision))

  (define (sqrt-iter n guess)
    (if (good-enough? n guess)
        guess
        (sqrt-iter n (improve n guess))))

  (define (sqrt-newton n)
    (sqrt-iter n (* n precision)))
  
  (sqrt-newton 25)

*/

using tokenization_result_w_whitespaces = decltype(tokenize(main_str)); // raw token list without the tokenized<...> wrapper
// remove whitespaces
using tokens = decltype(clean_whitespaces(tokenization_result_w_whitespaces{}));
// replace token_list<list<...>> with wrap<list<...>>
using clean_expression = typename replace_nested_list<tokens>::type; // convert list
// replace outer wrap<...> with tokenized<...>
using tb_evaluated = decltype(replace_wrapper(clean_expression{},tokenized{}));
// evaluate expression
using eval_result = decltype(IReval<environment<>>(IRcar(tb_evaluated{})));

//auto constexpr vcalu = contains_type_t<scm_if,tokens>::value;

int main(){
    std::cout << ":::: string to parse ::::" << '\n';
    std::cout << main_str() << '\n';
    std::cout << '\n';

    /** /
    std::cout << ":::: tokenization_result_w_whitespaces ::::" << '\n';
    std::cout << replace_chars(demangle<tb_evaluated>()) << '\n';
    std::cout << '\n';
    /**/

    std::cout << ":::: formatted tokenization result ::::" << '\n';
    std::cout << (replaceWhitespace(replace_chars(demangle<tokenization_result_w_whitespaces>()))) << std::endl;
    std::cout << '\n';
    
    /** /
    std::cout << ":::: cleaned ::::" << '\n';
    std::cout << replace_chars(demangle<tb_evaluated>()) << '\n';
    std::cout << '\n';
    /**/

    std::cout << ":::: eval res ::::" << '\n';
    std::cout << replace_chars(demangle<eval_result>()) << '\n';
    return 0;
}


