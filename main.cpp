#include "tests.h"
#include "ground.h"

//auto m = constexpr_string("(( define ss 11) (+ ss (+ ss 1)))"); // fix char and define bug
//auto m = constexpr_string("((define (fib n) (+ n 2)) (fib 3))");
//auto m = constexpr_string("((define n 1) (+ n 3))");
//auto m = constexpr_string("((define (sum a b) (+ a b)) (sum 1 2)))"); // this is fine
/** /
auto one_parameter_define = constexpr_string("((define (inc a) (+ 1 a)) (inc 1)))");
using one_parameter_define_tokens = decltype(IRcar(tokenizer(one_parameter_define))); // raw token list without the tokenized<...> wrapper
using one_parameter_define_result = decltype(IReval<environment<>>(one_parameter_define_tokens{}));
static_assert(is_same_type<one_parameter_define_result,integer<2>>);
/**/
auto no_parameter_define = constexpr_string("( (define n 1) (+ n 2))");
using no_parameter_define_tokens = decltype(IRcar(tokenizer(no_parameter_define))); // raw token list without the tokenized<...> wrapper
using no_parameter_define_result = decltype(IReval<environment<>>(no_parameter_define_tokens{}));
//static_assert(is_same_type<no_parameter_define_result,integer<3>>);

int main(){
    //pretty_print(demangle<init_env>());
    //pretty_print(demangle<clean_expression>());
    std::cout << "-----" << '\n';
    pretty_print(demangle<no_parameter_define_result>());
    return 0;
}


// if define gets evaluated, it could return a tagged thing that extends the env of the current layer

// lambda is also missing

// i could make quote, self evaluating and return its own wrapped value
// bc as it stands, everything is working fine, but the IR is not really a scheme
// quote is just a wrapper and not really a thing, i could replace it easily and make everything else a wrapper