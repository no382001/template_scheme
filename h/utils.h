#pragma once
#include <type_traits>
#include <regex>
#include <iostream>

#include "atoms.h"


template <template <class> typename A, template <class> typename B, typename... First, typename... Second>
bool constexpr is_same_list_t(A<First...> a,B<Second...> b){ 
	using derivedA = decltype(A{});
	using derivedB = decltype(B{});
	
	if constexpr (is_same_type<derivedA,derivedB>){
		return true;
	} else {
		return false;
	}
}

// for static asserts that need delayed constexpr evaluation
//  - static assert willl always fail with a constexpr konw value during template generation
//  - even when the branch is not used and will be omitted
//  - delay the evaluation of the false value to throw a static assert only when the branch is evaluated
#define DELAYED_FALSE !is_same_type<int,int>

// preproc eval defer
#define EMPTY()
#define DEFER_INNER(m) m EMPTY EMPTY EMPTY()
#define DEFER(m) DEFER_INNER(m)

// a compile time warning, problem wont come up until the branch is preprocessed
#define COMPILE_TIME_WARNING "Compile time warning"
// struct COMPILE_TIME_WARNING {};




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

    std::regex pattern(R"(c_<(\d+)>)"); // match all c_<...>
    std::string result = clean;
    std::smatch match;

    while (std::regex_search(result, match, pattern)) {
        char achar = static_cast<char>(std::stoi(match[1].str())); // conver mathed d to char
        result.replace(match.position(), match.length(), 1, achar);
    }

    std::cout << result << std::endl;
}

#include <string_view>
#define constexpr_string(str) ([]() constexpr -> std::string_view { return str; })