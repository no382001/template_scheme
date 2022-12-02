#include <iostream>
#include <string_view>
#include <type_traits>
#include "h/atoms.h"
#include "h/utils.h"
#include "h/lists.h"
#include "h/tokenizer.h"
#include "h/number_operations.h"
#include "h/pretty_print.h"
#include "h/parser.h"
#include "tests.h"

int main(){

	//auto x = constexpr_string("(+ 1 1)");
	//using tokens = decltype(tokenize(x));
	//auto constexpr res = parse(tokens{});
	//pretty_print(typeid(tokens).name());
	//auto str = std::string(demangle<tokens>());
	//pretty_print(str);	
	//std::cout << ";; " << res << "\n";
}


// clang++ -std=c++20 -S -emit-llvm main.cpp -o - | opt -analyze -dot-callgraph
// dot -Tpng -ocallgraph.png callgraph.dot

// print template instantiation
// https://stackoverflow.com/questions/4448094/can-we-see-the-template-instantiated-code-by-c-compiler
// clang++ -std=c++20 -Xclang -ast-print -fsyntax-only main.cpp

// https://stackoverflow.com/questions/5373714/how-to-generate-a-calling-graph-for-c-code