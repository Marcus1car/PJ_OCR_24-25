CC = gcc
CC_FLAGS = -Wall -Wextra -std=c17 -fsanitize=address -g
LIBS = -lm
BUILD_DIR = ./build

all: solver

build_dir:
	mkdir -p $(BUILD_DIR)
solver: build_dir
	$(CC) $(CC_FLAGS) solver.c -o build/solver $(LIBS)

.PHONY : clean
clean:
	rm -r $(BUILD_DIR)
