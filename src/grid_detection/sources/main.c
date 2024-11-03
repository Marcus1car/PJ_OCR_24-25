#include "../headers/detect_grid.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Error in main execution\n");
        printf("Usage: ./main <image location>\n");
        return 1;
    }
    SDL_Init(SDL_INIT_VIDEO);
    const char *file = argv[1];
    detect_grid(file);
    return 0;
}