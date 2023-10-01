#include "tests.h"
#include "ground.h"

auto main_str = constexpr_string("(eval '(+ 1 1))");
//using tokenization_result = decltype(IRcar(tokenizer(main_str))); // raw token list without the tokenized<...> wrapper
using tokenization_result = decltype(IRcar(tokenizer(main_str))); // raw token list without the tokenized<...> wrapper
using eval_result = decltype(IReval<environment<>>(tokenization_result{}));
//static_assert(is_same_type<eval_result,integer<2>>);

int main(){
    //pretty_print(demangle<init_env>());
    std::cout << "-----" << '\n';
    pretty_print(demangle<tokenization_result>());
    std::cout << "-----" << '\n';
    pretty_print(demangle<eval_result>());
    return 0;
}


// lambda is also missing

// eval wrap<list<...>>
// eval quote<wrap<list<..>>>
// eval wrap<list<..>>  quote does not matter here, probably eval is redundant when there is no quote
