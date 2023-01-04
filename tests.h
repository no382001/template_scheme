#pragma once
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

#define TEST(...) ([]() constexpr -> int { return parse(decltype(tokenize(constexpr_string(__VA_ARGS__))){}); })

// basic nesting test
static_assert(2 == TEST("(+ 1 1)")());
static_assert(3 == TEST("(+ 1 (+ 1 1))")());
static_assert(1 == TEST("(+ (- 1 1) 1)")());
static_assert(2 == TEST("(+ (- (+ 1 1) 1) 1)")());
static_assert(5 == TEST("(+ 1 (+ 1 (+ 1 0) 1) 1)")());
static_assert(4 == TEST("(+ 1 (+ 1 1 ) 1)")());
static_assert(5 == TEST("(+ (+ (+ 1 0) 1 (+ 2 0)) 1)")());
static_assert(6 == TEST("(+ 1 (+ 1 1 ) (+ 1 1) 1)")());
static_assert(6 == TEST("(+ 1 (+ 1 1 ) 1 (+ 1 1))")());
static_assert(4 == TEST("(+ 1 (+ 1 (+ 1 0)) 1)")());
static_assert(5 == TEST("(+ 1 (+ 1 (+ 1 0) 1) 1)")());
static_assert(19 == TEST("(+ 3 (+ 2 (+ 2 2) (+ 2 2) 2) (+ 2 2))")());

// mixed operator tests
static_assert(9 == TEST("(+ (* 2 3) (/ (+ 4 5) 3))")());
static_assert(30 == TEST("(+ (* (+ 2 3) (- 10 5)) (/ (* 10 5) (- 20 10)))")());
static_assert(-16 == TEST("(- (/ (* (- (+ 10 5) (* 2 3)) (/ (* 10 5) (- 20 10))) (+ 2 3)) (* (+ 2 3) (- 10 5)))")());

// nested if statement tests
static_assert(1 == TEST("(if (> 3 2) (if (> 2 1) (if (> 1 0) 1 0) 0) 0))")());

// some other garbage

//auto x = constexpr_string("(+ ab 1( 1 klajsd ok laksd) 1)");
//using tokenass = decltype(tokenize(x));
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
////using first_table = decltype(car(tab{}));
//using rest_table = decltype(cdr(tab{}));
////using c_t = decltype(c_list<c_<97>, c_<98>, c_<99>>{});
//auto constexpr asdasdaasd = is_c_list(integer<1>{});
//auto constexpr b = is_c_list(c_t{});
//auto constexpr b2 = is_c_list(tokens{});

//auto y = constexpr_string("(1111111111111111)"); //16to parse 7 actually parsed
//using token1s = decltype(tokenize(y));

//auto seven = constexpr_string("(1111111)");
//using tokens2 = decltype(tokenize(seven));

// "(1 abc 1 abc 1 ( 1 gelcim 1 okt 1)) (+ ab 1( 1 klajsd ok laksd) 1)"
//auto xss = constexpr_string("(+ (* 2 3) (/ (+ 4 5) 2))");
//using tokssens = decltype(tokenize(xss));
//auto constexpr sss = parse(tokssens{});

////using resss = decltype(table_search(c_list<c_<'x'>>{},table_entries{}));
//using resss2 = decltype(table_search(c_list<c_<'y'>>{},table_entries{}));

/* 
auto x = constexpr_string("(define y 22) (+ 1 y (- 1 1) 2)");
auto xssssss = constexpr_string("(define y 3) (define x 2) (define z  2) (+ 1 y (- x 1) z)");
auto xasas = constexpr_string("(+ 1 2 (- 1 1) 2)");

// populate the table with the contents of (define ...) expressions
using table_entries = decltype(gather_table_entries(x));
using table_entries2 = decltype(gather_table_entries(xssssss));

// build the ast for parsing, without (define ...) expressions, substitute symbols
using tokessns = decltype(tokenize_w_table<table_entries>(x));
using tokessnsss = decltype(tokenize_w_table<table_entries2>(xssssss));

using token2s = decltype(tokenize(xasas));
auto constexpr resultbasic = parse(token2s{});

*/
//auto constexpr result = parse(tokens{});

//constexpr bool b = is_same_type<s,token_list<c_list<c_<121>>>>;
//constexpr bool b2 = is_same_type<ss,c_list<c_<120>>>;

//using e = decltype(cdr(car(car(car(table_entries{})))));

//using res = decltype(tab::search<int>);
//auto constexpr res = parse(tokens{});
//pretty_print(typeid(tokens).name());




