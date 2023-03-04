#include "tests.h"
#include "h/lists.h"
#include "h/car_cdr.h"
#include "h/utils.h"
#include "h/atoms.h"
#include "h/primitive_operations.h"
#include "tests.h"
#include "h/eval_apply.h"

// (apply addtion '(1 2 3))
using sdfgs = decltype(IRapply(addition{},quote<list<integer<1>,integer<2>,integer<3>>>{}));

using asdasd = decltype(apply_primitve_procedure(int{},list<integer<1>,integer<1>>{}));

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <regex>
#include <map>
#include <cmath>


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

/**/

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


/**/

int main(){


//using comproc = decltype(IReval<init_env>(quote<list<apply,inc_c_list,quote<c_<110>>>>{}));
//static_assert(is_same_type<comproc,integer<2>>,"apply com proc 1 arg 1 operand");


// if there is no apply it will think that its a variable and replace it with the cadr of the entry,
// quot gets removed when integer self elvaulates in normal workflow
// have another column in the env table for the type of the variable, and the procedure that is needed to emplace into the expression
using comproc2 = decltype(IReval<init_env>(quote<list<inc_c_list,quote<integer<1>>>>{}));


// table_entry
// car is name
// cadr is variable/procedure:
//      caddr is variable value if variable
//      cddr is body if procedure

// when eval sees a variable
// decide witch branch by the table_entry column

// write a printer that print the env in columns 


print_table(init_env{});
return 0;
}




//static_assert(is_same_type<comproc2,integer<2>>,"com proc 1 arg 1 operand");

