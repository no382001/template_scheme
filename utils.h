#pragma once

#define constexpr_string(...) ([]() constexpr -> std::string_view { return __VA_ARGS__; })

constexpr int _pow(int base, int exp, int result = 1) {
	return exp < 1 ? result : _pow(base * base, exp / 2, (exp % 2) ? result * base : result);
} // https://stackoverflow.com/questions/17719674/c11-fast-constexpr-integer-powers


// This returns the log base b of n rounded down to an integer, and has more consistency with inexact results than the other answer.
constexpr int _log(int b, int n) {
	return n < b ? 0 : _log(b, n / b) + 1;
} //https://stackoverflow.com/questions/37582853/recursive-method-to-calculate-log

template <typename A, typename B>
constexpr inline bool is_same_type = std::is_same<A, B>::value;