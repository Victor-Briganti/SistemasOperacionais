.PHONY: main debug valgrind clear

FLAGS = -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Wconversion
SANITIZER = -fsanitize=memory

all: main

main:
	g++ -o main $(FLAGS) main.cpp 

debug:
	g++ -o main -g $(FLAGS) main.cpp 

sanitizer:
	g++ -o main -g $(SANITIZER) $(FLAGS) main.cpp 

valgrind: debug
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./main 3000 511

clear:
	rm -rf main 
