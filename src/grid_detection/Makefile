#-*-Makefile-*-

#target : dependencies
#	action

# Setting the compiler and the default linker program
CC = gcc
# options for pre-processor (-I, -include, -D ... )
CPPFLAGS =
# main compilation options
CFLAGSNE = -Wall -Wextra -std=c99 -O1 `pkg-config --cflags sdl2` `pkg-config --cflags --libs gtk+-3.0 gdk-pixbuf-2.0` -I..
CFLAGS = -Wall -Wextra -Werror -std=c99 -O1 `pkg-config --cflags sdl2` `pkg-config --cflags --libs gtk+-3.0 gdk-pixbuf-2.0`
# Linker options (probably always empty)
LDFLAGS =
# libs and path for linker
LDLIBS = -lm `pkg-config --libs sdl2 gtk+-3.0 gdk-pixbuf-2.0` -lSDL2_image -lSDL2_gfx

all : main

main : sources/main.o sources/detect_grid.o sources/grid_utils.o sources/segmentation.o sources/pixel_operations.o linked_list.o sources/set.o sources/tuple.o sources/heapsort.o sources/matrix.o sources/homography.o
	$(CC) $(CFLAGSNE) sources/main.o sources/detect_grid.o sources/grid_utils.o sources/segmentation.o sources/pixel_operations.o linked_list.o sources/set.o sources/tuple.o sources/heapsort.o sources/matrix.o sources/homography.o -o main $(LDLIBS)

main.o : sources/main.c
	$(CC) -c $(CFLAGSNE) sources/main.c -o sources/main.o $(LDLIBS)

sources/detect_grid.o : sources/detect_grid.c
	$(CC) -c $(CFLAGSNE) sources/detect_grid.c -o sources/detect_grid.o $(LDLIBS)

sources/grid_utils.o : sources/grid_utils.c
	$(CC) -c $(CFLAGSNE) sources/grid_utils.c -o sources/grid_utils.o $(LDLIBS)

sources/segmentation.o : sources/segmentation.c
	$(CC) -c $(CFLAGSNE) sources/segmentation.c -o sources/segmentation.o $(LDLIBS)

sources/pixel_operations.o : sources/pixel_operations.c
	$(CC) -c $(CFLAGSNE) sources/pixel_operations.c -o sources/pixel_operations.o $(LDLIBS)

linked_list.o : linked_list.c
	$(CC) -c $(CFLAGSNE) linked_list.c -o linked_list.o $(LDLIBS)

sources/set.o : sources/set.c
	$(CC) -c $(CFLAGSNE) sources/set.c -o sources/set.o $(LDLIBS)

sources/tuple.o : sources/tuple.c
	$(CC) -c $(CFLAGSNE) sources/tuple.c -o sources/tuple.o $(LDLIBS)

sources/heapsort.o : sources/heapsort.c
	$(CC) -c $(CFLAGSNE) sources/heapsort.c -o sources/heapsort.o $(LDLIBS)

sources/matrix.o : sources/matrix.c
	$(CC) -c $(CFLAGSNE) sources/matrix.c -o sources/matrix.o $(LDLIBS)

sources/homography.o : sources/homography.c
	$(CC) -c $(CFLAGSNE) sources/homography.c -o sources/homography.o $(LDLIBS)



clean :
	rm  sources/*.o main  *.o
