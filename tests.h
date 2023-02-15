#include "h/lists.h"
#include "h/utils.h"

struct testobj {};

//using quotetewwst = decltype(make_quote()); static assert

using quotetest = decltype(make_quote(testobj{},testobj{}));
using quotetestres = quote<testobj,testobj>;
static_assert(is_same_type<quotetest,quotetestres>,"");

