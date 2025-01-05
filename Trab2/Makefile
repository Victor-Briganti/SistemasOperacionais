CXX:=clang++
CXXFLAGS:=-Wextra -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Wconversion
DEBFLAGS:=-g3 -fstandalone-debug
INCLUDE:=-Iinclude/

# Definição de diretórios
SRC_DIR:=src
BUILD_DIR:=build
RELEASE_DIR:=$(BUILD_DIR)/release
DEBUG_DIR:=$(BUILD_DIR)/debug

# Arquivos .cpp e .o
SRCS:=$(shell find src -type f)

# Diretórios padrões
DIRS:=$(sort $(dir $(SRCS)))

# Caminhos para compilação de debug
DEBUG_DIRS=$(patsubst $(SRC_DIR)/%, $(DEBUG_DIR)/%, $(DIRS))
DEBUG_OBJS=$(SRCS:$(SRC_DIR)/%.cpp=$(DEBUG_DIR)/%.o)

# Caminhos para compilação de release
RELEASE_DIRS=$(patsubst $(SRC_DIR)/%, $(RELEASE_DIR)/%, $(DIRS))
RELEASE_OBJS=$(SRCS:$(SRC_DIR)/%.cpp=$(RELEASE_DIR)/%.o)

$(DEBUG_DIRS):
	@mkdir -p $(DEBUG_DIRS)

$(DEBUG_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(INCLUDE) $(CXXFLAGS) $(DEBFLAGS)

$(DEBUG_DIR)/main: $(DEBUG_OBJS)
	$(CXX) -o $@ $(INCLUDE) $(CXXFLAGS) $(DEBFLAGS) $^

debug: $(DEBUG_DIRS) $(DEBUG_DIR)/main

$(RELEASE_DIRS):
	@mkdir -p $(RELEASE_DIRS)

$(RELEASE_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(INCLUDE) $(CXXFLAGS) $(DEBFLAGS)

$(RELEASE_DIR)/main: $(RELEASE_OBJS)
	$(CXX) -o $@ $(INCLUDE) $(CXXFLAGS) $(DEBFLAGS) $^

release: $(RELEASE_DIRS) $(RELEASE_DIR)/main

valgrind: debug
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --trace-children=yes --verbose $(DEBUG_DIR)/main fat32.img teste

image:
	dd if=/dev/zero of=fat.img bs=1M count=40
	mkfs.fat -F 32 fat.img

.PHONY: all release debug clean valgrind image
