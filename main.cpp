#include "tests.h"
#include "ground.h"

namespace test {
    auto str = constexpr_string("(( define ss 11) (+ ss 1))"); // fix char and define bug
    using tokenization_result = decltype(IRcar(tokenizer(str))); // raw token list without the tokenized<...> wrapper
    using eval_result = decltype(IReval<environment<>>(tokenization_result{}));
    //static_assert(is_same_type<no_parameter_define_result,integer<3>>);
};

int main(){
    //pretty_print(demangle<init_env>());
    //pretty_print(demangle<clean_expression>());
    std::cout << "-----" << '\n';
    pretty_print(demangle<test::eval_result>());
    return 0;
}


// if define gets evaluated, it could return a tagged thing that extends the env of the current layer

// lambda is also missing

// i could make quote, self evaluating and return its own wrapped value
// bc as it stands, everything is working fine, but the IR is not really a scheme
// quote is just a wrapper and not really a thing, i could replace it easily and make everything else a wrapper