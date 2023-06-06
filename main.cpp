//#include "tests.h"
#include "ground.h"
#include "h/tokenizer.h"


#include <string_view>
#define constexpr_string(...) ([]() constexpr -> std::string_view { return __VA_ARGS__; })


auto str = constexpr_string("(+ 2 1)");
using tokens = decltype(tokenize(str));

int main(){
    //print_table(init_env{});
    return 0;
}