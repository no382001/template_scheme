#pragma once
#include "h/lists.h"
#include "h/car_cdr.h"
#include "h/utils.h"
#include "h/env.h"
#include "h/eval_apply.h"
#include "h/tokenizer.h"

struct testobj {};
struct fooobj {};
struct plus {};

/**/
// QUOTE
// (quoted? exp)
// -------------
//using quotetewwst = decltype(make_wrap()); //empty quote
using quotetest = decltype(make_wrap(testobj{},testobj{}));
using quotetestres = wrap<testobj,testobj>;
static_assert(is_same_type<quotetest,quotetestres>,"");
static_assert(is_wrapped(quotetest{}),"quoted?");

// CAR
// (car '()) ; err
// (car '(testobj)) ; testobj
// --------------------------
//using cartest = decltype(car(make_list())); // car of empty list
using cartest = decltype(car(make_wrap(make_list(testobj{}))));
using cartestres = testobj;
static_assert(is_same_type<cartest,cartestres>,"(car '(testobj)) ;testobj");

// CDR
// (cdr '()) ; err
// (car '(testobj)) ; ()
// (cdr '(test test2)) ; (test2)
// -----------------------------
//using cdrtest1 = decltype(cdr(make_wrap(make_list()))); // cdr of empty list
using cdrtest2 = decltype(cdr(make_wrap(make_list(testobj{})))); //cdr of last element
//using cdrtestres2 = list<>; // removed bc types mangle in the tests and corrupt results
static_assert(is_same_type<cdrtest2,list<>>,"(car '(testobj)) ;()");

using cdrtest = decltype(cdr(make_wrap(make_list(testobj{},testobj{}))));
using cdrtestres = list<testobj>;
static_assert(is_same_type<cdrtest,cdrtestres>,"(cdr '(test test2)) ;(test2)");

// CADR
// (cadr '()) ; err
// (cadr '(test test2 test)) ; test2
// (cadr  '(test test2)) ; test2
// -----------------------------
//using cadrtest1 = decltype(cadr(make_wrap(make_list()))); // cdr of empty list
using cadrtest2 = decltype(cadr(make_wrap(make_list(testobj{},fooobj{},testobj{}))));
static_assert(is_same_type<cadrtest2,fooobj>,"(cadr '(testobj foobj testobj)) ; foobj");

using cadrtest = decltype(cadr(make_wrap(make_list(testobj{},testobj{}))));
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
static_assert(is_same_type<listtest41,listres41>,"obj in list constructor and empty list on either side, collapses into obj in list");

using listtest5 = decltype(make_list(list<testobj>{},list<>{},list<testobj>{},list<testobj>{}));
using listres5 = decltype(list<testobj,testobj,testobj>{});
static_assert(is_same_type<listtest5,listres5>,"3 list<obj> in list constructor and empty list inbetween them, collapses into 3 list<obj> in list");

// (apply addtion '(1 2 3))
using appltest1 = decltype(IRapply(addition{},wrap<list<integer<1>,integer<2>,integer<3>>>{}));


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

using sssss = decltype(IReval<init_env>(wrap<c_<110>>{}));
static_assert(is_same_type<sssss,integer<1>>,"ireval on variable, with init env containing n as 1");

using ssssaa = decltype(IReval<init_env>(wrap<list<list<c_<110>,c_<110>>,c_<110>,c_<110>>>{}));

static_assert(is_same_type<ssssaa,list<list<integer<1>, integer<1>>,integer<1>, integer<1>>>,"");

// (eval '(apply addition '(1 2 3)))
using evaltest1 = decltype(IReval<init_env>(wrap<list<apply,addition,wrap<list<integer<1>,integer<2>,integer<3>>>>>{}));
using evaltest21 = decltype(integer<6>{});
static_assert(is_same_type<evaltest1,evaltest21>,"(eval '(apply addition '(1 2 3)))");

// (eval '(apply proc 'body 'param '(1 2 3) 'env))
using proctest1 = decltype(IReval<init_env>(wrap<list<apply,addition,wrap<list<integer<1>,integer<2>,integer<3>>>>>{}));
static_assert(is_same_type<proctest1,evaltest21>,"(eval '(apply addition '(1 2 3)))");

using vartest = decltype(IReval<init_env>(wrap<list<apply,addition,wrap<list<c_<110>,integer<2>>>>>{}));
static_assert(is_same_type<vartest,integer<3>>,"(define x 1) (eval '(apply addition '(x 2)))");

using test_em = decltype(eval_members<init_env>(wrap<list<integer<1>,integer<2>>>{}));
static_assert(is_same_type<test_em,list<integer<1>,integer<2>>>,"self evaluating");

using test_em_var = decltype(eval_members<init_env>(wrap<list<c_<110>,integer<2>>>{}));
static_assert(is_same_type<test_em_var,list<integer<1>,integer<2>>>,"self eval and variable");

using evaluated_symbol = decltype(IReval<init_env>(wrap<c_<110>>{}));
using testaddev22 = decltype(apply_addition(list<evaluated_symbol,integer<3>>{}));
using testaddevres22 = decltype(integer<4>{});
static_assert(is_same_type<testaddev22,testaddevres22>,"additon of symbol and integer");

using nesttest1 = decltype(IReval<init_env>(wrap<list<apply,addition,wrap<list<integer<1>,integer<2>,list<apply,addition,wrap<list<integer<1>,integer<2>,integer<3>>>>>>>>{}));
static_assert(is_same_type<nesttest1,integer<9>>,"(eval '(apply 'addition '(1 2 '(apply 'addition '(1 2 3)))");

using nesttest2 = decltype(IReval<init_env>(wrap<list<apply,addition,wrap<list<integer<1>,list<apply,addition,wrap<list<integer<1>,integer<2>,integer<3>>>>,integer<3>>>>>{}));
static_assert(is_same_type<nesttest2,integer<10>>,"nesstest 2");

using comproc = decltype(IReval<init_env>(wrap<list<apply,inc_c_list,wrap<c_<110>>>>{}));
static_assert(is_same_type<comproc,integer<2>>,"apply com proc, 1 arg and 1 operand");

// if there is no apply it will think that its a variable and replace it with the cadr of the entry,
// quot gets removed when integer self elvaulates in normal workflow
// have another column in the env table for the type of the variable, and the procedure that is needed to emplace into the expression
using comproc2 = decltype(IReval<init_env>(wrap<list<inc_c_list,wrap<integer<1>>>>{}));
static_assert(is_same_type<comproc2,integer<2>>,"apply com proc 1 arg 1 operand");
                                            
using comproc3 = decltype(IReval<init_env>(wrap<list<inc_c_list,list<inc_c_list,wrap<integer<1>>>>>{}));
static_assert(is_same_type<comproc3,integer<3>>,"(inc (inc '1))"); // (inc (inc '1) '1) works too with 4 as a result so a new branch is launched when inc processes the operands

using comproc4 = decltype(IReval<init_env>(wrap<list<sum_of_2,wrap<list<integer<1>,integer<2>>>>>{}));
static_assert(is_same_type<comproc4,integer<3>>,"2 arg copmp proc");

using simpleeval = decltype(IReval<init_env>(wrap<list<equal,wrap<list<integer<1>,integer<2>>>>>{}));
static_assert(is_same_type<simpleeval,scm_false>,"2 arg copmp proc");

using ifproc = decltype(IReval<init_env>(wrap<list<scm_if,wrap<list<equal,wrap<list<integer<1>,integer<2>>>>>,integer<1>,integer<2>>>{}));
static_assert(is_same_type<ifproc,integer<2>>,"2 arg copmp proc");



// test from prim op

//using testrelv24 = decltype(less(list<integer<3>>{}));
using testrelv2 = decltype(apply_less(list<integer<2>,integer<3>,integer<4>>{}));
using testrelres2 = decltype(scm_true{});
static_assert(is_same_type<testrelv2,testrelres2>,"rel of two integers: positive test");
using testrelv24 = decltype(apply_less(list<integer<3>,integer<2>>{}));
using testrelres24 = decltype(scm_false{});
static_assert(is_same_type<testrelv24,testrelres24>,"rel of two integers: negative test");

using testrelv2s = decltype(apply_more(list<integer<2>,integer<3>,integer<4>>{}));
using testrelres2s = decltype(scm_false{});
static_assert(is_same_type<testrelv2s,testrelres2s>,"rel of two integers: positive test");
using testrelv24aa = decltype(apply_more(list<integer<3>,integer<2>>{}));
using testrelres24aa = decltype(scm_true{});
static_assert(is_same_type<testrelv24aa,testrelres24aa>,"rel of two integers: negative test");

using testrelv2sa = decltype(apply_equal(list<integer<2>,integer<2>,integer<2>>{}));
using testrelres2sa = decltype(scm_true{});
static_assert(is_same_type<testrelv2sa,testrelres2sa>,"rel of two integers: positive test");
using testrelv24aaa = decltype(apply_equal(list<integer<3>,integer<2>>{}));
using dfff = decltype(scm_false{});
static_assert(is_same_type<testrelv24aaa,dfff>,"rel of two integers: negative test");

using testr22elv2 = decltype(apply_primitve_procedure(addition{},list<integer<2>,integer<3>,integer<4>>{}));

using testr22elv22 = decltype(apply_primitve_procedure(int{},list<integer<2>,integer<3>,integer<4>>{}));
static_assert(is_same_type<testr22elv22,void>,"not prim proc");



// define fibonacchi
/*
(define (fib x)
    (if (<= x 2)
    1
    (+ (fib (- x 1)) (fib (- x 2))))))
*/

// ---------------FIBONACCHI-------------------------------------------------------------
/**/
using fibonacchi_base_case = decltype(IReval<init_env>(wrap<list<fib_name,wrap<integer<1>>>>{}));
static_assert(is_same_type<fibonacchi_base_case,integer<1>>,"fib 1");

using fibonacchi_base_case2 = decltype(IReval<init_env>(wrap<list<fib_name,wrap<integer<2>>>>{}));
static_assert(is_same_type<fibonacchi_base_case2,integer<1>>,"fib 2");

using resfib = decltype(make_list(fibonacchi_base_case{}));

using resfibasd = decltype(IReval<init_env>(wrap<list<lesseq,c_<110>,integer<2>>>{}));
using resfibasdsa = decltype(make_list(resfibasd{}));

using asdasdasdasd = decltype(list<list<integer<2>, integer<1> >, integer<2> >{});
using asdasdasdasd2 = decltype(make_list(list<integer<2>, integer<1>>{},integer<2>{}));
using asdasdasdasd3 = decltype(make_list(make_list(integer<2>{}, integer<1>{}),integer<2>{}));


// this isnt valid but this is what it gets, and it can still eval
using asd0asidas = decltype(IRapply(lesseq{},wrap<list<list<subtraction, integer<3>, integer<2>>, integer<2>>>{}));
using asd0asidasasa = decltype(make_list(asd0asidas{}));
/**/


using fibonacchi_case = decltype(IReval<init_env>(wrap<list<fib_name,wrap<integer<3>>>>{}));
static_assert(is_same_type<fibonacchi_case,integer<2>>,"fib 3");

using fibonacchi_case2 = decltype(IReval<init_env>(wrap<list<fib_name,wrap<integer<4>>>>{}));
static_assert(is_same_type<fibonacchi_case2,integer<3>>,"fib 4");

using fibonacchi_case3 = decltype(IReval<init_env>(wrap<list<fib_name,wrap<integer<6>>>>{}));
static_assert(is_same_type<fibonacchi_case3,integer<8>>,"fib 6");

using fibonacchi_case4 =
    decltype(IReval<init_env>(wrap<list<fib_name,wrap<integer<30>>>>{}));
static_assert(is_same_type<fibonacchi_case4,integer<832040>>,"fib 30");



// rec list replacement test

auto test_str = constexpr_string("(+ 11 (+ 11 2))");
using test_tokens = decltype(IRcar(tokenizer(test_str))); // raw token list

// some basic expression
using wraooertest = decltype(IReval<init_env>(
    wrap<list<
      addition, integer<11>, wrap<list<
        addition, integer<11>, integer<2>>>>>{}));

static_assert(is_same_type<wraooertest,integer<24>>,""); // check if eval works, get result

using replaced_expression = typename replace_nested_list<test_tokens>::type; // replace
using expected_type = wrap<list<addition, integer<11>, wrap<list<addition, integer<11>, integer<2>>>>>; // this is how it should look after

static_assert(is_same_type<replaced_expression, expected_type>, "recursive replacement"); // check if same

using after_replacement_evaluation = decltype(IReval<init_env>(replaced_expression{}));

static_assert(is_same_type<wraooertest,after_replacement_evaluation>,""); // check if they both get the same result

// DEFINE

auto one_parameter_define = constexpr_string("((define (inc a) (+ 1 a)) (inc 1))");
using one_parameter_define_tokens = decltype(IRcar(tokenizer(one_parameter_define))); // raw token list without the tokenized<...> wrapper
using one_parameter_define_result = decltype(IReval<environment<>>(one_parameter_define_tokens{}));
static_assert(is_same_type<one_parameter_define_result,integer<2>>);

auto two_parameter_define = constexpr_string("( (define (sum a b) (+ b a)) (sum 1 2))");
using two_parameter_define_tokens = decltype(IRcar(tokenizer(two_parameter_define))); // raw token list without the tokenized<...> wrapper
using two_parameter_define_result = decltype(IReval<environment<>>(two_parameter_define_tokens{}));
static_assert(is_same_type<two_parameter_define_result,integer<3>>);

auto multi_char_var_name = constexpr_string("(( define ss 11) (+ ss 1))"); // fix char and define bug
using multi_char_var_name_tokens = decltype(IRcar(tokenizer(multi_char_var_name))); // raw token list without the tokenized<...> wrapper
using multi_char_var_name_result = decltype(IReval<environment<>>(multi_char_var_name_tokens{}));
static_assert(is_same_type<multi_char_var_name_result,integer<12>>);