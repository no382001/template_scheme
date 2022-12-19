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
#include "h/pretty_print.h"
#include "h/car_crd.h"
#include "h/lambda.h"
//#include "tests.h"

//static_assert(3 == parse(decltype(tokenize(constexpr_string("((lambda (x y) (+ x y)) 2 1)"))){}),"");
//static_assert(1 == parse(decltype(tokenize(constexpr_string("((lambda (x) (x)) 1)"))){}),"");

int main(){
	auto constexpr string = constexpr_string("(((lambda (x) (lambda (y) (+ x y))) 3) 4)");
	using tokens = decltype(tokenize(string)); // car fails in instances where the lists dont fold inwards
	auto ast = std::string(demangle<tokens>());
	std::cout << ast << '\n';

	// the problem now is: i have lots of list types that im using and also whenever i car or cdr, different results need to be handled in some way


	//using tokens = decltype(token_list<list<table<token_list<pair_l<token_list<c_list<c_<120>>>, token_list<integer<3>>>, pair_l<c_list<c_<121>>, integer<4>>>, token_list<plus, c_list<c_<120>>, c_list<c_<121>>>>>>{}); 

	//using t =  decltype(car(car(car(tokens{}))));
	//using value = decltype(search_pair_l(token_list<c_list<c_<120>>>{},t{}));
	///using x =  decltype(remove_outer(cdr(car(car(tokens{})))));

	//using result = decltype(substitute(t{},x{}));
};