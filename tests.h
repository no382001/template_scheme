#pragma once
#include "h/string.h"
#include "h/lists.h"
#include "h/car_cdr.h"
#include "h/env.h"
#include "h/eval_apply.h"
#include "h/tokenizer.h"

// moved from the end of env.h

// (define (inc a) (+ a 1))
// (inc 1) ; 2

// c_list<c_<105>, c_<110>, c_<99>>
using inc_c_list = decltype(c_list<c_<'i'>,c_<'n'>,c_<'c'>>{});
using sum_of_2 = decltype(c_list<c_<'s'>,c_<'u'>,c_<'m'>>{});
using sub_c_list = decltype(c_list<c_<'s'>,c_<'u'>,c_<'b'>>{});

using fib_name = decltype(c_list<c_<'f'>,c_<'i'>,c_<'b'>>{});
using fib_proc_body = decltype(
    wrap<list<scm_if,
        wrap<list<lesseq,c_<'x'>,integer<2>>>,
        integer<1>,
            list<addition,
                list<fib_name,wrap<list<subtraction,c_<'x'>,integer<1>>>>,
                list<fib_name,wrap<list<subtraction,c_<'x'>,integer<2>>>>>>>{});

using init_env = decltype(
    make_environment(table_entry<c_<110>,variable,integer<1>>{},  //));
                    table_entry<inc_c_list,procedure,c_<'a'>,wrap<list<addition,c_<'a'>,integer<1>>>>{},
                    table_entry<sub_c_list,procedure,c_<'a'>,wrap<list<subtraction,c_<'a'>,integer<1>>>>{},
                    table_entry<sum_of_2,procedure,list<c_<'a'>,c_<'b'>>,wrap<list<addition,c_<'a'>,c_<'b'>>>>{},
                    table_entry<fib_name,procedure,c_<'x'>,fib_proc_body>{}));

// --------

namespace Tests {

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

/** /
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
/**/

// LIST behaviour tests
namespace lists {

    CRTP_LIST(test_list);

    using listtest1 = decltype(make_test_list());
    using listres1 = decltype(test_list<>{});
    static_assert(is_same_type<listtest1,listres1>,"empty list constructor");

    using listtest2 = decltype(make_test_list(make_test_list()));
    using listres2 = decltype(test_list<>{});
    static_assert(is_same_type<listtest2,listres2>,"empty list in list constructor, collapses into empty list");

    using listtest3 = decltype(make_test_list(make_test_list(testobj{})));
    using listres3 = decltype(test_list<testobj>{});
    static_assert(is_same_type<listtest3,listres3>,"obj in list constructor, collapses into obj in list");

    using listtest4 = decltype(make_test_list(make_test_list(testobj{}),make_test_list()));
    using listres4 = decltype(test_list<testobj>{});
    static_assert(is_same_type<listtest4,listres4>,"obj in list constructor and empty list, collapses into obj in list");

    using listtest41 = decltype(make_test_list(make_test_list(),make_test_list(testobj{}),make_test_list()));
    using listres41 = decltype(test_list<testobj>{});
    static_assert(is_same_type<listtest41,listres41>,"obj in list constructor and empty list on either side, collapses into obj in list");

    using listtest5 = decltype(make_test_list(test_list<testobj>{},test_list<>{},test_list<testobj>{},test_list<testobj>{}));
    using listres5 = decltype(test_list<testobj,testobj,testobj>{});
    static_assert(is_same_type<listtest5,listres5>,"3 list<obj> in list constructor and empty list inbetween them, collapses into 3 list<obj> in list");

    // (apply addtion '(1 2 3))
    using appltest1 = decltype(IRapply<environment<>>(addition{},wrap<list<integer<1>,integer<2>,integer<3>>>{}));

    using map_pair_test = decltype(map_pair(list<integer<1>>{},list<integer<1>>{}));
    static_assert(is_same_type<map_pair_test,environment<table_entry<integer<1>,variable,integer<1>>>>,"map_pair_test");
    //using map_pair_test2 = decltype(map_pair(list<integer<1>>{},list<integer<1>,integer<1>>{}));
};

namespace list_searches {
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
};

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

namespace compound_procedures {
    using comproc = decltype(IReval<init_env>(wrap<list<apply,inc_c_list,wrap<c_<110>>>>{}));
    static_assert(is_same_type<comproc,integer<2>>,"apply com proc, 1 arg and 1 operand");

    // (probably)
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
};

// ---------------PRIM PROC RELATED----------------

namespace prim_procedures {
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

    using testaddev = decltype(apply_addition(list<integer<2>,integer<3>>{}));
    using testaddevres = decltype(integer<5>{});
    static_assert(is_same_type<testaddev,testaddevres>,"additon of two integers");

    // not relevant anymore 
    namespace what_is_happening_with_this { 
        using testaddev = decltype(apply_subtraction(list<integer<2>,integer<3>>{}));
        using testaddevres = decltype(integer<-1>{});
        static_assert(is_same_type<testaddev,testaddevres>,"subtraction of two integers");
    };
};

namespace replace_nested_list {
    // token_list -> wrap
    auto test_str = CSTRING("(+ 11 (+ 11 2))");
    using test_tokens = decltype(IRcar(tokenizer(test_str))); // raw token list
    
    // some basic expression
    using wraooertest = decltype(IReval<init_env>(
        wrap<list<
        addition, integer<11>, wrap<list<
            addition, integer<11>, integer<2>>>>>{}));

    // check if eval works, get result
    static_assert(is_same_type<wraooertest,integer<24>>,""); 
    // replace
    using replaced_expression = typename ::replace_nested_list<test_tokens>::type;
    // this is how it should look after
    using expected_type = wrap<list<addition, integer<11>, wrap<list<addition, integer<11>, integer<2>>>>>;
    // check if same
    static_assert(is_same_type<replaced_expression, expected_type>, "recursive replacement");

    using after_replacement_evaluation = decltype(IReval<init_env>(replaced_expression{}));
    // check if they both get the same result
    static_assert(is_same_type<wraooertest,after_replacement_evaluation>,"");
};



// ---------------DEFINE RELATED----------------

namespace define {
    auto one_parameter_define = CSTRING("((define (inc a) (+ 1 a)) (inc 1))");
    using one_parameter_define_tokens = decltype(IRcar(tokenizer(one_parameter_define))); // raw token list without the tokenized<...> wrapper
    using one_parameter_define_result = decltype(IReval<environment<>>(one_parameter_define_tokens{}));
    static_assert(is_same_type<one_parameter_define_result,integer<2>>);

    auto two_parameter_define = CSTRING("( (define (sum a b) (+ b a)) (sum 1 2))");
    using two_parameter_define_tokens = decltype(IRcar(tokenizer(two_parameter_define))); // raw token list without the tokenized<...> wrapper
    using two_parameter_define_result = decltype(IReval<environment<>>(two_parameter_define_tokens{}));
    static_assert(is_same_type<two_parameter_define_result,integer<3>>);

    auto multi_char_var_name = CSTRING("(( define ss 11) (+ ss 1))"); // fix char and define bug
    using multi_char_var_name_tokens = decltype(IRcar(tokenizer(multi_char_var_name))); // raw token list without the tokenized<...> wrapper
    using multi_char_var_name_result = decltype(IReval<environment<>>(multi_char_var_name_tokens{}));
    static_assert(is_same_type<multi_char_var_name_result,integer<12>>);
};

namespace double_define {
    auto str = CSTRING("(( define ss 11) ( define aa 11) (+ ss aa))");
    using tokenization_result = decltype(IRcar(tokenizer(str)));
    using eval_result = decltype(IReval<environment<>>(tokenization_result{}));
    static_assert(is_same_type<eval_result,integer<22>>);
};


namespace FullyEvaldFib {
    auto main_str = CSTRING("((define (fib x) (if (< x 3) 1 (+ (fib (- x 1)) (fib (- x 2))))) (fib 3))");
    using tokenization_result = decltype(IRcar(tokenizer(main_str))); // raw token list without the tokenized<...> wrapper
    using eval_result = decltype(IReval<environment<>>(tokenization_result{}));
    static_assert(is_same_type<eval_result,integer<2>>);
};

namespace Quoteiung {
    auto main_str = CSTRING("('(+ 1 1))");
    using tokenization_result = decltype(IRcar(tokenizer(main_str))); // raw token list without the tokenized<...> wrapper
    using eval_result = decltype(IReval<environment<>>(tokenization_result{}));
    static_assert(is_same_type<eval_result,wrap<list<addition, integer<1>, integer<1>>>>);
};

namespace ApplyProper {
    auto main_str = CSTRING("(apply + '(1 1))");
    using tokenization_result = decltype(IRcar(tokenizer(main_str))); // raw token list without the tokenized<...> wrapper
    using eval_result = decltype(IReval<environment<>>(tokenization_result{}));
    static_assert(is_same_type<eval_result,integer<2>>);
};

namespace EvalProper {
    auto main_str = CSTRING("(eval '(+ 1 1))");
    using tokenization_result = decltype(IRcar(tokenizer(main_str))); // raw token list without the tokenized<...> wrapper
    using eval_result = decltype(IReval<environment<>>(tokenization_result{}));
    static_assert(is_same_type<eval_result,integer<2>>);
};

};

namespace car_cdr_list_cons {
    auto main_str = CSTRING(R"(
    (cons (car 
        (list 1 
            (cdr (list
                1 (+ 1 2))))) 1)
    )");

    using tokenization_result_w_whitespaces = decltype(tokenize(main_str)); // raw token list without the tokenized<...> wrapper
    // remove whitespaces
    using tokens = decltype(clean_whitespaces(tokenization_result_w_whitespaces{}));
    // replace token_list<list<...>> with wrap<list<...>>
    using clean_expression = typename replace_nested_list<tokens>::type; // convert list
    // replace outer wrap<...> with tokenized<...>
    using tb_evaluated = decltype(replace_wrapper(clean_expression{},tokenized{}));
    // evaluate expression
    using eval_result = decltype(IReval<init_env>(IRcar(tb_evaluated{})));

    static_assert(is_same_type<eval_result,cons<integer<1>,integer<1>>>,"");
};

namespace define_test_single {

    auto main_str = CSTRING(R"(
    (
    (define (remainder a b)
        (- a (* (/ a b) b)))
    (remainder 3 2)
    )
    )");

    using tokenization_result_w_whitespaces = decltype(tokenize(main_str)); // raw token list without the tokenized<...> wrapper
    // remove whitespaces
    using tokens = decltype(clean_whitespaces(tokenization_result_w_whitespaces{}));
    // replace token_list<list<...>> with wrap<list<...>>
    using clean_expression = typename replace_nested_list<tokens>::type; // convert list
    // replace outer wrap<...> with tokenized<...>
    using tb_evaluated = decltype(replace_wrapper(clean_expression{},tokenized{}));
    // evaluate expression
    using eval_result = decltype(IReval<init_env>(IRcar(tb_evaluated{})));

    static_assert(is_same_type<eval_result,integer<1>>,"");
};

// its fucked from here
namespace define_test_double_define_in_same_scope {
    // if i tab this like this:
    /*
    auto main_str = CSTRING(R"((
        (define y 2)
        (define x 1)
        (define (remainder a b)
            (- a (* (/ a b) b)))
        (+ x (remainder 3 2))
    ))");
    */
    // instead of this, it fails for whatever reason
auto main_str = CSTRING(R"((
    (define y 2)
    (define x 1)
    (define (remainder a b)
        (- a (* (/ a b) b)))
    (+ x (remainder 3 2))
))");

    using tokenization_result_w_whitespaces = decltype(tokenize(main_str)); // raw token list without the tokenized<...> wrapper
    // remove whitespaces
    using tokens = decltype(clean_whitespaces(tokenization_result_w_whitespaces{}));
    // replace token_list<list<...>> with wrap<list<...>>
    using clean_expression = typename replace_nested_list<tokens>::type; // convert list
    // replace outer wrap<...> with tokenized<...>
    using tb_evaluated = decltype(replace_wrapper(clean_expression{},tokenized{}));
    // evaluate expression
    using eval_result = decltype(IReval<init_env>(IRcar(tb_evaluated{})));
    static_assert(is_same_type<eval_result,integer<2>>,"");
};



// so the outer scope of (...) works for any amount of defines
// but for some reason if i define inside a scope of another define, the tag is considered a compound proc and fails


namespace broken_syntax_for_inner_defines_but_works {
    // define_var_name_helper_integer gets a list of things when it only expects one thing or a list which it takes the second element of
    // so its not an easy fix

    // this behaviour kind of similar to how i need to put an outer (...) to execute defines and expressions in one scope

auto main_str = CSTRING(R"(
(
    (define (add-two x)
        (
            (define y 2)
            (+ x y)))
    (add-two 2)
)
)");

    using tokenization_result_w_whitespaces = decltype(tokenize(main_str)); // raw token list without the tokenized<...> wrapper
    // remove whitespaces
    using tokens = decltype(clean_whitespaces(tokenization_result_w_whitespaces{}));
    // replace token_list<list<...>> with wrap<list<...>>
    using clean_expression = typename replace_nested_list<tokens>::type; // convert list
    // replace outer wrap<...> with tokenized<...>
    using tb_evaluated = decltype(replace_wrapper(clean_expression{},tokenized{}));
    // evaluate expression
    using eval_result = decltype(IReval<environment<>>(IRcar(tb_evaluated{})));
    static_assert(is_same_type<eval_result,integer<4>>,"");
};

namespace sum_up_to_n {

auto main_str = CSTRING(R"(
(
(define (sum-up-to n)
(if (= n 0)
    0
    (+ n (sum-up-to (- n 1)))))
(sum-up-to 2)
)
)");

    using tokenization_result_w_whitespaces = decltype(tokenize(main_str)); // raw token list without the tokenized<...> wrapper
    // remove whitespaces
    using tokens = decltype(clean_whitespaces(tokenization_result_w_whitespaces{}));
    // replace token_list<list<...>> with wrap<list<...>>
    using clean_expression = typename replace_nested_list<tokens>::type; // convert list
    // replace outer wrap<...> with tokenized<...>
    using tb_evaluated = decltype(replace_wrapper(clean_expression{},tokenized{}));
    // evaluate expression
    using eval_result = decltype(IReval<environment<>>(IRcar(tb_evaluated{})));
    static_assert(is_same_type<eval_result,integer<3>>,"");
};

namespace comments {

auto main_str = CSTRING(R"(
(
;this is a comment within the midst of tokens
(+ 1 1)
; this is also a comment, until its a newline
)
)");

    using tokenization_result_w_whitespaces = decltype(tokenize(main_str));
    using tokens = decltype(clean_whitespaces(tokenization_result_w_whitespaces{}));
    using clean_expression = typename replace_nested_list<tokens>::type; // convert list
    using tb_evaluated = decltype(replace_wrapper(clean_expression{},tokenized{}));
    using eval_result = decltype(IReval<environment<>>(IRcar(tb_evaluated{})));
    static_assert(is_same_type<eval_result,integer<2>>,"");
};