
#ifndef PREPROCESS_H
#define PREPROCESS_H


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_rotozoom.h>  // Updated to SDL2
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Import functions
SDL_Surface* loadImage(const char* given_path);

// Rotation functions
SDL_Surface* man_rotation(SDL_Surface* given_image, double angle);

// Grayscale functions
void convertToGrayscale(SDL_Surface* surface);

// Noise detection and reduction
double calculateNoiseLevel(SDL_Surface* surface);
void medianFilter(SDL_Surface *surface);

// Contrast enhancement
void enhanceContrast(SDL_Surface* surface, double noiseLevel);

// Binarization
Uint8 calculateMeanLight(SDL_Surface* surface);
void convertToBlackAndWhite(SDL_Surface* surface);
void adaptivePreprocessing(SDL_Surface *surface);
#endif // PREPROCESS_H