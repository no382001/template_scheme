make :
	g++ -std=c++20 -w -ftemplate-backtrace-limit=100 -ftemplate-depth=10000 -fconstexpr-depth=800 -Wall -Wextra main.cpp -o build/a.out;
debug:
	@/bin/bash -c 'echo "--> logging"; time make > build/err.log 2>&1; echo "--> error logging finished"; echo "--> processing"; time python3 utils/process.py; echo "--> processing finished";'

clang:
	clang-13 -std=c++2b main.cpp
run: 
	clear; make && ./build/a.out
