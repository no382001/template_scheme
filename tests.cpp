#include <string_view>
#include <type_traits>
#include <cmath>
#include "h/atoms.h"
#include "h/utils.h"
#include "h/lists.h"
#include "h/tokenizer.h"
#include "h/number_operations.h"
//#include "pretty_print.h"
#include <iostream>

int main(){
    int success, fail = 0;

    auto x = constexpr_string("(+ 3 (+ 3 (+ 2 2) 2))");
	//auto x = constexpr_string("( abcd )");
	using tokens = decltype(tokenize(x));
	auto constexpr res = parse(tokens{});

}