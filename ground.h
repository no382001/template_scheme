#include "tests.h"
#include "h/lists.h"
#include "h/car_cdr.h"
#include "h/utils.h"
#include "h/atoms.h"

// -----------------------------------------------------------------------------------------------------
// proof of concept for using types as results

template <int A, typename... Rest>
auto constexpr addtion_sub(list<integer<A>, Rest...>) {
	if constexpr (sizeof...(Rest) > 0)
		return A + addtion_sub(list<Rest...>{});
	else{ return A; }
}

template <int A, typename... Rest>
auto constexpr additon(list<integer<A>,Rest...>) {
        static_assert((sizeof...(Rest) > 0), "addition with a single operand");
		return integer<A + addtion_sub(list<Rest...>{})>{}; // pass cdr to sublayer
}

using testaddev = decltype(additon(list<integer<2>,integer<3>>{}));
using testaddevres = decltype(integer<5>{});
static_assert(is_same_type<testaddev,testaddevres>,"additon of two integers");

//using testaddev3 = decltype(additon(list<integer<2>>{})); //addtion with single operand



template < typename Exp>
auto constexpr eval(Exp) {
    using car_of_exp = decltype(car(Exp{}));
	if constexpr (is_same_type<plus,car_of_exp>){
        // addtion is basically the apply of primitive procedure addition to arguments cdr(exp)
        return additon(cdr(Exp{}));
    } else {
        return;
    }
}

// evaluate primitive procedure addition, to return a result
using testaddition = decltype(eval(quote<list<plus,integer<2>,integer<3>>>{}));
using testadditionres = decltype(integer<5>{});
static_assert(is_same_type<testadditionres,testaddition>,"eval of additon of two integers");

// addition is a primitive-procedure
// - make prim proc, instantiate templates, and emplace in the env table

template < typename Proc, typename Args >
auto constexpr apply(Proc,Args) {
	return;
}

// -----------------------------------------------------------------------------------------------------
// setting up an init-env for eval

// environment table is a list of lists, or pairs
LIST(environment);

template <typename... columns>
struct table_entry {};

template <typename Env, typename Entry>
auto constexpr extend_environment(Entry){
    return Env::append(make_environment(Entry{}));
}

struct variable {};
struct procedure {};

template <typename name>
struct p_name {};

template <typename... args>
struct p_args {};

template <typename... body>
struct p_body {};


// search a list of lists, where the car of entry list is the identifier
// returns void if entry was not found
// log(n)
template <typename candidate, typename A, typename... Args>
auto constexpr table_search(candidate,environment<A,Args...>){
    using car_of_entry = decltype(IRcar(A{}));
    if constexpr (is_same_type<candidate,car_of_entry>){
        return A{};
	} else if constexpr (sizeof...(Args) > 0) { // if there are still entries left to search
        return table_search(candidate{},environment<Args...>{});
    } else {
        return;
    }
}

using env = decltype(make_environment()); // init list
// (define n 1)
using n_1 = decltype(table_entry<variable,c_<110>,integer<1>>{});
// (make-procedure (f n) (+ 1 n))
using proc_f_n_plus_1_n = decltype(table_entry<procedure,p_name<c_<'f'>>,p_args<c_<'n'>>,p_body<list<plus,integer<1>,c_<'n'>>>>{});

// add entries
using env1 = decltype(extend_environment<env>(n_1{}));
using env2 = decltype(extend_environment<env1>(proc_f_n_plus_1_n{}));

using search_expected = decltype(table_entry<variable, c_<110>, integer<1>>{});
using search_res = decltype(table_search(variable{},env2{}));
static_assert(is_same_type<search_expected,search_res>, "first entry");

using search_res2 = decltype(table_search(procedure{},env2{})); 
static_assert(is_same_type<proc_f_n_plus_1_n,search_res2>, "second entry");

using search_res3 = decltype(table_search(int{},env2{}));
static_assert(is_same_type<void,search_res3>, "non existent entry");