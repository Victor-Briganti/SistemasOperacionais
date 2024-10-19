.PHONY: main debug check-thread check-address check-valgrind clear

CXX = g++
CXFLAGS = -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Wconversion
LDFLAGS = -lpthread
DEBUG = -g3
ADDRESS = -fsanitize=address
THREAD = -fsanitize=thread

all: main

main:
	$(CXX) -o $@ $(CXFLAGS) $(LDFLAGS) common.cpp professor.cpp aluno.cpp main.cpp

debug:
	$(CXX) -o $@ $(CXFLAGS) $(DEBUG) $(LDFLAGS) common.cpp professor.cpp aluno.cpp main.cpp

check-address:
	$(CXX) -o debug $(CXFLAGS) $(LDFLAGS) $(ADDRESS) common.cpp professor.cpp aluno.cpp main.cpp

check-thread:
	$(CXX) -o debug $(CXFLAGS) $(LDFLAGS) $(THREAD) common.cpp professor.cpp aluno.cpp main.cpp

check-valgrind: debug
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./debug 6

check-helgrind: debug
	valgrind --tool=helgrind --verbose ./debug 6

clean:
	rm -rf main debug
