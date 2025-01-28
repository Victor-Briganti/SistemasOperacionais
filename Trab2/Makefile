CXX:=g++
CXXFLAGS:=-Wextra -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Wconversion
DEBFLAGS:=-g3 -fstandalone-debug
INCLUDE:=-Iinclude/

all:
	$(CXX) $(INCLUDE) ./src/filesystem/cluster/cluster.cpp ./src/filesystem/entry/dentry.cpp ./src/filesystem/entry/long_entry.cpp ./src/filesystem/entry/short_entry.cpp ./src/filesystem/fatfs.cpp ./src/filesystem/structure/bpb.cpp ./src/filesystem/structure/fat_table.cpp ./src/filesystem/structure/fsinfo.cpp ./src/io/fs_adapter.cpp ./src/io/image.cpp ./src/main.cpp ./src/path/pathname.cpp ./src/shell/shell.cpp ./src/utils/logger.cpp ./src/utils/time.cpp -o main

debug:
	$(CXX) $(INCLUDE) $(CXXFLAGS) $(DEBFLAGS) ./src/filesystem/cluster/cluster.cpp ./src/filesystem/entry/dentry.cpp ./src/filesystem/entry/long_entry.cpp ./src/filesystem/entry/short_entry.cpp ./src/filesystem/fatfs.cpp ./src/filesystem/structure/bpb.cpp ./src/filesystem/structure/fat_table.cpp ./src/filesystem/structure/fsinfo.cpp ./src/io/fs_adapter.cpp ./src/io/image.cpp ./src/main.cpp ./src/path/pathname.cpp ./src/shell/shell.cpp ./src/utils/logger.cpp ./src/utils/time.cpp -o main

valgrind: debug
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --trace-children=yes --verbose $(DEBUG_DIR)/main fat32.img img/tres_tigres_tristes_dividindo_tristando_um_tristador

clean:
	rm -rf build/

.PHONY: all valgrind
