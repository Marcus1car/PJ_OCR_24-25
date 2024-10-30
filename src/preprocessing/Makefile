# Define the compiler
CC = gcc

# Compiler flags for warnings and optimization
CFLAGS = -Wall -O2 `sdl2-config --cflags` -I.

# Linker flags for SDL2 and SDL2_image
LDFLAGS = `sdl2-config --libs` -lSDL2_image -lm

# Output binary name
TARGET = preprocess

# Source files
SRC = preprocess.c
OBJ = $(SRC:.c=.o)

# Default target to build the executable
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all clean
