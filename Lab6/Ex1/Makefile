.PHONY: main debug valgrind clear

FLAGS = -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Wconversion
THREADS = -lpthread

all: main

main:
	g++ -o main $(FLAGS) $(THREADS) main.cpp 

debug:
	g++ -o main -g $(FLAGS) $(THREADS) main.cpp 

debug-thread:
	g++ -o main -g -fsanitize=thread $(FLAGS) $(THREADS) main.cpp 

debug-address:
	g++ -o main -g -fsanitize=address $(FLAGS) $(THREADS) main.cpp 

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./main

clear:
	rm -rf main
