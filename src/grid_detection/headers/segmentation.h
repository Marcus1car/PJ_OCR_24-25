#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include "../linked_list.h"
#include "heapsort.h"
#include "pixel_operations.h"
#include "set.h"
#include "tuple.h"
#include <SDL2/SDL.h>

CCTuple *twopassSegmentation(SDL_Surface *image, SDL_Surface *segmap,
                             size_t *segval);

size_t propagate(SDL_Surface *image, SDL_Surface *segmap, int segval, size_t x,
                 size_t y);

#endif
