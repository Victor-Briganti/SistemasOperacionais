.PHONY: main client server client-debug server-debug valgrind clear

FLAGS = -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Wconversion

all: client server

client:
	g++ -o client $(FLAGS) client.cpp

server:
	g++ -o server $(FLAGS) server.cpp

client-debug:
	g++ -o client -g $(FLAGS) client.cpp

server-debug:
	g++ -o server -g $(FLAGS) server.cpp

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./main

clear:
	rm -rf server client socket 
