#include "tests.h"
#include "h/eval_apply.h"
#include "h/tokenizer.h"
#include "h/pretty_print.h"

#include <type_traits>
#include <utility>

auto main_str = CSTRING(R"(
  (+ 1 1)
)");

// tabs seem to fuck some things up, idk why

/*
(define (pascal r c)
    (if (or (= c 1) (= c r))
      1
      (+ (pascal (- r 1) (- c 1)) (pascal (- r 1) c))))
  
  (pascal 3 2)
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
    std::cout << main_str.chars << '\n';
    std::cout << '\n';

    std::cout << ":::: tokenization_result_w_whitespaces ::::" << '\n';
    std::cout << replace_chars(demangle<tb_evaluated>()) << '\n';
    std::cout << '\n';

    std::cout << ":::: formatted tokenization result ::::" << '\n';
    std::cout << (replaceWhitespace(replace_chars(demangle<tokenization_result_w_whitespaces>()))) << std::endl;
    std::cout << '\n';
    
    std::cout << ":::: cleaned ::::" << '\n';
    std::cout << replace_chars(demangle<tb_evaluated>()) << '\n';
    std::cout << '\n';

    std::cout << ":::: eval res ::::" << '\n';
    std::cout << replace_chars(demangle<eval_result>()) << '\n';

    return 0;
}