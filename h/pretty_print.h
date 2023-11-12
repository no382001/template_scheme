#pragma once

#include "tokenizer.h"

#include <regex>
#include <iostream>

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


// --- REMOVING ALL OCCURANCES OF AN ELEMENT FROM NESTED VARIATIONS OF list<...> and token_list<...>

// --START---- redundant concatination

template <typename List1, typename List2>
struct concat;

template <typename... Types1, typename... Types2>
struct concat<list<Types1...>, list<Types2...>> {
    using type = list<Types1..., Types2...>;
};

template <typename... Types1, typename... Types2>
struct concat<token_list<Types1...>, token_list<Types2...>> {
    using type = token_list<Types1..., Types2...>;
};

template <typename List1, typename List2>
using concat_t = typename concat<List1, List2>::type;

// --END--- redundant concatination

// ------REMOVE_TYPE--------------

template <typename T, typename List>
struct remove_type;

template <typename T, typename First, typename... Rest>
struct remove_type<T, list<First, Rest...>> {
    using type = typename std::conditional<
        std::is_same<T, First>::value,
        typename remove_type<T, list<Rest...>>::type,
        concat_t<
			list<typename remove_type<T, First>::type>,
			typename remove_type<T, list<Rest...>>::type>
    >::type;
};

template <typename T, typename First, typename... Rest>
struct remove_type<T, token_list<First, Rest...>> {
    using type = typename std::conditional<
        std::is_same<T, First>::value,
        typename remove_type<T, token_list<Rest...>>::type,
        concat_t<
			token_list<typename remove_type<T, First>::type>,
			typename remove_type<T, token_list<Rest...>>::type>
    >::type;
};

template <typename T, typename... Types, typename... Rest> // token list nested spec
struct remove_type<T, token_list<token_list<Types...>, Rest...>> {
    using type = concat_t<token_list<typename remove_type<T, token_list<Types...>>::type>, typename remove_type<T, token_list<Rest...>>::type>;
};

template <typename T, typename... Types, typename... Rest> // nested list spec
struct remove_type<T, list<list<Types...>, Rest...>> {
    using type = concat_t<list<typename remove_type<T, list<Types...>>::type>, typename remove_type<T, list<Rest...>>::type>;
};

template <typename T,typename A>
struct remove_type {
    using type = A;
};

template <typename T>
struct remove_type<T, T> {
    using type = void;
};

template <typename T>
struct remove_type<T, token_list<>> {
    using type = token_list<>;
};

template <typename T>
struct remove_type<T, list<>> {
    using type = list<>;
};

// -------------------------

template <typename T, typename List>
using remove_type_t = typename remove_type<T, List>::type;

using original_list = list<void, void, int, void>;
using cleaned_list = remove_type_t<void, original_list>;
static_assert(is_same_type<cleaned_list,list<int>>,"");
static_assert(is_same_type<remove_type_t<void,list<>>,list<>>,"");


using oh_list = list<void, list<int, void>, int, void>;
using clelist = remove_type_t<void, oh_list>;
static_assert(is_same_type<clelist, list<list<int>, int>>,"");


using o_list = list<void, list<int, void>, list<void, list<int, void>, int, void>,int, void,list<void, list<int, void>, int, void>>;
using cl_list = remove_type_t<void, o_list>;
static_assert(is_same_type<cl_list, list<list<int>,list<list<int>,int>,int,list<list<int>, int>>>,"");

using tlissss = token_list<whitespace<10>, list<token_list<scm_define, whitespace<32>, void, whitespace<32>, integer<1>>>, whitespace<10>, list<token_list<scm_define, whitespace<32>, void, whitespace<32>, integer<2>>>, whitespace<10>>;
using clisss = remove_type_t<whitespace<10>, tlissss>;
static_assert(is_same_type<clisss, token_list<list<token_list<scm_define, whitespace<32>, void, whitespace<32>, integer<1>>>, list<token_list<scm_define, whitespace<32>, void, whitespace<32>, integer<2>>>>>,"");


using faszom = remove_type_t<whitespace<32>, clisss>;
static_assert(is_same_type<faszom, token_list<list<token_list<scm_define, void, integer<1>>>, list<token_list<scm_define, void, integer<2>>>>>,"");

// i dont use this
namespace RemoveWhitespace {

	// map

	template <template <typename> class Func, typename List>
	struct map;

	template <template <typename> class Func>
	struct map<Func, list<>> {
		using type = list<>;
	};

	template <template <typename> class Func, typename First, typename... Rest>
	// removes all void on sight, make something void with the function to remove it
	struct map<Func, list<First, Rest...>> {
		using transformed_first_type = typename Func<First>::type;
		using first_type_list = typename std::conditional<
			std::is_same<transformed_first_type, void>::value,
			list<>,
			list<transformed_first_type>
    	>::type;

		using rest_list = typename map<Func, list<Rest...>>::type;
    	using type = typename concat<first_type_list, rest_list>::type;
	};

	template <template <typename> class Func, typename List>
	using map_t = typename map<Func, List>::type;

	// -- F(I) = I+1

	template <typename A>
	struct F;

	template <int I>
	struct F<integer<I>> {
		using type = integer<I + 1>;
	};

	using intlist = list<integer<1>,integer<2>,integer<3>>;
	using mapped = typename map<F,intlist>::type;
	static_assert(is_same_type<mapped,list<integer<2>, integer<3>, integer<4>>>,"");

	// remove all

	template <typename A> struct remove_int { using type = A; };
	 // if its int, return void so map omits the element
	template <> struct remove_int<int> { using type = void; };

	using l = list<int,char,int,char,char>;
	using res = map<remove_int,l>::type;

}

auto replace_chars(std::string str) {
	// delete all instances of struct and any whitespace in the string
	auto clean = std::regex_replace(str, std::regex(R"((struct )|( ))"), "");

    std::regex pattern(R"(c_<(\d+)>)"); // match all c_<...>
    std::string result = clean;
    std::smatch match;

    while (std::regex_search(result, match, pattern)) {
        char achar = static_cast<char>(std::stoi(match[1].str())); // conver mathed d to char
        result.replace(match.position(), match.length(), 1, achar);
    }

    result.erase(std::remove(result.begin(), result.end(), ','), result.end());
    
    std::regex c_list_re("c_list<([^>]+)>");
    std::string result2 = std::regex_replace(result, c_list_re, "$1");

    //std::regex brackets_re("list<token_list<([^>]+)>>");
    //std::string result3 = std::regex_replace(result2, brackets_re, "m $1 m");

    return result2;
}

template <typename T>
auto constexpr clean_whitespaces(T) {
    using removed_nl = decltype(remove_type_t<whitespace<'\n'>,T>{});
    using removed_spaces = decltype(remove_type_t<whitespace<' '>,removed_nl>{});

    return removed_spaces{};
}

std::string replaceWhitespace(std::string input) {
    std::string ws10 = "whitespace<10>";
    std::string ws32 = "whitespace<32>";
    std::string space = " ";
    std::string newline = "\n";

    size_t pos = input.find(ws32);
    while (pos != std::string::npos) {
        input.replace(pos, ws32.length(), space);
        pos = input.find(ws32, pos + space.length());
    }

    pos = input.find(ws10);
    while (pos != std::string::npos) {
        input.replace(pos, ws10.length(), newline);
        pos = input.find(ws10, pos + newline.length());
    }

    return input;
}
