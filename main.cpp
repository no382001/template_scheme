#include <string_view>
#include <type_traits>
#include <cmath>
#include "h/atoms.h"
#include "h/utils.h"
#include "h/lists.h"
#include "h/tokenizer.h"
#include "h/number_operations.h"
#include "h/parser.h"
#include "tests.h"

#include "h/pretty_print.h"
#include <iostream>
#include <string_view>

int main(){

	auto x = constexpr_string("(+ 3 (+ 2 (+ 2 2) (+ 2 2) 2) (+ 2 2))");
	using tokens = decltype(tokenize(x));
	auto constexpr res = parse(tokens{});
	//pretty_print(typeid(tokens).name());
	
	auto str = std::string(demangle<tokens>());
	pretty_print(str);

	std::cout << ";; " << res << "\n";

}