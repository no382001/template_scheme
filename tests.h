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


static_assert(2 == TEST("(+ 1 1)")());
static_assert(3 == TEST("(+ 1 (+ 1 1))")());
static_assert(1 == TEST("(+ (- 1 1) 1)")());
static_assert(2 == TEST("(+ (- (+ 1 1) 1) 1)")());
static_assert(5 == TEST("(+ 1 (+ 1 (+ 1 0) 1) 1)")());
static_assert(19 == TEST("(+ 3 (+ 2 (+ 2 2) (+ 2 2) 2) (+ 2 2))")());
// ...

static_assert(4 == parse(decltype(tokenize(constexpr_string("(+ 1 (+ 1 1 ) 1)"))){}),"");
static_assert(5 == parse(decltype(tokenize(constexpr_string("(+ (+ (+ 1 0) 1 (+ 2 0)) 1)"))){}),"");
static_assert(6 == parse(decltype(tokenize(constexpr_string("(+ 1 (+ 1 1 ) (+ 1 1) 1)"))){}),"");
static_assert(6 == parse(decltype(tokenize(constexpr_string("(+ 1 (+ 1 1 ) 1 (+ 1 1))"))){}),"");
static_assert(6 == parse(decltype(tokenize(constexpr_string("(+ 1 (+ 1 1 ) 1 (+ 1 1))"))){}),"");
static_assert(4 == parse(decltype(tokenize(constexpr_string("(+ 1 (+ 1 (+ 1 0)) 1)"))){}),"nested LR");
static_assert(5 == parse(decltype(tokenize(constexpr_string("(+ 1 (+ 1 (+ 1 0) 1) 1)"))){}),"nested LR");


auto x = constexpr_string("(+ ab 1( 1 klajsd ok laksd) 1)");
using tokens = decltype(tokenize(x));
using token_away = decltype(car(tokens{}));
using list_Away = decltype(car(token_away{}));
using first = decltype(car(list_Away{}));
using fas = decltype(car(list<>{}));
using asdsad = decltype(car(make_list()));
using fa21s = decltype(car(list<integer<1>,integer<2>>{}));
using fa221s = decltype(cdr(list<integer<1>,integer<2>>{}));
using fa212321s = decltype(cdr(list<>{}));
//using tab = decltype(make_table(table_entry<int,tokens>{},table_entry<char,tokens>{}));
////using first_table = decltype(car(tab{}));
//using rest_table = decltype(cdr(tab{}));
////using c_t = decltype(c_list<c_<97>, c_<98>, c_<99>>{});
auto constexpr asdasd = is_c_list(integer<1>{});
//auto constexpr b = is_c_list(c_t{});
auto constexpr b2 = is_c_list(tokens{});

//auto y = constexpr_string("(1111111111111111)"); //16to parse 7 actually parsed
//using token1s = decltype(tokenize(y));

//auto seven = constexpr_string("(1111111)");
//using tokens2 = decltype(tokenize(seven));



// "(1 abc 1 abc 1 ( 1 gelcim 1 okt 1)) (+ ab 1( 1 klajsd ok laksd) 1)"
auto xss = constexpr_string("(define x (+ 1 1 x)) (define y (22)) (+ 1 y x)");
using tokssens = decltype(tokenize(xss));
using table_entries = decltype(gather_table_entries(xss));

using resss = decltype(table_search(c_list<c_<'x'>>{},table_entries{}));
using resss2 = decltype(table_search(c_list<c_<'y'>>{},table_entries{}));
