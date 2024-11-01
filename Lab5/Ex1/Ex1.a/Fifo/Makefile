.PHONY: client client-debug server server-debug valgrind clear

FLAGS = -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Wconversion

all: client server

client:
	g++ -o client $(FLAGS) client.cpp 

server:
	g++ -o server $(FLAGS) server.cpp 

client-debug:
	g++ -o client $(FLAGS) client.cpp 

server-debug:
	g++ -o server $(FLAGS) server.cpp 

# valgrind:
# 	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./main

clear:
	rm -rf client server ffifo