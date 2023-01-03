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
#include "h/if.h"
#include "tests.h"


static_assert(3 == parse(decltype(tokenizer(constexpr_string("((lambda (x y) (+ x y)) 2 1)"))){}),"");
static_assert(2 == parse(decltype(tokenizer(constexpr_string("(+ 1 ((lambda (x) (x)) 1))"))){}),"");
static_assert(7 == parse(decltype(tokenizer(constexpr_string("(((lambda (x) (lambda (y) (+ x y))) 3) 4)"))){}),"");

int main(){
	auto constexpr string = constexpr_string("(if (= 1 1) (+ 1 1) (3))");
	/*
	(define x 1)
	(if (> x 0)
      (1)
	  (0))

	(if (= 1 1) '1 '0)
	// implement quoting, now it just evaluates but lone ints evaluate to their values they two are expressions

	*/
	
	using tokens = decltype(tokenizer(string));
	auto constexpr res = parse(tokens{});

	auto ast = std::string(demangle<tokens>());
	std::cout << ast << '\n';
};