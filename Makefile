make : main.cpp
	g++ -std=c++2a -w main.cpp

gentemp : main.cpp
	clang++ -std=c++20 -Xclang -ast-print -fsyntax-only main.cpp > gen/gen_temp.cpp
