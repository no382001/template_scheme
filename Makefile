make :
	(g++ -std=c++20 -w ground.cpp || exit 1); ./a.out;
