#pragma once
#include "h/atoms.h"
#include "h/utils.h"
#include "h/lists.h"
#include "h/tokenizer.h"
#include "h/number_operations.h"

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
