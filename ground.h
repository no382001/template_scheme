
#include "h/eval_apply.h"
#include "h/tokenizer.h"

auto str = constexpr_string("(+ 11 (+ 11 2))");
using tokens = decltype(IRcar(tokenizer(str))); // raw token list without the tokenized<...> wrapper
using clean_expression = decltype(replace_nested_list<tokens>{}); // convert list
using result_of_expression = decltype(IReval<init_env>(replaced_expression{}));

static_assert(is_same_type<result_of_expression,integer<24>>,"long way");