#pragma once

template <typename A, typename... Args>
auto constexpr car_inner(A,Args...){
    return A{};
}

template <template<class> class T, typename... Args>
auto constexpr car(T<Args...>){
    if constexpr (sizeof...(Args) > 0) {
        return car_inner(Args{}...);
    } else {
        return T{};
    }
}

template <template<class> class T, typename A, typename... Args>
auto constexpr cdr_inner(A,Args...){
    return T<Args...>{};
}

template <template<class> class T, typename... Args>
auto constexpr cdr(T<Args...>){
    if constexpr (sizeof...(Args) > 0) {
        return cdr_inner<T>(Args{}...);
    } else {
        return T{};
    }
}