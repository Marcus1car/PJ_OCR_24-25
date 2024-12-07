#ifndef PREPROCESS_UTILS_H
#define PREPROCESS_UTILS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// Function prototypes for all functions in preprocess_utils.c
SDL_Surface* loadImage(const char* given_path);
double noiselevel_weighted(SDL_Surface *surface);
void sortFilter(Uint8 close[], int size);
void Filterfunc(SDL_Surface *surface);
void Grayscalefunct(SDL_Surface *surface);
Uint8 meanLight(SDL_Surface *surface);
Uint8 medianLight(SDL_Surface *surface);
void binarize(SDL_Surface *surface);
void more_contrast(SDL_Surface *surface, double noiseLevel);
void FinalFunc(SDL_Surface *surface);

#endif // PREPROCESS_UTILS_H