.PHONY: client client-debug server server-debug valgrind clear

FLAGS = -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Wconversion
MATH = -lm

all: client server

client:
	g++ -o client $(MATH) $(FLAGS) client.cpp 

server:
	g++ -o server $(MATH) $(FLAGS) server.cpp 

client-debug:
	g++ -o client $(MATH) $(FLAGS) client.cpp 

server-debug:
	g++ -o server $(MATH) $(FLAGS) server.cpp 

# valgrind:
# 	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./main

clear:
	rm -rf client server ffifo