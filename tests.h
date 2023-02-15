#include "h/lists.h"
#include "h/car_cdr.h"
#include "h/utils.h"

struct testobj {};

//using quotetewwst = decltype(make_quote()); //empty quote
using quotetest = decltype(make_quote(testobj{},testobj{}));
using quotetestres = quote<testobj,testobj>;
static_assert(is_same_type<quotetest,quotetestres>,"");

// CAR
// (car '()) ; err
// (car '(testobj)) ; testobj
// --------------------------
//using cartest = decltype(car(make_list())); // car of empty list
using cartest = decltype(car(make_quote(make_list(testobj{}))));
using cartestres = testobj;
static_assert(is_same_type<cartest,cartestres>,"");

// CDR
// (cdr '()) ; err
// (car '(testobj)) ; ()
// (cdr '(test test2)) ; (test2)
// -----------------------------
//using cdrtest1 = decltype(cdr(make_list())); // cdr of empty list
using cdrtest2 = decltype(cdr(make_quote(make_list(testobj{})))); //cdr of last element
using cdrtestres2 = list<>;
static_assert(is_same_type<cdrtest,cdrtestres>,"");

using cdrtest = decltype(cdr(make_quote(make_list(testobj{},testobj{}))));
using cdrtestres = list<testobj>;
static_assert(is_same_type<cdrtest,cdrtestres>,"");

