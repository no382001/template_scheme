#pragma once
#include <iostream>
#include <regex>

template <typename T>
constexpr auto demangle() -> std::string {
#if defined(__clang__)
    constexpr auto prefix = std::string_view{"[T = "};
    constexpr auto suffix = "]";
    constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(__GNUC__)
    constexpr auto prefix = std::string_view{"with T = "};
    constexpr auto suffix = "; ";
    constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(_MSC_VER)
    constexpr auto prefix = std::string_view{"get_type_name<"};
    constexpr auto suffix = ">(void)";
    constexpr auto function = std::string_view{__FUNCSIG__};
#else
# error Unsupported compiler
#endif
    const auto start = function.find(prefix) + prefix.size();
    const auto end = function.find(suffix);
    const auto size = end - start;
    return std::string(function.substr(start, size)).data();
}

void pretty_print(std::string str) {
	// delete all instances of struct and any whitespace in the string
	auto clean = std::regex_replace(str, std::regex(R"((struct )|( ))"), "");
    std::cout << clean << '\n';
}

#include "h/tokenizer.h"


auto str = constexpr_string("(+ 11 (+ 11 2))");
using tokens = decltype(IRcar(tokenizer(str)));

using fibonacchi_case4 = 
    decltype(IReval<init_env>(quote<list<fib_name,quote<integer<30>>>>{}));


// list<token_list<
//   addition, integer<11>, list<token_list<
//     addition, integer<11>, integer<2>>>>>>

using wraooertest = decltype(IReval<init_env>(
    quote<list<
      addition, integer<11>, quote<list<
        addition, integer<11>, integer<2>>>>>{}));

static_assert(is_same_type<wraooertest,integer<24>>,"");



// recursively replace every list and token_list token, to make sure i dont break anything
template <
	template<class> class R, template<class,class> class innerR,
	typename A, typename... Args,typename... Brgs>
constexpr auto replace_wrapper(R<innerR<Brgs...>>,Args...){
    if constexpr (sizeof...(Args) > 0) {
	    using second = decltype(replace_wrapper(R<innerR<Brgs...>>{},IRcar(Args{}...)));
        return list<second,Args...>{};
    } else {
        return list<>{};
    }
}
/** /
template <
	template<class> class T, template<class,class> class innerT,
	template<class> class R, template<class> class innerR,
	typename A, typename... Args,typename... Brgs>
constexpr auto replace_wrapper(T<innerT<A,Args...>>,R<innerR<Brgs...>>){
    using second = replace_wrapper(T<innerT<Args...>>,R<innerR<Brgs...>>);
	return R<innerR<second>>{};
}
// need to check every member
// wrapper around for quote wrap
// go and apply replace_wrapper on any matching list
// 
/**/


using wrap_replace_res = decltype(replace_wrapper(tokens{},quote<list<>>{}));


// prio list:

// - the token list wrapper
// -- you can even have nested defines right? it is basically let, but i imagine its the same

// - probably need to handle multiple statements, like (define n 1) (+ 1 n)
// -- this is easy bs theres only one value return statement
// -- there needs to be a failsafe to ensure that only one function like that gets passed


// i dont even have define implemented
// take a list of expressions
// evaluate them 'line by line'
// pass the env to the next
template <typename Env, typename... Tokenized>
auto constexpr EvaluateExpressions(tokenized<Tokenized...>) {
    // return IReval<Env>(make_quote(IRcdr(Exp{})));
}
