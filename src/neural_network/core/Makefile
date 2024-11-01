CC_NVIDIA = nvc
NVC_FLAGS = -fast -gpu:mem:managed -Minfo=all
NVC_LIBS = -lm -lSDL2 -lSDL2_image
NVC_PMGMT = -acc


CC = gcc
CC_FLAGS = -Wall -Wextra -std=c17 -fsanitize=address -g -O2 -pedantic -Wshadow 
LIBS = -lm
SDL_LIBS = -lSDL2 -lSDL2_image
BUILD_DIR = ./build
DEPS = core_network.c
OPENMP = -fopenacc -foffload=-lm #-foffload=nvptx-none   -foffload=-lm

POC 			= $(CC) $(CC_FLAGS) $(DEPS) poc.c -o $(BUILD_DIR)/poc $(LIBS)
TRAINING_IMGS	= $(CC) $(CC_FLAGS)  $(DEPS) ocr.c training_images.c -o $(BUILD_DIR)/training_images $(LIBS) $(SDL_LIBS)
POC_LOAD		= $(CC) $(CC_FLAGS) $(DEPS) poc_load.c -o $(BUILD_DIR)/poc_load $(LIBS) 

all: poc training_images poc_load
all_para: poc_para training_images_para poc_load_para
all_nvc: nvc_training_images

build_dir:
	mkdir -p $(BUILD_DIR)


poc: build_dir
	$(POC)

poc_load: build_dir
	$(POC_LOAD)

training_images: build_dir
	$(TRAINING_IMGS)

# poc_para: build_dir
# 	$(POC) $(OPENMP)

# poc_load_para: build_dir
# 	$(POC_LOAD) $(OPENMP)

# training_images_para: build_dir
# 	$(TRAINING_IMGS) $(OPENMP)

#parallel compilation using openaac and nvc compiler (Nvidia HPC SDK)
nvc_training_images: build_dir
	$(CC_NVIDIA) $(NVC_PMGMT) $(CC_FLAGS) neural.c training_images.c -o $(BUILD_DIR)/parallel_training_images $(NVC_LIBS)

.PHONY : clean
clean:
	rm -r $(BUILD_DIR)
