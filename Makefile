make : main.cpp
	g++ -std=c++2a -w main.cpp

run : make a.out
	clear; ./a.out

gentemp : main.cpp
	clang++ -std=c++20 -Xclang -ast-print -fsyntax-only main.cpp > gen/gen_temp.cpp