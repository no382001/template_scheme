#include "tests.h"
#include "h/lists.h"
#include "h/car_cdr.h"
#include "h/utils.h"
#include "h/atoms.h"
#include "h/primitive_operations.h"
#include "tests.h"

struct apply {};
struct eval {};

template < typename Proc, typename Args >
auto constexpr IRapply(Proc,quote<Args>) {
    // addition '(1 2 3)
    // if its not a prim it could be a compound
	using prim_proc = decltype(apply_primitve_procedure(Proc{},Args{}));

    return prim_proc{};

}

// (apply addtion '(1 2 3))
using appltest1 = decltype(IRapply(addition{},quote<list<integer<1>,integer<2>,integer<3>>>{}));


using init_env = decltype(make_environment(table_entry<variable,c_<110>,integer<1>>{}));

template < typename Exp, typename Env = init_env >
auto constexpr IReval(quote<Exp>) {
    // if it is an application
    using indicator = decltype(IRcar(Exp{}));
    if constexpr (is_same_type<apply,indicator>){
        using app_operator = decltype(IRcadr(Exp{}));
        using app_operands = decltype(IRcaddr(Exp{}));

        return IRapply(app_operator{},app_operands{});
    }
}

// (eval '(apply addition '(1 2 3)))
using evaltest1 = decltype(IReval(quote<list<apply,addition,quote<list<integer<1>,integer<2>,integer<3>>>>>{}));
using evaltest21 = decltype(integer<6>{});
static_assert(is_same_type<evaltest1,evaltest21>,"(eval '(apply addition '(1 2 3)))");

// (eval '(apply proc 'body 'param '(1 2 3) 'env))
using proctest1 = decltype(IReval(quote<list<apply,addition,quote<list<integer<1>,integer<2>,integer<3>>>>>{}));
//static_assert(is_same_type<proctest1,evaltest21>,"(eval '(apply addition '(1 2 3)))");

// setup env and put a procedure inside


/*
((application? exp)
         (apply (eval (operator exp) env)
                (list-of-values (operands exp) env)))


*/
// eval looks up the operator in the environment per (eval (operator exp) env)
// operator is basically 


// (eval (apply f 2))
// eval evaluates f in env, probably compund-proc


/*
(define (list-of-values exps env)
  (if (no-operands? exps)
    '() 
    (cons (eval (first-operand exps) env)
          (list-of-values (rest-operands exps) env))))


*/
// gets the list of values as per the env return empty list if there are none
// evaluates the car in env
// cons the list-of-values remainder in env


// so lets say that env contains x as 3
// if i (eval (apply addition (n 2)))
// then list of values should in the first pass go and eval n , which is a variable, look it up and return its value


//  implement list-of-values with variable? in eval
// (eval (apply addition (n 2)))
//using vartest = decltype(IReval(quote<list<apply,addition,quote<list<c_<110>,integer<2>>>>>{}));
