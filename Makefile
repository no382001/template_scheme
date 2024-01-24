CFLAGS = -Wall -Wextra

make :
	@/bin/bash -c 'time g++ -std=c++17 -w ${CFLAGS} -ftemplate-backtrace-limit=100 -ftemplate-depth=10000 -fconstexpr-depth=800 main.cpp -o build/a.out;'
fatal:
	g++ -std=c++20 -w ${CFLAGS} -Wfatal-errors main.cpp -o build/a.out;
debug:
	@/bin/bash -c 'echo "--> logging"; time g++ -std=c++20 -w ${CFLAGS} main.cpp -o build/a.out > build/err.log 2>&1; echo "--> error logging finished"; echo "--> processing"; time python3 utils/process.py; echo "--> processing finished";'
clang:
	clang-13 -std=c++2b main.cpp
run: 
	clear; make && ./build/a.out
