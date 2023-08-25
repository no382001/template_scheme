#include "tests.h"
#include "ground.h"

//auto m = constexpr_string("(( define ss 11) (+ ss (+ ss 1)))"); // fix char and define bug
auto m = constexpr_string("((define (fib n) (+ n 2)) (fib 3))");
using tokens = decltype(IRcar(tokenizer(m))); // raw token list without the tokenized<...> wrapper
using result_of_expression = decltype(IReval<environment<>>(tokens{}));



constexpr auto end_of_char_list = find_first_non_c< 0 >(constexpr_string("dd "));

int main(){
    //pretty_print(demangle<init_env>());
    //pretty_print(demangle<clean_expression>());
    std::cout << "-----" << '\n';
    pretty_print(demangle<result_of_expression>());
    return 0;
}

// keywords are in, now i need to implement define,
// probably need to use a lookahead, collect all of them in order (or not),
// apply each to the environment, then execute all other code


// if define gets evaluated, it could return a tagged thing that extends the env of the current layer

// lambda is also missing

// i could make quote, self evaluating and return its own wrapped value
// bc as it stands, everything is working fine, but the IR is not really a scheme
// quote is just a wrapper and not really a thing, i could replace it easily and make everything else a wrapper