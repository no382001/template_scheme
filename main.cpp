#include <string_view>
#include <type_traits>
#include <iostream>
#include "h/atoms.h"
#include "h/utils.h"
#include "h/lists.h"
#include "h/tokenizer.h"
#include "h/number_operations.h"
#include "h/parser.h"
#include "h/table.h"
#include "tests.h"
#include "h/pretty_print.h"
#include "h/car_crd.h"

int main(){
	auto string = constexpr_string("(define y 3) (define x 2) (define z  2) (+ 1 y (- x 1) z)");

	// populate the table with the contents of (define ...) expressions
	using table_entries = decltype(gather_table_entries(string));

	// build the ast for parsing, without (define ...) expressions, but substitute the symbols with their values
	using tokens = decltype(tokenize_w_table<table_entries>(string));

	auto constexpr result = parse(tokens{});

	auto ast = std::string(demangle<tokens>());
	pretty_print(ast);

	std::cout << ";; " << result << "\n";
}