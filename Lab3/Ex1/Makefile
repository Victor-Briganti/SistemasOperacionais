.PHONY: main debug valgrind clear

FLAGS = -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Wconversion
THREADS = -lpthread
MATH = -lm

all: main

main:
	g++ -o main $(FLAGS) $(THREADS) $(MATH) main.cpp 

debug:
	g++ -o main -g $(FLAGS) $(THREADS) $(MATH) main.cpp 

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./main 10 10 1 

clear:
	rm -rf main