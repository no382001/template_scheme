#include "tests.h"
#include "h/eval_apply.h"
#include "h/tokenizer.h"
#include "h/pretty_print.h"

#include <type_traits>
#include <utility>

auto main_str = constexpr_string(R"(
(
  (define (sum-up-to n)
  (if (= n 0)
      0
      (+ n (sum-up-to (- n 1)))))
  (sum-up-to 2)
)
)");



/*
the problem might be that i want to bind a value to a variable but at the same time during the expansion of the expression
the same value comes up and list_search never can find the right one, hence the infinite loop, since append puts it in front
  option 1: search from left side
  option 2: append from head, creating a kind of stack of same name variables
  
  ...
  table_entry<n, variable, integer<2> >, 
	table_entry<n, variable, integer<1> > >
  ...

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
// the templates intersect sometimes and give a wrong result???
/*
(define (neg x) (- 0 x))
(neg 2) ; is fine

(define (neg n) (- 0 n))
(neg 2) ; is not
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


