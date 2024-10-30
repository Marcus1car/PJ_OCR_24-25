# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -O2 -g
LDFLAGS = -lSDL2 -lSDL2_image -lm

# SDL2 flags - automatically detect using pkg-config if available
SDL_CFLAGS := $(shell pkg-config --cflags sdl2 SDL2_image)
SDL_LDFLAGS := $(shell pkg-config --libs sdl2 SDL2_image)

# Add SDL flags to our flags
CFLAGS += $(SDL_CFLAGS)
LDFLAGS += $(SDL_LDFLAGS)

# Source files
LIB_SRCS = preprocess.c
LIB_OBJS = $(LIB_SRCS:.c=.o)
MAIN_SRCS = main.c
MAIN_OBJS = $(MAIN_SRCS:.c=.o)
TEST_SRCS = test_preprocess.c
TEST_OBJS = $(TEST_SRCS:.c=.o)

# Header files
HEADERS = preprocess.h

# Main targets
all: libpreprocess.a preprocess_main preprocess_test

# Static library
libpreprocess.a: $(LIB_OBJS)
	ar rcs $@ $^

# Main program
preprocess_main: $(MAIN_OBJS) libpreprocess.a
	$(CC) -o $@ $(MAIN_OBJS) -L. -lpreprocess $(LDFLAGS)

# Test program
preprocess_test: $(TEST_OBJS) libpreprocess.a
	$(CC) -o $@ $(TEST_OBJS) -L. -lpreprocess $(LDFLAGS)

# Object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(LIB_OBJS) $(MAIN_OBJS) $(TEST_OBJS) libpreprocess.a preprocess_main preprocess_test test_*.png

# Test target
test: preprocess_test
	./preprocess_test

.PHONY: all clean test