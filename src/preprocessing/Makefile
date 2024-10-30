# Specify the compiler
CC = gcc

# Compiler flags to include SDL2
CFLAGS = -Wall -I/usr/include/SDL2 -D_REENTRANT

# Linker flags for SDL2
LDFLAGS = -lSDL2 -lSDL2_image -lm

# Source files
SOURCES_PREPROCESS = preprocess.c
SOURCES_ROTATE = man_rota.c

# Executable names
TARGET_PREPROCESS = preprocess
TARGET_ROTATE = man_rota

# Build main preprocessing executable
$(TARGET_PREPROCESS): $(SOURCES_PREPROCESS)
	$(CC) $(CFLAGS) -o $(TARGET_PREPROCESS) $(SOURCES_PREPROCESS) $(LDFLAGS)

# Build rotate_image executable
$(TARGET_ROTATE): $(SOURCES_ROTATE)
	$(CC) $(CFLAGS) -o $(TARGET_ROTATE) $(SOURCES_ROTATE) $(LDFLAGS)

# Clean up build files
clean:
	rm -f $(TARGET_PREPROCESS) $(TARGET_ROTATE) *.o
