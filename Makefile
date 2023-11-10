make :
	g++ -std=c++20 -w -ftemplate-backtrace-limit=0 -ftemplate-depth=1000000 main.cpp -o build/a.out;
debug:
	make > build/err.log 2>&1; awk '{gsub(/( Env |; Exp |; A |; Args |; Evaluated_opnds |; arglist |; Op | Proc |; Predicate |; Then |; Else )/, "\n\t&"); print}' build/err.log > build/err.log;
clang:
	clang-13 -std=c++2b main.cpp
run: 
	make && ./build/a.out