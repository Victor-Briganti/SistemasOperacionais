INCLUDE=-Iinclude/
CXXFLAGS=-Wextra -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Wconversion

all: directory image
	clang++ -o build/main $(INCLUDE) $(CXXFLAGS) src/shell/shell.cpp src/main.cpp 

debug:
	clang++ -o build/main -g $(INCLUDE) $(CXXFLAGS) src/shell/shell.cpp src/main.cpp 

# O correto aqui é que o 'count' seja 40 no mínimo, mas para teste usaremos 1 
image:
	dd if=/dev/zero of=fat.img bs=1M count=1
	mkfs.fat -F 32 fat.img

directory:
	mkdir -p build

.PHONY: all directory image clean
