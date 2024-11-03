#ifndef DETECT_GRID_H
#define DETECT_GRID_H

#include "grid_utils.h"
#include "homography.h"
#include "pixel_operations.h"
#include "segmentation.h"
#include "tuple.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

SDL_Surface *detect_grid(const char *file);
size_t findgrid(CCTuple *histo, size_t histo_size, size_t *coords[],
                SDL_Surface **surf[]);
int get_distance(int X1, int Y1, int X2, int Y2);

#endif