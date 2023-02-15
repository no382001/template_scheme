#pragma once
#include <type_traits>

template <typename A, typename B>
constexpr inline bool is_same_type = std::is_same<A, B>::value;