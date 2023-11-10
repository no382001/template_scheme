#include "tests.h"
#include "h/eval_apply.h"
#include "h/tokenizer.h"

#include <type_traits>
#include <utility>

template<typename T, typename = void> // spec for SFINAE
struct can_call_IReval : std::false_type {};

template<typename T> // spec for ok
struct can_call_IReval<T, std::void_t<decltype(IReval<environment<>>(std::declval<T>()))>> : std::true_type {};

template<typename T, bool CanCall = can_call_IReval<T>::value> // isnt this basically std::enable_if?
struct eval_result_helper {
    using type = decltype(IReval<environment<>>(T{}));
};

template<typename T>
struct eval_result_helper<T, false> {
    using type = void;
};

auto main_str = constexpr_string(R"(
(define (list-of-values exps env)
  (if (no-operands? exps)
    '()
    (cons (eval (first-operand exps) env)
          (list-of-values (rest-operands exps) env))))
)");

using tokenization_result = decltype(IRcar(tokenizer(main_str))); // raw token list without the tokenized<...> wrapper
using eval_result = typename eval_result_helper<tokenization_result>::type;

int main(){
    std::cout << "-env----" << '\n';
    pretty_print(demangle<init_env>());
    std::cout << "-tokens-" << '\n';
    pretty_print(demangle<tokenization_result>());
    std::cout << "-result-" << '\n';
    pretty_print(demangle<eval_result>());
    return 0;
}


// lambda is also missing

// eval wrap<list<...>>
// eval quote<wrap<list<..>>>
// eval wrap<list<..>>  quote does not matter here, probably eval is redundant when there is no quote
