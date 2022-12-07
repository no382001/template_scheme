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
#include <iostream>
#include <string_view>

int main(){

	auto x = constexpr_string("(abc abc 1 (sbv okt))");
	using tokens = decltype(tokenize(x));
	
	using prot_table = decltype(collect_entries(tokens{})); 

	using tab = decltype(make_table(table_entry<int,tokens>{},table_entry<char,tokens>{}));
	using first = decltype(car(tab{}));
	using rest = decltype(cdr(tab{}));


	using c_t = decltype(c_list<c_<97>, c_<98>, c_<99>>{});
	auto constexpr b = is_char_list(c_t{});
	auto constexpr b2 = is_char_list(tokens{});


	//using res = decltype(tab::search<int>);
	//auto constexpr res = parse(tokens{});
	//pretty_print(typeid(tokens).name());
	
	//auto str = std::string(demangle<tokens>());
	//pretty_print(str);

	//std::cout << ";; " << res << "\n";


}