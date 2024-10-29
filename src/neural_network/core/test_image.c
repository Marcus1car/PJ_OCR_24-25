#include <err.h>
#include <stdlib.h>
#include "ocr.h"

int main(int argc, char** argv) {
  if (argc != 4) {
    errx(EXIT_FAILURE,
         "Usage test_image <ocr model data> <image path> <0|1, 1 = bw; 0 = "
         "gray scale>");
  }

  Network* ocr = load_nn_data(argv[1]);
  if (ocr == NULL)
    errx(EXIT_FAILURE, "Error loading the ocr model");

  SDL_Surface* img = load_image(argv[2]);
  if (img == NULL)
    errx(EXIT_FAILURE, "Error loading the ocr model");
  int nb_or_gs = atoi(argv[3]);
  if (nb_or_gs == 0) {
    to_gs(img);

  } else {
    to_bw(img);
  }
  double* result = predict_from_surface(ocr, img);
  network_free(ocr);

  printf("Predictions: \n");
  for (size_t k = 0; k < OUTPUT_SIZE; k++) {
    printf("%c - %3.3f\n", 'a' + k, result[k]);
  }

  free(result);

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window* window =
      SDL_CreateWindow("SDL Display Surface", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, 480, 480, SDL_WINDOW_SHOWN);
  if (!window) {
    printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Renderer* renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, img);
  SDL_FreeSurface(img);  // Free the surface after creating the texture
  if (!texture) {
    printf("Unable to create texture! SDL_Error: %s\n", SDL_GetError());
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }
  SDL_FreeSurface(img);


  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);

  SDL_Event e;
  int quit = 0;
  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = 1;
      }
    }
  }
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

  return EXIT_SUCCESS;
}