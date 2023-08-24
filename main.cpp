#include "tests.h"
#include "ground.h"



auto m = constexpr_string("(fib 11)");
using tokens = decltype(IRcar(tokenizer(m))); // raw token list without the tokenized<...> wrapper
using clean_expression = typename replace_nested_list<tokens>::type; // convert list
using result_of_expression = decltype(IReval<init_env>(clean_expression{}));

//static_assert(is_same_type<result_of_expression,integer<2>>,"long way");

auto other = constexpr_string("(define 11)");
using tokens_other = decltype(IRcar(tokenizer(other)));


int main(){
    //pretty_print(demangle<init_env>());
    pretty_print(demangle<clean_expression>());
    //pretty_print(demangle<result_of_expression>());
    return 0;
}

// there is no way to tokenize, ifs and all that probably