#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <stdlib.h>
#include <time.h>

#include "core_network.h"

#define OUTPUT_SIZE 26
#define IMG_H 32
#define IMG_W 32

/**
 * @brief Returns a pointer to an Neural network struct initialized for OCR
 *
 * @param hidden The size of the hidden layer
 * @return A pointer to the initialized neural network
 */
Network* init_ocr(size_t hidden) {
  if (hidden <= 0) {
    errx(EXIT_FAILURE, "Invalid hidden layer size");
  }
  return init_nn(IMG_H * IMG_W, hidden, OUTPUT_SIZE, RELU, SOFTMAX);
}

/**
 * @brief Loads an SDL_Surface from disk
 *
 * @param path The path of the image
 * @return A pointer to the SDL Surface corresponding to the loaded image
 */
SDL_Surface* load_image(const char* path) {
  SDL_Surface* t = IMG_Load(path);
  SDL_Surface* img = SDL_ConvertSurfaceFormat(t, SDL_PIXELFORMAT_RGB888, 0);
  SDL_FreeSurface(t);
  return img;
}

/**
 * @brief Compute the OTSU threshold of a surface
 *
 * @param surface - An SDL surface to which OTSU threshold needs to be computed
 * @return The otsu threshold
 */
int calculate_otsu_threshold(SDL_Surface* surface) {
  int width = surface->w;
  int height = surface->h;

  Uint32* pixels = (Uint32*)surface->pixels;

  int histogram[256] = {0};

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      Uint32 pixel = pixels[(y * width) + x];
      Uint8 r, g, b;
      SDL_GetRGB(pixel, surface->format, &r, &g, &b);
      Uint8 gray = (Uint8)(0.299 * r + 0.587 * g + 0.114 * b);
      histogram[gray]++;
    }
  }
  int total_pixels = width * height;
  int sumB = 0;
  int wB = 0;
  double max_variance = 0;
  int threshold = 0;
  int sum1 = 0;

  for (int i = 0; i < 256; i++) {
    sum1 += i * histogram[i];
  }

  for (int i = 0; i < 256; i++) {
    wB += histogram[i];
    if (wB == 0)
      continue;

    int wF = total_pixels - wB;
    if (wF == 0)
      break;

    sumB += i * histogram[i];
    double mB = (double)sumB / wB;
    double mF = (double)(sum1 - sumB) / wF;

    double variance = (double)wB * wF * (mB - mF) * (mB - mF);
    if (variance > max_variance) {
      max_variance = variance;
      threshold = i;
    }
  }

  return threshold;
}

/**
 * @brief Converts a surface to binary black and white - in place - using OTSU
 * thresholding
 *
 * @param surface The surface to convert to black and white
 */
void to_bw(SDL_Surface* surface) {
  int threshold = calculate_otsu_threshold(surface);
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
      Uint8 gray = (Uint8)(0.299 * r + 0.587 * g + 0.114 * b);
      Uint8 color_value = (gray > threshold) ? 255 : 0;

      Uint32 color =
          SDL_MapRGB(surface->format, color_value, color_value, color_value);
      pixels[(y * surface->w) + x] = color;
    }
  }
  if (SDL_MUSTLOCK(surface))
    SDL_UnlockSurface(surface);
}

/**
 * @brief Converts a surface to grayscale - in place
 *
 * @param surface The surface to convert to grayscale
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

/**
 * @brief Returns a pointer to double list of size IMG_H * IMG_W of pixel light
 * intensity (i.e. gray scale value of pixel), between 0 (black) and 1 (white)
 *
 * @param surface The surface to convert to double list
 * @return A double pointer correspoding to the double list
 */
double* to_double_array(SDL_Surface* surface) {
  double* gs_array = calloc(IMG_H * IMG_W, sizeof(double));
  int width = surface->w;
  int height = surface->h;
  Uint32* pixels = (Uint32*)surface->pixels;

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      Uint32 pixel = pixels[(y * width) + x];
      Uint8 r, g, b;
      SDL_GetRGB(pixel, surface->format, &r, &g, &b);
      double gray2 = (double)(0.299 * r + 0.587 * g + 0.114 * b) / 255.;
      gs_array[y * IMG_W + x] = gray2;
    }
  }
  return gs_array;
}

/**
 * @brief Returns a pointer to double list of size OUTPUT_SIZE of prediction of
 * a surface against the neural network. Note: size of output layer should be  *
 * of OUTPUT_SIZE
 *
 * @param ocr The neural network to predict against
 * @param surface The surface to perform the test
 * @return A double pointer correspoding to the double list of output
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
  // NOTE: Assumes NN output size is OUTPUT_SIZE or more (expect segfault
  // otherwise)
  predict_nn(ocr, gs_array);
  for (size_t k = 0; k < OUTPUT_SIZE; k++) {
    result[k] = ocr->output[k];
  }
  free(gs_array);
  return result;
}

/*** Helper functions ***/

/**
 * @brief UNSAFE - returns a double array of expected results from neural
 * network based on the first letter of the loaded image
 *
 * @param filename Path to the filename. Must be at least one character long and
 * the first character must be a-z
 * @return A double pointer correspoding to the double list of the expected
 * results
 */
double* get_target(const char* filename) {
  double* res = calloc(26, sizeof(double));
  res[filename[0] - 'a'] = 1;
  return res;
}

/**
 * @brief Swaps two doubles
 *
 * @param a Address of pointer to a
 * @param b Address of pointer to b
 */
void swap(double** a, double** b) {
  double* temp = *a;
  *a = *b;
  *b = temp;
}

/**
 * @brief Shuffles two arrays of double at the same time, which both are of size
 * size
 *
 * @param array1 Pointer to a double array
 * @param array2 Pointer to the second double array
 * @param size size of both arrays
 */
void shuffle(double** array1, double** array2, size_t size) {
  srand(time(NULL));
  for (int i = size - 1; i > 0; i--) {
    int j = rand() % (i + 1);
    swap(&array1[i], &array1[j]);
    swap(&array2[i], &array2[j]);
  }
}

/**
 * @brief Prints to stdout a double between 0 and 1 mapped to color gradients
 * blue to violed, and rounded to 3 decimals.
 *
 * @param value double to print
 */
void printColor(double value) {
  if (value < 0.0)
    value = 0.0;
  if (value > 1.0)
    value = 1.0;

  int red = (int)(value * 128);
  int green = 0;
  int blue = (int)(255 - value * 127);
  printf("\033[38;2;%d;%d;%dm%.3f\033[0m", red, green, blue, value);
}

/**
 * @brief Return the index of the maximum in a double array of size size
 *
 * @param arr the array
 * @param size the size of the array
 * @return The index of the max
 */
int indexOfMax(double* arr, size_t size) {
  if (size == 0) {
    return -1;
  }

  int maxIndex = 0;
  for (size_t i = 1; i < size; i++) {
    if (arr[i] > arr[maxIndex]) {
      maxIndex = i;
    }
  }

  return maxIndex;
}

/**
 * @brif Return the position of the element ith element if the array was sorted
 * in decending order (position starts at 1)
 *
 * @param arr the array
 * @param size the size of the array
 * @param i the position of the element in the array to get the position of
 * @return The value of the rank of the ith element (starts at 1)
 */
int get_rank(double* arr, size_t size, size_t i) {
  int rank = 1;
  double target = arr[i];

  for (size_t j = 0; j < size; j++) {
    if (arr[j] > target) {
      rank++;
    }
  }

  return rank;
}

/**
 * @brief prints the current iteration of learning. Only used internally for
 * debugging purposes.
 */
void print_current_iter(const Network* net,
                        const char current_letter,
                        const size_t iter,
                        const size_t max_iter) {
  printf("--- %ld/%ld ---\n", iter, max_iter);
  printf("CHAR = %c: [", current_letter);
  for (char i = 0; i < 26; i++) {
    printf("%c = ", 'A' + i);
    printColor(net->output[(size_t)i]);
    printf(", ");
  }
  printf("] Best guess: %c ", indexOfMax(net->output, 26) + 'A');
  printf("Rank: ");
  int rank = get_rank(net->output, 26, current_letter - 'A');
  if (rank == 1) {
    printf("\033[32m\033[1m%d\033[0m", rank);
  } else if (rank < 5) {
    printf("\033[33m\033[1m%d\033[0m", rank);
  } else {
    printf("\033[31m\033[1m%d\033[0m", rank);
  }
  if (net->output[current_letter - 'A'] < (double)0.5 && rank == 1)
    printf(" \033[31m\033[1mERROR PROBA\033[0m");
  printf("\n");
  printf("-----\n");
}