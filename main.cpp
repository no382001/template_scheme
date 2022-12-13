#include <string_view>
#include <type_traits>
#include <cmath>
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
#include <iostream>
#include <string_view>


// clist is actually symbol list, strings are denoted by double quotes
// modify tokenizer to replace c_lists with a symmbol that contains its layer information aswell as its string value
// copy and modify the tokenizer to tokenize just the symbols in the same way but this time return a table list with all the symbols

int main(){

	// "(1 abc 1 abc 1 ( 1 gelcim 1 okt 1)) (+ ab 1( 1 klajsd ok laksd) 1)"
	auto x = constexpr_string("(define x (+ 1 1 x)) (define y (22)) (+ 1 y x)");
	using tokens = decltype(tokenize(x));
	using table_entries = decltype(gather_table_entries(x));
	
	using resss = decltype(table_search(c_list<c_<'x'>>{},table_entries{}));
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