#ifndef OCR_H
#define OCR_H

#define OUTPUT_SIZE 26
#define IMG_H 32
#define IMG_W 32
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <time.h>
#include <stdlib.h>

#include "core_network.h"

double* predict_from_surface(Network* ocr, SDL_Surface* surface);
void to_gs(SDL_Surface* surface);
void to_bw(SDL_Surface* surface);
double* to_double_array(SDL_Surface* surface);
Network* init_ocr(size_t hidden);
SDL_Surface* load_image(const char* path);

/** Helper functions **/
void print_current_iter(const Network* net,
                        const char current_letter,
                        const size_t iter,
                        const size_t max_iter);
int get_rank(double* arr, size_t size, size_t i);
int indexOfMax(double* arr, size_t size);
void printColor(double value);
void shuffle(double** array1, double** array2, size_t size);
void swap(double** a, double** b);
double* get_target(const char* filename);

#endif