.PHONY: main debug valgrind clear

FLAGS = -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Wconversion
THREADS = -lpthread

all: main

main:
	g++ -o main $(FLAGS) $(THREADS) main.cpp 

debug:
	g++ -o main -g $(FLAGS) $(THREADS) main.cpp 

valgrind: debug
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./main 100 200 out 1

clear:
	rm -rf main
