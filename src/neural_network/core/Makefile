#PWD = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
PWD = .
CC_NVIDIA = nvc
NVC_FLAGS = -fast -gpu:mem:managed -Minfo=all
NVC_LIBS = -lm -lSDL2 -lSDL2_image
NVC_PMGMT = -acc


CC = gcc
CC_FLAGS = -Wall -Wextra -Wshadow -Wformat -Winit-self -Wuninitialized -Wmissing-include-dirs -Wparentheses -Wunused -Wmaybe-uninitialized -std=c17 -fsanitize=address -g -O2
LIBS = -lm
SDL_LIBS = -lSDL2 -lSDL2_image
BUILD_DIR = ./build/
DEPS = $(PWD)/lib/core_network.c
DEPS_OCR = $(PWD)/lib/ocr.c
MPMGMT = -fopenacc -foffload=-lm #-foffload=nvptx-none   -foffload=-lm

POC 			= $(CC) $(CC_FLAGS) $(DEPS) $(PWD)/poc.c -o $(BUILD_DIR)/poc $(LIBS)
TRAINING_IMGS	= $(CC) $(CC_FLAGS) $(DEPS) $(DEPS_OCR) $(PWD)/training_images.c -o $(BUILD_DIR)/training_images $(LIBS) $(SDL_LIBS)
POC_LOAD		= $(CC) $(CC_FLAGS) $(DEPS) $(PWD)/poc_load.c -o $(BUILD_DIR)/poc_load $(LIBS) 
TEST_ACCURACY	= $(CC) $(CC_FLAGS) $(DEPS) $(DEPS_OCR) $(PWD)/test_accuracy.c -o $(BUILD_DIR)/test_accuracy $(LIBS) $(SDL_LIBS)
TEST_IMAGE		= $(CC) $(CC_FLAGS) $(DEPS) $(DEPS_OCR) $(PWD)/test_image.c -o $(BUILD_DIR)/test_image $(LIBS) $(SDL_LIBS)

all: poc training_images poc_load test_accuracy test_image
#all_para: poc_para training_images_para poc_load_para 
all_nvc: nvc_training_images

build_dir:
	mkdir -p $(BUILD_DIR)


poc: build_dir
	$(POC)

poc_load: build_dir
	$(POC_LOAD)

training_images: build_dir
	$(TRAINING_IMGS)

test_accuracy: build_dir
	$(TEST_ACCURACY)

test_image: build_dir
	$(TEST_IMAGE)

# poc_para: build_dir
# 	$(POC) $(MPMGMT)

# poc_load_para: build_dir
# 	$(POC_LOAD) $(MPMGMT)

# training_images_para: build_dir
# 	$(TRAINING_IMGS) $(MPMGMT)

#parallel compilation using openaac and nvc compiler (Nvidia HPC SDK)
nvc_training_images: build_dir
	$(CC_NVIDIA) $(NVC_PMGMT) $(CC_FLAGS) $(DEPS) $(DEPS_OCR) training_images.c -o $(BUILD_DIR)/parallel_training_images $(NVC_LIBS)

.PHONY : clean
clean:
	rm -r $(BUILD_DIR)
