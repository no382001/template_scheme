#pragma once

#include "lists.h"
#include "car_cdr.h"
#include "atoms.h"
#include "primitive_operations.h"
#include "env.h"

// -- contains

// non template type
template <typename T, typename... Types>
struct contains_type : std::disjunction<std::is_same<T, Types>...> {};

// nested
template <typename T, template <typename...> class TT, typename... Types>
struct contains_type<T, TT<Types...>> : std::disjunction<contains_type<T, Types>...> {};

// base case recursion
template <typename T>
struct contains_type<T> : std::false_type {};

// helper
template <typename T, typename List>
using contains_type_t = contains_type<T, List>;

using my_list = token_list<list<int, token_list<list<float, double>, char>>>;
static_assert(contains_type_t<int, my_list>::value, "list should contain int");
static_assert(!contains_type_t<long, my_list>::value, "list should not contain long");
static_assert(contains_type_t<float, my_list>::value, "list should contain float");
static_assert(contains_type_t<double, my_list>::value, "list should contain double");

// -- !contains


// -- moved from pretty_print.h

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

// ----------

// moved from utils.h


// preproc eval defer
#define EMPTY()
#define DEFER_INNER(m) m EMPTY EMPTY EMPTY()
#define DEFER(m) DEFER_INNER(m)

// a compile time warning, problem wont come up until the branch is preprocessed
#define COMPILE_TIME_WARNING "Compile time warning"
// struct COMPILE_TIME_WARNING {};


struct MyStruct {
    int value;

    constexpr MyStruct(int v) : value(v) {}

    constexpr MyStruct operator+(const MyStruct& other) const {
        return MyStruct{ value + other.value };
    }
};

template<typename... Args>
constexpr auto sump(Args... args) {
    return (args + ...);
}

constexpr MyStruct a{1}, b{2}, c{3};

constexpr MyStruct result = sump(a, b, c);

template<int... Nums>
constexpr int ands() {
    return (... && Nums);
}

constexpr int ressult = ands<1, 0, 3, 4, 5>();

auto constexpr resullll = 1 && 0;

// take either #t or #f in the pack
// or a pack of integers, either way
// convert them to 1s and 0s, and return the result according to what they were

template <typename... Args>
constexpr bool and_proc(list<Args...>) {
    return (Args::value() && ...);
}

constexpr bool result2 = and_proc(list<integer<1>, integer<1>, integer<1>>{});
constexpr bool result22 = and_proc(list<integer<1>>{});
constexpr bool result222 = and_proc(list<integer<1>, integer<0>, integer<3>>{});
    
constexpr bool result1 = and_proc(list<scm_true,scm_true,scm_true>{});
constexpr bool result11 = and_proc(list<scm_true>{});
constexpr bool result111 = and_proc(list<scm_true,scm_false,scm_false>{});

static_assert(result1 == result2,"");
static_assert(result11 == result22,"");
static_assert(result111 == result222,"");