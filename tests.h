#include "h/lists.h"
#include "h/car_cdr.h"
#include "h/utils.h"

struct testobj {};
struct fooobj {};

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