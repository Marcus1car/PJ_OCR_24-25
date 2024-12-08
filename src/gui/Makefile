# Makefile

CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0` -Wall -O3 -g -fsanitize=address
LDLIBS = `pkg-config --libs gtk+-3.0` -lm -lSDL2 -lSDL2_image -g -fsanitize=address

SRCS = gui.c ../preprocessing/preprocess.c ../neural_network/core/lib/ocr.c ../neural_network/core/lib/core_network.c

OBJS = $(SRCS:.c=.o)

EXE = gui

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(OBJS) -o $(EXE) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJS) $(EXE)
