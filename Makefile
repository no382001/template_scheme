make :
	g++ -std=c++20 -w -ftemplate-backtrace-limit=0 -ftemplate-depth=100 ground.cpp;
debug:
	make > err.log 2>&1; awk '{gsub(/(; Exp |; A |; Args |; Evaluated_opnds |; arglist |; Op | Proc |; Predicate )/, "\n\t&"); print}' err.log > temp.txt && mv temp.txt err.log;
clang:
	clang-13 -std=c++2b ground.cpp
