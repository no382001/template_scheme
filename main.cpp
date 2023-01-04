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
/*
milestones:
	- lambda expressions are working well, migrate them to parse module
	- define expressions are working well, showcase with add function: almost ready 
	- implement the y combinator, showcase with fibonacchi

to implement:
	- fix lists, i makes so much extra work when its not working right
	- quoting, now ints just evaluate to their values like expressions, also there is no way to return a list
		- maybe an function that builds a string output, or just evaluates to an int like now
	- negative integers
	- strings
	- fix the integer concatinator, which probably breaks bc the int is stored in the template arg
	- add some more primitives

good to have:
	- read from file

*/

//static_assert(3 == TEST("((lambda (x y) (+ x y)) 2 1)")());
//static_assert(2 == TEST("(+ 1 ((lambda (x) (x)) 1))")());
//static_assert(7 == TEST("(((lambda (x) (lambda (y) (+ x y))) 3) 4)")());

int main(){
	auto constexpr string = constexpr_string("(((lambda (x) (lambda (y) (+ x y))) 3) 4)");

	using tokenss = decltype(tokenizer(string));
	// token_list<list<token_list<list<token_list<list<lambda<token_list<list<token_list<c_list<c_<120> > > >, list<lambda<token_list<list<token_list<c_list<c_<121> > > >, list<token_list<plus, c_list<c_<120> >, c_list<c_<121> > > > > > > > > >, integer<3> > >, integer<4> > > >
	
	//auto constexpr res = parse(tokenss{});

	auto ast = std::string(demangle<tokenss>());
	std::cout << ast << '\n';
};