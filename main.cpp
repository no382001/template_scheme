#include "tests.h"
#include "h/eval_apply.h"
#include "h/tokenizer.h"
#include "h/pretty_print.h"

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
(+ 1 2)
)");

using tokenization_result_w_whitespaces = decltype(tokenize(main_str)); // raw token list without the tokenized<...> wrapper
// remove whitespaces
using tokens = decltype(clean_whitespaces(tokenization_result_w_whitespaces{}));
// replace token_list<list<...>> with wrap<list<...>>
using clean_expression = typename replace_nested_list<tokens>::type; // convert list
// replace outer wrap<...> with tokenized<...>
using tb_evaluated = decltype(replace_wrapper(clean_expression{},tokenized{}));
// evaluate expression
using eval_result = decltype(IReval<init_env>(IRcar(tb_evaluated{})));

int main(){
    std::cout << ":::: string to parse ::::" << '\n';
    std::cout << main_str() << '\n';
    std::cout << '\n';


    std::cout << ":::: tokenization_result_w_whitespaces ::::" << '\n';
    pretty_print(demangle<tokenization_result_w_whitespaces>());
    std::cout << '\n';

    std::cout << ":::: formatted tokenization result ::::" << '\n';
    std::cout << replaceWhitespace(demangle<tokenization_result_w_whitespaces>()) << std::endl;
    std::cout << '\n';
    
    std::cout << ":::: cleaned ::::" << '\n';
    pretty_print(demangle<tb_evaluated>());
    std::cout << '\n';
    
    std::cout << ":::: eval res ::::" << '\n';
    pretty_print(demangle<eval_result>());
    return 0;
}


// lambda is also missing

// eval wrap<list<...>>
// eval quote<wrap<list<..>>>
// eval wrap<list<..>>  quote does not matter here, probably eval is redundant when there is no quote
