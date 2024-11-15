INCLUDE=-I./src/include
CXXFLAGS=-Wextra -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Wconversion

all: directory
	clang++ -o build/main $(INCLUDE) $(CXXFLAGS) src/shell/shell.cpp src/main.cpp 

debug:
	clang++ -o build/main -g $(INCLUDE) $(CXXFLAGS) src/shell/shell.cpp src/main.cpp 

directory:
	mkdir -p build

.PHONY: all directory clean
