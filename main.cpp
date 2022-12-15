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

LIST(pair_l);

template <template <class,class> typename One, typename A, typename... Args, template <class,class> typename Two, typename B, typename... Brgs>
auto constexpr map_pair_l(One<A,Args...>,Two<B,Brgs...>){
		using p = decltype(make_pair_l(A{},B{}));
	if constexpr (sizeof...(Args) == 0) {
		return make_token_list(p{});
	} else {
		using second = decltype(map_pair_l(One<Args...>{},Two<Brgs...>{}));
		return make_token_list(p{},second{});
	}
}

#define _lambda c_list<c_<108>, c_<97>, c_<109>, c_<98>, c_<100>, c_<97>>

// search for the key of a pair list, get the value back
// - returns void when not found
template <typename candidate, typename... Args>
auto constexpr search_pair_l(candidate,Args...){
    if constexpr (sizeof...(Args) == 0) {
		return make_token_list();
    } else {
		using curr_t = decltype(car(Args{}...));
		using to_compare = decltype(car(car(Args{}...)));
		if constexpr (is_same_type<candidate,to_compare>){
			return cdr(car(Args{}...));
		} else if constexpr (!is_same_type<token_list<>,curr_t>){
			return search_pair_l(candidate{},cdr(Args{}...));
		}
    }
}



int main(){
	auto constexpr string = constexpr_string("((lambda (x y) (+ x y)) 1 1)");
	using tokens = decltype(tokenize(string));

	using list = decltype(car(car(tokens{})));
	using operator_lam = decltype(car(car(car(list{}))));

	using arguments = decltype(car(car(cdr(car(car(list{}))))));
	using expressions = decltype(cdr(cdr(car(car(list{})))));
	using parameters = decltype(cdr(car(car(tokens{}))));

	using arg_x_parameter_table = decltype(map_pair_l(arguments{},parameters{}));

	using etest = decltype(car(car(arg_x_parameter_table{})));

	using found = decltype(search_pair_l(c_list<c_<120>>{},arg_x_parameter_table{}));

	static_assert(is_same_type<operator_lam,_lambda>,"");
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