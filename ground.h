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

template <typename T>
void print_table(T t) {
    auto str = demangle<T>();

	// delete all instances of struct and any whitespace in the string
	auto clean = std::regex_replace(str, std::regex(R"((struct )|( ))"), "");
    std::cout << "-- contents of environment: " << '\n'; 
    
    auto lptr = 0;
    auto rptr = 0;
    auto indent = 0;

    while ((lptr = clean.find("table_entry<",rptr)) != std::string::npos){
        lptr += 11;
        rptr = lptr;
        indent = 1;

        while (indent != 0) {
            rptr = clean.find_first_of("<>", rptr+1);

            switch (clean[rptr]) {
                case '<': indent++; break;
                case '>': indent--; break;
            }
        }
        std::cout << clean.substr(lptr-11,rptr - lptr + 12) << '\n';
    }
    std::cout << "------------------------" << '\n';
}