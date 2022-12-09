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

	// "(1 abc 1 abc 1 ( 1 gelcim 1 okt 1))"
	auto x = constexpr_string("(+ ab 1( 1 1 ok 1) 1)");
	using table_entries = decltype(tokenize_table_entries(x));
	
	//using tokens = decltype(tokenize(x));
/*
	using token_away = decltype(car(tokens{}));
	using list_Away = decltype(car(token_away{}));
	using first = decltype(car(list_Away{}));
	using fas = decltype(car(list<>{}));
	using asdsad = decltype(car(make_list()));
	using fa21s = decltype(car(list<integer<1>,integer<2>>{}));
	using fa221s = decltype(cdr(list<integer<1>,integer<2>>{}));
	using fa212321s = decltype(cdr(list<>{}));
*/


	//using tab = decltype(make_table(table_entry<int,tokens>{},table_entry<char,tokens>{}));
	//using first_table = decltype(car(tab{}));
	//using rest_table = decltype(cdr(tab{}));

	//using c_t = decltype(c_list<c_<97>, c_<98>, c_<99>>{});
	//auto constexpr asdasd = is_c_list(integer<1>{});
	//auto constexpr b = is_c_list(c_t{});
	//auto constexpr b2 = is_c_list(tokens{});


	//using res = decltype(tab::search<int>);
	//auto constexpr res = parse(tokens{});
	//pretty_print(typeid(tokens).name());
	
	//auto str = std::string(demangle<prot_table>());
	//pretty_print(str);

	//std::cout << ";; " << res << "\n";


}