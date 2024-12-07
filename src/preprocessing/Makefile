# Specify the compiler
CC = gcc

# Compiler flags to include SDL2
CFLAGS = -Wall -I/usr/include/SDL2 -I. -D_REENTRANT

# Linker flags for SDL2
LDFLAGS = -lSDL2 -lSDL2_image -lm

# Source files
SOURCES_PREPROCESS = preprocess.c
SOURCES_ROTATE = man_rota.c
SOURCES_AUTO_ROTATE = auto_rota.c
SOURCES_UTILS = preprocess_utils.c # New source file for functions from preprocess.c

# Object files
OBJS_PREPROCESS = preprocess.o
OBJS_ROTATE = man_rota.o preprocess_utils.o # Include preprocess_utils.o for man_rota
OBJS_AUTO_ROTATE = auto_rota.o preprocess_utils.o # Include preprocess_utils.o for auto_rota

# Executable names
TARGET_PREPROCESS = preprocess
TARGET_ROTATE = man_rota
TARGET_AUTO_ROTATE = auto_rota

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build main preprocessing executable
$(TARGET_PREPROCESS): $(OBJS_PREPROCESS)
	$(CC) $(CFLAGS) -o $(TARGET_PREPROCESS) $(OBJS_PREPROCESS) $(LDFLAGS)

# Build rotate_image executable
$(TARGET_ROTATE): $(OBJS_ROTATE)
	$(CC) $(CFLAGS) -o $(TARGET_ROTATE) $(OBJS_ROTATE) $(LDFLAGS)

# Build auto_rota executable
$(TARGET_AUTO_ROTATE): $(OBJS_AUTO_ROTATE)
	$(CC) $(CFLAGS) -o $(TARGET_AUTO_ROTATE) $(OBJS_AUTO_ROTATE) $(LDFLAGS)

# Clean up build files
clean:
	rm -f $(OBJS_PREPROCESS) $(OBJS_ROTATE) $(OBJS_AUTO_ROTATE) $(TARGET_PREPROCESS) $(TARGET_ROTATE) $(TARGET_AUTO_ROTATE)
