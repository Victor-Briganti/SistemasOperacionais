INCLUDE=-Iinclude/
CXXFLAGS=-Wextra -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Wconversion

all: common

common: directory
	clang++ -o build/main $(INCLUDE) $(CXXFLAGS) src/path/parser.cpp src/io/image.cpp src/filesystem/bpb.cpp src/filesystem/fat_table.cpp src/filesystem/fsinfo.cpp src/filesystem/dentry.cpp src/filesystem/fatfs.cpp src/utils/color.cpp src/main.cpp

debug: directory
	clang++ -o build/main -g $(INCLUDE) $(CXXFLAGS) src/path/parser.cpp src/io/image.cpp src/filesystem/bpb.cpp src/filesystem/fat_table.cpp src/filesystem/fsinfo.cpp src/filesystem/dentry.cpp src/filesystem/fatfs.cpp src/utils/color.cpp src/main.cpp

image:
	dd if=/dev/zero of=fat.img bs=1M count=40
	mkfs.fat -F 32 fat.img

valgrind: debug
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --trace-children=yes --verbose build/main fat32.img img/test/test2

directory:
	mkdir -p build

.PHONY: all common debug directory image valgrind clean
