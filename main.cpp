#include <string_view>
#include <type_traits>
#include <cmath>
#include "atoms.h"
#include "utils.h"
#include "lists.h"

template <typename Lambda, size_t Index = 0>
//pass a stringview return type lambda that passes the arguments with __VA_ARGS__
constexpr auto tokenize(Lambda str_lambda)
{
	constexpr auto str = str_lambda();

	if constexpr (Index < str.size()) {
		// deduce the type of the current char
		using curr = decltype(deduce_token_type< str[Index] >());

		if constexpr (is_integer_v<curr>) { // catch integers	
			constexpr auto first_non_integer = find_first_non_integer< Index + 1 >(str_lambda);
			using integer_type = decltype(make_integer< Index, first_non_integer >(str_lambda));
			using second = decltype(tokenize< Lambda, first_non_integer >(str_lambda));

			return make_token_list(integer_type{}, second{});
		}
		else {
			// if its not a specially handled token
			using next = decltype(tokenize< Lambda, Index + 1 >(str_lambda));
			return make_token_list(curr{}, next{});
		}
	}
	else {
		return make_token_list();
	}
}

int main()
{
	auto x = constexpr_string("(+ 1 12)");

	using y = decltype(tokenize(x));
}