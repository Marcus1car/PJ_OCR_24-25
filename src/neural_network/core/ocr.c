#include <SDL2/SDL.h>
#include <err.h>
#include <stdlib.h>
#include <SDL2/SDL_image.h>

#include "neural.h"

#define OUTPUT_SIZE 26
#define IMG_H 32
#define IMG_W 32

/*
 * @brief Returns a pointer to an Neural network struct initialized for OCR
 *
 * @param hidden The size of the hidden layer
 */
Network* init_ocr(size_t hidden) {
  if (hidden <= 0) {
    errx(EXIT_FAILURE, "Invalid hidden layer size");
  }
  return network_init(IMG_H * IMG_W, hidden, OUTPUT_SIZE, RELU, SOFTMAX);
}

/*
 * @brief Loads an SDL_Surface from disk
 *
 * @param path The path of the image
 */
SDL_Surface* load_image(const char* path) {
  SDL_Surface* t = IMG_Load(path);
  SDL_Surface* img = SDL_ConvertSurfaceFormat(t, SDL_PIXELFORMAT_RGB888, 0);
  SDL_FreeSurface(t);
  return img;
}

/*
 * @brief Convert a surface to grayscale
 *
 * @param surface - An SDL surface to convert to grayscale on place
 */
void to_gs(SDL_Surface* surface) {
  int width = surface->w;
  int height = surface->h;
  if (SDL_MUSTLOCK(surface))
    SDL_LockSurface(surface);

  Uint32* pixels = (Uint32*)surface->pixels;

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      Uint32 pixel = pixels[(y * width) + x];
      Uint8 r, g, b;
      SDL_GetRGB(pixel, surface->format, &r, &g, &b);
      double gray2 = (double)(0.299 * r + 0.587 * g + 0.114 * b) / 255;
      Uint8 color_value = (Uint8)(gray2 * 255);

      Uint32 color =
          SDL_MapRGB(surface->format, color_value, color_value, color_value);
      pixels[(y * surface->w) + x] = color;
    }
  }
  if (SDL_MUSTLOCK(surface))
    SDL_UnlockSurface(surface);
}

/*
 * @brief Returns a pointer to double list of size OUTPUT_SIZE of prediction of
 * a surface against the neural network. Note: size of output layer should be  * of OUTPUT_SIZE
 *
 * @param ocr The neural network to predict against
 * @param surface The surface to perform the test
 */

double* predict_from_surface(Network* ocr, SDL_Surface* surface) {
  if (surface->h != IMG_H || surface->w != IMG_W)
    errx(EXIT_FAILURE, "Invalid image size: predict_from_surface()");

  to_gs(surface);

  double* gs_array = calloc(IMG_H * IMG_W, sizeof(double));
  double* result = calloc(OUTPUT_SIZE, sizeof(double));
  if (gs_array == NULL || result == NULL)
    errx(EXIT_FAILURE, "Memory allocation failed: predict_from_surface()");

  int width = surface->w;
  int height = surface->h;
  Uint32* pixels = (Uint32*)surface->pixels;

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      Uint32 pixel = pixels[(y * width) + x];
      Uint8 r, g, b;
      SDL_GetRGB(pixel, surface->format, &r, &g, &b);
      double gray2 = (double)(0.299 * r + 0.587 * g + 0.114 * b) / 255;
      gs_array[y * IMG_W + x] = ((double)gray2);
    }
  }
  // NOTE: Assumes NN output size is 26 or more (expect segfault otherwise)
  network_predict(ocr, gs_array);
  for (size_t k = 0; k < OUTPUT_SIZE; k++) {
    result[k] = ocr->output[k];
  }
  free(gs_array);
  return result;
}