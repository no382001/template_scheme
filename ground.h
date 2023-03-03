#pragma once
#include "tests.h"
#include "h/lists.h"
#include "h/car_cdr.h"
#include "h/utils.h"
#include "h/atoms.h"
#include "h/primitive_operations.h"
#include "tests.h"
#include "h/eval_apply.h"



// (apply addtion '(1 2 3))
using sdfgs = decltype(IRapply(addition{},quote<list<integer<1>,integer<2>,integer<3>>>{}));

using asdasd = decltype(apply_primitve_procedure(int{},list<integer<1>,integer<1>>{}));




