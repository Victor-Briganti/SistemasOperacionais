.PHONY: main debug valgrind clear

FLAGS = -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Wconversion
MATH = -lm

all: main

main:
	g++ -o main $(MATH) $(FLAGS) main.cpp 

debug:
	g++ -o main -g $(MATH) $(FLAGS) main.cpp 

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./main

clear:
	rm -rf main