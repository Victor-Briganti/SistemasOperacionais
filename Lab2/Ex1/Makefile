.PHONY: main clear

CXX = g++
LFLAG = -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Wconversion

all: main 

main:
	$(CXX) -o main $(LFLAG) main.cpp 

debug:
	$(CXX) -o main $(LFLAG) -g main.cpp 

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --trace-children=yes --verbose ./main 