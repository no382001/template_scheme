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
	
	auto constexpr result = parse(tokens{});

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