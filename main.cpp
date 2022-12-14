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


int main(){
	auto constexpr string = constexpr_string("((lambda (x y) (+ x y)) 1 1)");
	using tokens = decltype(tokenize(string));

	using list = decltype(car(car(tokens{})));
	using operator_lam = decltype(car(car(car(list{}))));


	using arguments = decltype(car(car(cdr(car(car(list{}))))));
	using expressions = decltype(cdr(cdr(car(car(list{})))));
	using parameters = decltype(cdr(car(car(tokens{}))));

	using something = decltype(map_pair_l(arguments{},parameters{}));
	// implement putting 2 lists into pairs

};