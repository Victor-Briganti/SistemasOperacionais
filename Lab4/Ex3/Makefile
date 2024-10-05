.PHONY: main debug valgrind clear

FLAGS = -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Wconversion

all: main

main:
	g++ -o main $(FLAGS) consumer.cpp producer.cpp main.cpp 

debug:
	g++ -o main -g $(FLAGS) consumer.cpp producer.cpp main.cpp 

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./main

clear:
	rm -rf main 
