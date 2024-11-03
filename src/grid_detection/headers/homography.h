#ifndef HOMOGRAPHY_H
#define HOMOGRAPHY_H

#include "matrix.h"
#include "pixel_operations.h"
#include "tuple.h"
#include <SDL2/SDL.h>
#include <math.h>
#include <stdlib.h>

Matrix *findhomography(double *src, double size);

SDL_Surface *transformimage(Matrix *transform, SDL_Surface *image, double size);

#endif