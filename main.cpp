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

// the outer layer
template < typename A, typename ...Rest >
auto constexpr parse(token_list< A, Rest... >) {
	// extract the first argument of token_list
	if constexpr (sizeof...(Rest) >= 0) {
		return handle_prefix(A{});
		//return define_atom(constexpr_string("abcd")) == A;
	}
	else {
		return 0;
	}
}

int main(){
	auto x = constexpr_string("(+ 3 (+ 3 (+ 2 2) 2))");
	//auto x = constexpr_string("( abcd )");
	using tokens = decltype(tokenize(x));
	auto constexpr res = parse(tokens{});
	//pretty_print(typeid(tokens).name());
	std::cout << ";; " << res << "\n";

	/** /
	auto y = constexpr_string("(+ 3 (+ 2 2) 2)");
	using tokens2 = decltype(tokenize(y));
	auto constexpr res2 = parse(tokens2{});
	//pretty_print(typeid(tokens).name());
	std::cout << ";; " << res2 << "\n";
	/**/
}


// clang++ -std=c++20 -S -emit-llvm main.cpp -o - | opt -analyze -dot-callgraph
// dot -Tpng -ocallgraph.png callgraph.dot

// print template instantiation
// https://stackoverflow.com/questions/4448094/can-we-see-the-template-instantiated-code-by-c-compiler
// clang++ -std=c++20 -Xclang -ast-print -fsyntax-only main.cpp

// https://stackoverflow.com/questions/5373714/how-to-generate-a-calling-graph-for-c-code