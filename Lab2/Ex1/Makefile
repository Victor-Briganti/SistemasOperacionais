.PHONY: main debug valgrind

CXX = g++
LFLAG = -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Wconversion

all: main 

main:
	$(CXX) -o main $(LFLAG) -lm main.cpp 

debug:
	$(CXX) -o main $(LFLAG) -lm -g main.cpp 

valgrind: debug
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --trace-children=yes --verbose ./main 3
