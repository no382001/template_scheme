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



int main(){
	auto constexpr string = constexpr_string("((lambda (x y) (+ x y)) 2 1)");
	using tokens = decltype(tokenize(string)); // car fails in instances where the lists dont fold inwards
	
	/** /
	token_list<
		list<token_list<
			list<
				lambda<
					token_list<
						list<token_list<
							c_list<c_<120>>, c_list<c_<121>>
						>>,
						list<token_list<
							plus,
							c_list<c_<120>>, c_list<c_<121>>
						>>
					>
				>
			>,
			integer<1>,
			integer<1>>>> 




	using list = decltype(car(tokens{}));
	using operator_lam = decltype(car(car(list{})));

	using arguments = decltype(car(car(car(car(car(car(list{})))))));
	using expression = decltype(car(car(cdr(car(car(car(car(list{}))))))));
	using parameters = decltype(cdr(car(car(tokens{}))));

	using arg_x_parameter_table = decltype(map_pair_l(arguments{},parameters{}));

	//using etest = decltype(car(car(arg_x_parameter_table{})));

	using found = decltype(search_pair_l(c_list<c_<120>>{},arg_x_parameter_table{}));

	using result = decltype(substitute(arg_x_parameter_table{},expression{}));
	*/

	

	// function to simplify if possible, or make_token_list overload, but none really works

	


	//static_assert(is_same_type<operator_lam,_lambda>,"");
		// if it is lambda send a callback
		// then step back into the list and handle it as a lambda expression
		// - arguments
		// - expressions
		// - parameters
		// 
		// - map the arg and par together
		// tokenize expressions (this whole procedure should happen in tokenize)
		// when returned here, go thru the token list and substitute the symbol if found
		// 
		// TODO: i need to rewrite the table search thing, whihch im going to delete,
		// if empty list is returned then its not found
		// then return the list for another lambda to handle possibly?   im pretty sure i might need some cases for this to fuly work


	// each layer should return a tokenlist that is already substituted by the inner layer
	// the outermost will be ready for parsing

	// the lambda should simply disappear after it is resolved, leaving only the substituted list



};