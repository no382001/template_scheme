CFLAGS = -Wall -Wextra
VERSION = -std=c++17
EXTENDED_LIMITS = -ftemplate-backtrace-limit=100 -ftemplate-depth=10000 -fconstexpr-depth=800
SUPPRESS_WARNING = -w
GPP = g++

make :
	@/bin/bash -c 'time ${GPP} ${VERSION} ${SUPPRESS_WARNING} ${CFLAGS} ${EXTENDED_LIMITS} main.cpp -o build/a.out;'
fatal:
	${GPP} ${VERSION} ${SUPPRESS_WARNING} ${CFLAGS} -Wfatal-errors main.cpp -o build/a.out;
debug:
	@/bin/bash -c 'echo "--> logging"; time ${GPP} ${VERSION} ${SUPPRESS_WARNING} ${CFLAGS} main.cpp -o build/a.out > build/err.log 2>&1; echo "--> error logging finished"; echo "--> processing"; time python3 utils/process.py; echo "--> processing finished";'
run: 
	clear; make && ./build/a.out
