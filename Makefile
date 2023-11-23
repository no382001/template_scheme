make :
	g++ -std=c++20 -w -ftemplate-backtrace-limit=100 -ftemplate-depth=10000 -fconstexpr-depth=800 -Wall -Wextra main.cpp -o build/a.out;
debug:
	make > build/temp.log 2>&1; \
	awk '{gsub(/(table_entry| Env |; Exp |; A |; Args |; Evaluated_opnds |; arglist |; Op | Proc |; Predicate |; Then |; Else )/, "\n\t&"); print}' build/temp.log > build/err.log; rm build/temp.log; \
	python3 utils/replace_char.py build/err.log build/err.log

clang:
	clang-13 -std=c++2b main.cpp
run: 
	clear; make && ./build/a.out
