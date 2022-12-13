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
	//auto x = constexpr_string("(define x (+ 1 1 x)) (define y (22)) (+ 1 y x)");
	auto x = constexpr_string("(define y 22) (+ 1 y (- 1 1) 2)");
	auto xasas = constexpr_string("(+ 1 2 (- 1 1) 2)");

	// populate the table with the contents of (define ...) expressions
	using table_entries = decltype(gather_table_entries(x));

	// build the ast for parsing, without (define ...) expressions, substitute symbols
	using tokessns = decltype(tokenize_w_table<table_entries>(x));
	auto constexpr result = parse(car(tokessns{}));
	
	using token2s = decltype(tokenize(xasas));
	auto constexpr resultbasic = parse(token2s{});
	
	// parse tokens, and look up table entries if they are encountered
	//auto constexpr result = parse();

	using resss = decltype((table_search(c_list<c_<'y'>>{},table_entries{})));

	using asdasd = decltype(car(cdr(car(resss{}))));
	using resss2 = decltype(table_search(c_list<c_<'y'>>{},table_entries{}));
	
	using ss = decltype(car(car(car(table_entries{}))));
	//using ss = decltype(car(car(table_entries{})));

	using s = decltype(car(car(car(cdr(table_entries{})))));

	//constexpr bool b = is_same_type<s,token_list<c_list<c_<121>>>>;
	//constexpr bool b2 = is_same_type<ss,c_list<c_<120>>>;

	//using e = decltype(cdr(car(car(car(table_entries{})))));

	//using res = decltype(tab::search<int>);
	//auto constexpr res = parse(tokens{});
	//pretty_print(typeid(tokens).name());
	
	//auto str = std::string(demangle<resss>());
	// pretty_print(str);

	//std::cout << ";; " << res << "\n";


}