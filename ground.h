#include "tests.h"
#include "h/lists.h"
#include "h/car_cdr.h"
#include "h/utils.h"
#include "h/atoms.h"

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