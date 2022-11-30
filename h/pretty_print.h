#pragma once

#include <iostream>
#include <string>
#include <regex>


template <typename T>
constexpr auto demangle() -> std::string
{
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

    std::string indent_list = std::string("\n");
    size_t token = 0;
    while ((token = clean.find_first_of("<>,", token)) != std::string::npos) {
        switch (clean[token]) {
        case '<': indent_list.append(" ");
        case ',': clean.insert(token + 1, indent_list);
            break;
        case '>': indent_list.erase(indent_list.size() - 1);
            clean.insert(token, indent_list);
        }
        token += indent_list.size() + 1;
        const size_t nw = clean.find_first_not_of(" ", token);
        if (nw != std::string::npos) {
            clean.erase(token, nw - token);
        }
    }

    std::cout << clean << '\n';
}
