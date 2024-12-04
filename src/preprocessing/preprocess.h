#ifndef PREPROCESS_H
#define PREPROCESS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// Declare external functions
extern SDL_Surface* loadImage(const char* given_path);
extern void FinalFunc(SDL_Surface *surface);

// Add other function declarations as needed
#endif