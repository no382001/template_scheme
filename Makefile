make :
	g++ -std=c++20 -w -ftemplate-backtrace-limit=0 -ftemplate-depth=1000000 -fconstexpr-depth=10000000 main.cpp -o build/a.out;
debug:
	make > build/temp.log 2>&1; \
	awk '{gsub(/( Env |; Exp |; A |; Args |; Evaluated_opnds |; arglist |; Op | Proc |; Predicate |; Then |; Else )/, "\n\t&"); print}' build/temp.log > build/err.log; rm build/temp.log; \
	python3 utils/replace_char.py build/err.log build/err.log

clang:
	clang-13 -std=c++2b main.cpp
run: 
	clear; make && ./build/a.out
