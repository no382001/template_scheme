#pragma once
#include "h/lists.h"
#include "h/car_cdr.h"
#include "h/utils.h"
#include "h/env.h"
#include "h/eval_apply.h"

struct testobj {};
struct fooobj {};
struct plus {};

/**/
// QUOTE
// (quoted? exp)
// -------------
//using quotetewwst = decltype(make_quote()); //empty quote
using quotetest = decltype(make_quote(testobj{},testobj{}));
using quotetestres = quote<testobj,testobj>;
static_assert(is_same_type<quotetest,quotetestres>,"");
static_assert(is_quoted(quotetest{}),"quoted?");

// CAR
// (car '()) ; err
// (car '(testobj)) ; testobj
// --------------------------
//using cartest = decltype(car(make_list())); // car of empty list
using cartest = decltype(car(make_quote(make_list(testobj{}))));
using cartestres = testobj;
static_assert(is_same_type<cartest,cartestres>,"(car '(testobj)) ;testobj");

// CDR
// (cdr '()) ; err
// (car '(testobj)) ; ()
// (cdr '(test test2)) ; (test2)
// -----------------------------
//using cdrtest1 = decltype(cdr(make_quote(make_list()))); // cdr of empty list
using cdrtest2 = decltype(cdr(make_quote(make_list(testobj{})))); //cdr of last element
//using cdrtestres2 = list<>; // removed bc types mangle in the tests and corrupt results
static_assert(is_same_type<cdrtest2,list<>>,"(car '(testobj)) ;()");

using cdrtest = decltype(cdr(make_quote(make_list(testobj{},testobj{}))));
using cdrtestres = list<testobj>;
static_assert(is_same_type<cdrtest,cdrtestres>,"(cdr '(test test2)) ;(test2)");

// CADR
// (cadr '()) ; err
// (cadr '(test test2 test)) ; test2
// (cadr  '(test test2)) ; test2
// -----------------------------
//using cadrtest1 = decltype(cadr(make_quote(make_list()))); // cdr of empty list
using cadrtest2 = decltype(cadr(make_quote(make_list(testobj{},fooobj{},testobj{}))));
static_assert(is_same_type<cadrtest2,fooobj>,"(cadr '(testobj foobj testobj)) ; foobj");

using cadrtest = decltype(cadr(make_quote(make_list(testobj{},testobj{}))));
static_assert(is_same_type<cadrtest,testobj>,"(cdr '(test test2)) ; test2");

// LIST behaviour tests

using listtest1 = decltype(make_list());
using listres1 = decltype(list<>{});
static_assert(is_same_type<listtest1,listres1>,"empty list constructor");

using listtest2 = decltype(make_list(make_list()));
using listres2 = decltype(list<>{});
static_assert(is_same_type<listtest2,listres2>,"empty list in list constructor, collapses into empty list");

using listtest3 = decltype(make_list(make_list(testobj{})));
using listres3 = decltype(list<testobj>{});
static_assert(is_same_type<listtest3,listres3>,"obj in list constructor, collapses into obj in list");

using listtest4 = decltype(make_list(make_list(testobj{}),make_list()));
using listres4 = decltype(list<testobj>{});
static_assert(is_same_type<listtest4,listres4>,"obj in list constructor and empty list, collapses into obj in list");

using listtest41 = decltype(make_list(make_list(),make_list(testobj{}),make_list()));
using listres41 = decltype(list<testobj>{});
//static_assert(is_same_type<listtest41,listres41>,"obj in list constructor and empty list on either side, collapses into obj in list");

using listtest5 = decltype(make_list(list<testobj>{},list<>{},list<testobj>{},list<testobj>{}));
using listres5 = decltype(list<testobj,testobj,testobj>{});
static_assert(is_same_type<listtest5,listres5>,"3 list<obj> in list constructor and empty list inbetween them, collapses into 3 list<obj> in list");

// (apply addtion '(1 2 3))
using appltest1 = decltype(IRapply(addition{},quote<list<integer<1>,integer<2>,integer<3>>>{}));


using map_pair_test = decltype(map_pair(list<integer<1>>{},list<integer<1>>{}));
static_assert(is_same_type<map_pair_test,environment<table_entry<integer<1>,variable,integer<1>>>>,"map_pair_test");
//using map_pair_test2 = decltype(map_pair(list<integer<1>>{},list<integer<1>,integer<1>>{}));


// list search tests


template <typename name>
struct p_name {};
template <typename... args>
struct p_args {};
template <typename... body>
struct p_body {};

using env = decltype(make_environment()); // init list
// (define n 1)
using n_1 = decltype(table_entry<variable,c_<110>,integer<1>>{});
// (make-procedure (f n) (+ 1 n))
using proc_f_n_plus_1_n = decltype(table_entry<procedure,p_name<c_<'f'>>,p_args<c_<'n'>>,p_body<list<plus,integer<1>,c_<'n'>>>>{});

// add entries to "table"
using env1 = decltype(extend_environment<env>(n_1{}));
using env2 = decltype(extend_environment<env1>(proc_f_n_plus_1_n{}));

using search_expected = decltype(table_entry<variable, c_<110>, integer<1>>{});
using search_res = decltype(list_search(variable{},env2{}));
static_assert(is_same_type<search_expected,search_res>, "first entry");

using search_res2 = decltype(list_search(procedure{},env2{})); 
static_assert(is_same_type<table_entry<procedure,p_name<c_<'f'>>,p_args<c_<'n'>>,p_body<list<plus,integer<1>,c_<'n'>>>>,search_res2>, "second entry");

using search_res3 = decltype(list_search(int{},env2{}));
static_assert(is_same_type<void,search_res3>, "non existent entry");


using list_of_test = decltype(list_of_values<init_env>(c_<110>{},c_<110>{}));
static_assert(is_same_type<list_of_test,list<integer<1>, integer<1>>>,"serach and replace, with init env containing n as 1");

using sssss = decltype(IReval<init_env>(quote<c_<110>>{}));
static_assert(is_same_type<sssss,integer<1>>,"ireval on variable, with init env containing n as 1");

using ssssaa = decltype(IReval<init_env>(quote<list<list<c_<110>,c_<110>>,c_<110>,c_<110>>>{}));

static_assert(is_same_type<ssssaa,list<list<integer<1>, integer<1>>,integer<1>, integer<1>>>,"");

// (eval '(apply addition '(1 2 3)))
using evaltest1 = decltype(IReval<init_env>(quote<list<apply,addition,quote<list<integer<1>,integer<2>,integer<3>>>>>{}));
using evaltest21 = decltype(integer<6>{});
static_assert(is_same_type<evaltest1,evaltest21>,"(eval '(apply addition '(1 2 3)))");

// (eval '(apply proc 'body 'param '(1 2 3) 'env))
using proctest1 = decltype(IReval<init_env>(quote<list<apply,addition,quote<list<integer<1>,integer<2>,integer<3>>>>>{}));
static_assert(is_same_type<proctest1,evaltest21>,"(eval '(apply addition '(1 2 3)))");

using vartest = decltype(IReval<init_env>(quote<list<apply,addition,quote<list<c_<110>,integer<2>>>>>{}));
static_assert(is_same_type<vartest,integer<3>>,"(define x 1) (eval '(apply addition '(x 2)))");

using test_em = decltype(eval_members<init_env>(quote<list<integer<1>,integer<2>>>{}));
static_assert(is_same_type<test_em,list<integer<1>,integer<2>>>,"self evaluating");

using test_em_var = decltype(eval_members<init_env>(quote<list<c_<110>,integer<2>>>{}));
static_assert(is_same_type<test_em_var,list<integer<1>,integer<2>>>,"self eval and variable");

using evaluated_symbol = decltype(IReval<init_env>(quote<c_<110>>{}));
using testaddev22 = decltype(apply_addition(list<evaluated_symbol,integer<3>>{}));
using testaddevres22 = decltype(integer<4>{});
static_assert(is_same_type<testaddev22,testaddevres22>,"additon of symbol and integer");

using nesttest1 = decltype(IReval<init_env>(quote<list<apply,addition,quote<list<integer<1>,integer<2>,list<apply,addition,quote<list<integer<1>,integer<2>,integer<3>>>>>>>>{}));
static_assert(is_same_type<nesttest1,integer<9>>,"(eval '(apply 'addition '(1 2 '(apply 'addition '(1 2 3)))");

using nesttest2 = decltype(IReval<init_env>(quote<list<apply,addition,quote<list<integer<1>,list<apply,addition,quote<list<integer<1>,integer<2>,integer<3>>>>,integer<3>>>>>{}));
static_assert(is_same_type<nesttest2,integer<10>>,"nesstest 2");

using comproc = decltype(IReval<init_env>(quote<list<apply,inc_c_list,quote<c_<110>>>>{}));
static_assert(is_same_type<comproc,integer<2>>,"apply com proc, 1 arg and 1 operand");

// if there is no apply it will think that its a variable and replace it with the cadr of the entry,
// quot gets removed when integer self elvaulates in normal workflow
// have another column in the env table for the type of the variable, and the procedure that is needed to emplace into the expression
using comproc2 = decltype(IReval<init_env>(quote<list<inc_c_list,quote<integer<1>>>>{}));
static_assert(is_same_type<comproc2,integer<2>>,"apply com proc 1 arg 1 operand");
                                            
using comproc3 = decltype(IReval<init_env>(quote<list<inc_c_list,list<inc_c_list,quote<integer<1>>>>>{}));
static_assert(is_same_type<comproc3,integer<3>>,"(inc (inc '1))"); // (inc (inc '1) '1) works too with 4 as a result so a new branch is launched when inc processes the operands

using comproc4 = decltype(IReval<init_env>(quote<list<sum_of_2,quote<list<integer<1>,integer<2>>>>>{}));
static_assert(is_same_type<comproc4,integer<3>>,"2 arg copmp proc");

using simpleeval = decltype(IReval<init_env>(quote<list<equal,quote<list<integer<1>,integer<2>>>>>{}));
static_assert(is_same_type<simpleeval,scm_false>,"2 arg copmp proc");

using ifproc = decltype(IReval<init_env>(quote<list<scm_if,quote<list<equal,quote<list<integer<1>,integer<2>>>>>,integer<1>,integer<2>>>{}));
static_assert(is_same_type<ifproc,integer<2>>,"2 arg copmp proc");

// define fibonacchi
/*
(define (fib x)
    (if (<= x 2)
    1
    (+ (fib (- x 1)) (fib (- x 2))))))
*/

// ---------------DEBUGGING-------------------------------------------------------------

using fibonacchi_base_case = decltype(IReval<init_env>(quote<list<fib_name,quote<integer<1>>>>{}));
static_assert(is_same_type<fibonacchi_base_case,integer<1>>,"fib 1");

using fibonacchi_base_case = decltype(IReval<init_env>(quote<list<fib_name,quote<integer<2>>>>{}));
static_assert(is_same_type<fibonacchi_base_case,integer<1>>,"fib 2");

using resfib = decltype(make_list(fibonacchi_base_case{}));
using fibonacchi_base_case = decltype(IReval<init_env>(quote<list<fib_name,quote<integer<3>>>>{}));

using resfibasd = decltype(IReval<init_env>(quote<list<lesseq,c_<110>,integer<2>>>{}));
// why the fuck does it evaluate to

/*
in file included from h/env.h:6,
                 from tests.h:5,
                 from ground.cpp:1:
h/primitive_operations.h: In instantiation of ‘constexpr auto apply_primitve_procedure(
    lesseq, Arguments) [with Arguments = list<list<integer<2>, integer<1> >, integer<2> >]’:
h/eval_apply.h:105:53:   required from ‘constexpr auto IRapply(Proc, quote<Exp>) [with 
    Proc = lesseq; Args = list<list<integer<2>, integer<1> >, integer<2> >]’
h/eval_apply.h:179:46:   required from ‘constexpr auto IReval(quote<Exp>)

operands evaluate to a list, but the list does not collapse, maybe using make_list instead of explicit list<> could solve this?

Evaluated_opnds = list<subtraction, subtraction, subtraction, subtraction, subtraction, subtraction, subtraction, subtraction, subtraction, integer<3>, integer<2>, integer<2>, integer<2>, integer<2>, integer<2>, integer<2>, integer<2>, integer<2>, integer<2> >]’


appending there does not help

*/

using asdasdasdasd = decltype(list<list<integer<2>, integer<1> >, integer<2> >{});
using asdasdasdasd2 = decltype(make_list(list<integer<2>, integer<1>>{},integer<2>{}));
using asdasdasdasd3 = decltype(make_list(make_list(integer<2>{}, integer<1>{}),integer<2>{}));

using asdasdasd = decltype(IReval<init_env>(quote<list<c_<110>,integer<1>>>{}));