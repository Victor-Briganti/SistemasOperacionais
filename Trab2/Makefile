INCLUDE=-Iinclude/
CXXFLAGS=-Wextra -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Wconversion

all: directory
	clang++ -o build/main $(INCLUDE) $(CXXFLAGS) src/io/image.cpp src/filesystem/bpb.cpp src/filesystem/fat_table.cpp src/utils/color.cpp src/main.cpp

debug:
	clang++ -o build/main -g $(INCLUDE) $(CXXFLAGS) src/io/image.cpp src/filesystem/bpb.cpp src/filesystem/fat_table.cpp src/utils/color.cpp src/main.cpp

image:
	dd if=/dev/zero of=fat.img bs=1M count=40
	mkfs.fat -F 32 fat.img

directory:
	mkdir -p build

.PHONY: all directory image clean
