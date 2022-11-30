#pragma once

#define constexpr_string(...) ([]() constexpr -> std::string_view { return __VA_ARGS__; })

constexpr int _pow(int base, int exp, int result = 1) {
	return exp < 1 ? result : _pow(base * base, exp / 2, (exp % 2) ? result * base : result);
}

constexpr int _log(int b, int n) {
	return n < b ? 0 : _log(b, n / b) + 1;
}

template <typename A, typename B>
constexpr inline bool is_same_type = std::is_same<A, B>::value;


template <typename Lambda, size_t Index, size_t end_of_list>
//pass a stringview return type lambda that passes the arguments with __VA_ARGS__
constexpr auto tokenize_list(Lambda str_lambda)
{
	using curr = decltype(tokenize< Lambda, Index + 1 >(str_lambda));

	return make_list(curr{});
}