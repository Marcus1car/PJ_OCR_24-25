CC = gcc
CC_FLAGS = -Wall -Wextra -std=c17 -fsanitize=address -g
LIBS = -lm
SDL_LIBS = -lSDL2 -lSDL2_image
BUILD_DIR = ./build
DEPS = neural.c

all: poc training_images

build_dir:
	mkdir -p $(BUILD_DIR)
poc: build_dir
	$(CC) $(CC_FLAGS) $(DEPS) poc.c -o build/poc $(LIBS)

poc_load: build_dir
	$(CC) $(CC_FLAGS) $(DEPS) poc_load.c -o build/poc_load $(LIBS)

training_images: build_dir
	$(CC) $(CC_FLAGS) $(DEPS) training_images.c -o build/training_images $(LIBS) $(SDL_LIBS)

.PHONY : clean
clean:
	rm -r $(BUILD_DIR)
