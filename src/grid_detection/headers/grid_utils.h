#ifndef GRID_UTILS_H
#define GRID_UTILS_H

#include "grid_utils.h"
#include "pixel_operations.h"
#include "segmentation.h"
#include "tuple.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <dirent.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtk.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void colorizeimage(SDL_Surface *image, SDL_Surface *segmap);

void remove_grid(SDL_Surface *image, SDL_Surface *segmap, size_t objval, int b);

void find_extremity_coordinates(SDL_Surface *segmap, size_t objval, size_t *top,
                                size_t *right, size_t *left, size_t *bottom);

void find_corner_coordinates(SDL_Surface *segmap, size_t objval,
                             size_t *topleftX, size_t *topleftY,
                             size_t *toprightX, size_t *toprightY,
                             size_t *bottomleftX, size_t *bottomleftY,
                             size_t *bottomrightX, size_t *bottomrightY);

SDL_Surface *cut_segmap(SDL_Surface *image, size_t top, size_t bottom,
                        size_t left, size_t right);

SDL_Surface *cut_image(SDL_Surface *image, size_t top, size_t bottom,
                       size_t left, size_t right);

void save_cells(const char *file);

SDL_Surface *reduce_res(SDL_Surface *image);

void copy_symbol(SDL_Surface *output, SDL_Surface *input, size_t left,
                 size_t top);

int remove_dir(const char *folder_name);

#endif
