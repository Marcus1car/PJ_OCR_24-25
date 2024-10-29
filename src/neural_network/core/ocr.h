#ifndef OCR_H
#define OCR_H
#include <SDL2/SDL.h>
#include "neural.h"

double* predict_from_surface(Network* ocr, SDL_Surface* surface);
void to_gs(SDL_Surface* surface);
Network* init_ocr(size_t hidden);
SDL_Surface* load_image(const char* path);

#endif