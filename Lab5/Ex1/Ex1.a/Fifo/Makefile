.PHONY: receiver receiver-debug sender sender-debug valgrind clear

FLAGS = -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Wconversion

all: receiver sender

receiver:
	g++ -o receiver $(FLAGS) receiver.cpp 

sender:
	g++ -o sender $(FLAGS) sender.cpp 

receiver-debug:
	g++ -o receiver $(FLAGS) receiver.cpp 

sender-debug:
	g++ -o sender $(FLAGS) sender.cpp 

# valgrind:
# 	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./main

clean:
	rm -rf receiver sender ffifo