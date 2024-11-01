#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <dirent.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int gaussian_kernel[3][3] = {{0, 1, 0}, {1, 6, 1}, {0, 1, 0}};
const int kernel_sum = 10;

/**
 * @brief Apply a gaussian blur according to the kernel set above - in place
 *
 * @param surface The surface to apply gaussian blur
 */
void apply_gaussian_blur(SDL_Surface* surface) {
  if (SDL_LockSurface(surface) != 0) {
    printf("Failed to lock surface: %s\n", SDL_GetError());
    return;
  }

  SDL_Surface* temp_surface = SDL_ConvertSurface(surface, surface->format, 0);
  if (!temp_surface) {
    printf("Failed to create temporary surface: %s\n", SDL_GetError());
    SDL_UnlockSurface(surface);
    return;
  }

  Uint32* pixels = (Uint32*)surface->pixels;
  Uint32* temp_pixels = (Uint32*)temp_surface->pixels;

  int width = surface->w;
  int height = surface->h;

  for (int y = 1; y < height - 1; y++) {
    for (int x = 1; x < width - 1; x++) {
      int r = 0, g = 0, b = 0;

      for (int ky = -1; ky <= 1; ky++) {
        for (int kx = -1; kx <= 1; kx++) {
          int pixel_x = x + kx;
          int pixel_y = y + ky;
          Uint32 pixel_color = temp_pixels[pixel_y * width + pixel_x];

          Uint8 pr, pg, pb;
          SDL_GetRGB(pixel_color, surface->format, &pr, &pg, &pb);

          int kernel_value = gaussian_kernel[ky + 1][kx + 1];
          r += pr * kernel_value;
          g += pg * kernel_value;
          b += pb * kernel_value;
        }
      }

      r /= kernel_sum;
      g /= kernel_sum;
      b /= kernel_sum;

      pixels[y * width + x] = SDL_MapRGB(surface->format, r, g, b);
    }
  }

  SDL_FreeSurface(temp_surface);
  SDL_UnlockSurface(surface);
}

/**
 * @brief Add noise in place to the surface
 *
 * @param surface The surface to apply noise
 * @param intensity 0-100, specifies the noise intensity
 */
void add_noise(SDL_Surface* surface, int intensity) {
  int num_pixels = (surface->w * surface->h * intensity) / 100;
  for (int i = 0; i < num_pixels; ++i) {
    int x = rand() % surface->w;
    int y = rand() % surface->h;
    Uint32* pixels = (Uint32*)surface->pixels;
    Uint32 color = SDL_MapRGB(surface->format, rand() % 127 + 128,
                              rand() % 127 + 128, rand() % 127 + 128);
    pixels[y * surface->w + x] = color;
  }
}

/*
 * @brief Add artifacts in place to the surface
 *
 * @param surface The surface to artifacts
 * @param num_artifacts The number of artifacts to generate
 */
void add_artifacts(SDL_Surface* surface, int num_artifacts) {
  Uint32* pixels = (Uint32*)surface->pixels;
  int width = surface->w;
  int height = surface->h;

  for (size_t i = 0; i < num_artifacts; i++) {
    Uint8 color_value = rand() % 256;
    Uint8 alpha = (Uint8)(rand() % 128 + 128);
    Uint32 color = SDL_MapRGBA(surface->format, color_value, color_value,
                               color_value, alpha);

    int center_x = rand() % width;
    int center_y = rand() % height;
    int radius = rand() % 5 + 3;

    for (int y = -radius; y <= radius; y++) {
      for (int x = -radius; x <= radius; x++) {
        int posX = center_x + x;
        int posY = center_y + y;

        if (posX >= 0 && posX < width && posY >= 0 && posY < height &&
            x * x + y * y <= radius * radius) {
          Uint32 current_pixel = pixels[posY * width + posX];
          Uint8 pr, pg, pb, pa;
          SDL_GetRGBA(current_pixel, surface->format, &pr, &pg, &pb, &pa);

          pr = (Uint8)((pr * (255 - alpha) + color_value * alpha) / 255);
          pg = (Uint8)((pg * (255 - alpha) + color_value * alpha) / 255);
          pb = (Uint8)((pb * (255 - alpha) + color_value * alpha) / 255);

          pixels[posY * width + posX] =
              SDL_MapRGBA(surface->format, pr, pg, pb, 255);
        }
      }
    }
  }
}

/**
 * @brief Add subtle artifacts in place to the surface (less visible than
 * add_artifacts)
 *
 * @param surface The surface to artifacts
 * @param num_artifacts The number of artifacts to generate
 */
void add_subtle_artifacts(SDL_Surface* surface, int num_artifacts) {
  Uint32* pixels = (Uint32*)surface->pixels;
  int width = surface->w;
  int height = surface->h;

  for (size_t i = 0; i < num_artifacts; i++) {
    Uint8 color_value = rand() % 128 + 128;
    int max_alpha = rand() % 64 + 64;
    int radius = rand() % 8 + 4;

    int center_x = rand() % width;
    int center_y = rand() % height;

    for (int y = -radius; y <= radius; y++) {
      for (int x = -radius; x <= radius; x++) {
        int posX = center_x + x;
        int posY = center_y + y;

        int d_sqr = x * x + y * y;
        if (posX >= 0 && posX < width && posY >= 0 && posY < height &&
            d_sqr <= radius * radius) {
          double distance = sqrt(d_sqr);
          int alpha = (int)(max_alpha * (1 - distance / radius));
          Uint8 faded_alpha =
              alpha > 0 ? alpha : 0;  // Clamp to positive values

          Uint32 current_pixel = pixels[posY * width + posX];
          Uint8 pr, pg, pb, pa;
          SDL_GetRGBA(current_pixel, surface->format, &pr, &pg, &pb, &pa);

          Uint8 final_r =
              (Uint8)((pr * (255 - faded_alpha) + color_value * faded_alpha) /
                      255);
          Uint8 final_g =
              (Uint8)((pg * (255 - faded_alpha) + color_value * faded_alpha) /
                      255);
          Uint8 final_b =
              (Uint8)((pb * (255 - faded_alpha) + color_value * faded_alpha) /
                      255);
          pixels[posY * width + posX] =
              SDL_MapRGBA(surface->format, final_r, final_g, final_b, 255);
        }
      }
    }
  }
}

/**
 * @brief Generates an image and saves it to the specified path
 *
 * @param renderer The renderer where to generate the texture
 * @param font Font of the letter
 * @param letter The letter to generate
 * @param output_path Where to save the image
 * @param angle The tilt angle of the image
 * @param noise_intensity The noise percentage to apply
 * @param clean boolean, apply  gaussian blur, artifacts and noise if
 * set to 0
 */
void render_and_save_letter(SDL_Renderer* renderer,
                            TTF_Font* font,
                            char letter,
                            const char* output_path,
                            double angle,
                            int noise_intensity,
                            int clean) {
  Uint8 gray_value = rand() % 128;
  SDL_Color text_color = {gray_value, gray_value, gray_value, 255};
  char text[2] = {letter, '\0'};

  SDL_Surface* text_surface = TTF_RenderText_Solid(font, text, text_color);
  if (!text_surface) {
    printf("TTF_RenderText_Solid error: %s\n", TTF_GetError());
    return;
  }

  SDL_Texture* text_texture =
      SDL_CreateTextureFromSurface(renderer, text_surface);
  SDL_FreeSurface(text_surface);

  if (!text_texture) {
    printf("SDL_CreateTextureFromSurface error: %s\n", SDL_GetError());
    return;
  }

  int tex_w = 0, tex_h = 0;
  SDL_QueryTexture(text_texture, NULL, NULL, &tex_w, &tex_h);
  SDL_Rect dstrect = {16 - tex_w / 2, 16 - tex_h / 2, tex_w, tex_h};

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);
  SDL_RenderCopyEx(renderer, text_texture, NULL, &dstrect, angle, NULL,
                   SDL_FLIP_NONE);

  Uint32 pixel_format;
  int renderer_w, renderer_h;
  SDL_RendererInfo renderer_info;
  SDL_GetRendererInfo(renderer, &renderer_info);
  pixel_format = renderer_info.texture_formats[0];
  SDL_GetRendererOutputSize(renderer, &renderer_w, &renderer_h);

  if (renderer_w != 32 || renderer_h != 32) {
    printf("Renderer output size mismatch! Expected 32x32, got %dx%d\n",
           renderer_w, renderer_h);
    SDL_DestroyTexture(text_texture);
    return;
  }

  SDL_Surface* image_surface =
      SDL_CreateRGBSurfaceWithFormat(0, 32, 32, 32, pixel_format);
  if (!image_surface) {
    printf("SDL_CreateRGBSurfaceWithFormat error: %s\n", SDL_GetError());
    SDL_DestroyTexture(text_texture);
    return;
  }

  if (SDL_RenderReadPixels(renderer, NULL, image_surface->format->format,
                           image_surface->pixels, image_surface->pitch) != 0) {
    printf("SDL_RenderReadPixels error: %s\n", SDL_GetError());
    SDL_FreeSurface(image_surface);
    SDL_DestroyTexture(text_texture);
    return;
  }
  if (clean == 0) {
    add_noise(image_surface, noise_intensity);
    add_subtle_artifacts(image_surface, rand() % 3 + 1);
    if (rand() % 2) {
      apply_gaussian_blur(image_surface);
    }
  }

  if (IMG_SavePNG(image_surface, output_path) != 0) {
    printf("Failed to save image: %s\n", IMG_GetError());
  } else {
    printf("\r\033[KSaved image: %s", output_path);
  }

  SDL_FreeSurface(image_surface);
  SDL_DestroyTexture(text_texture);
}

/**
 * @brief Generates the dataset
 *
 * @param font_folder The path to a folder containing font files
 * @param output_folder The path to a folder to save images
 * @param noise_intensity The noise percentage to apply
 */
void generate_dataset_from_fonts(const char* font_folder,
                                 const char* output_folder,
                                 int noise_intensity) {
  DIR* dir;
  struct dirent* ent;
  if ((dir = opendir(font_folder)) != NULL) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window =
        SDL_CreateWindow("Font Rendering", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, 32, 32, SDL_WINDOW_HIDDEN);
    SDL_Renderer* renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Loop through all files in the folder
    while ((ent = readdir(dir)) != NULL) {
      char* ext = strrchr(ent->d_name, '.');
      if (ext && strcmp(ext, ".ttf") == 0) {
        char font_path[512];
        snprintf(font_path, sizeof(font_path), "%s/%s", font_folder,
                 ent->d_name);

        TTF_Font* font = TTF_OpenFont(font_path, 32);
        if (!font) {
          printf("Failed to load font: %s\n", TTF_GetError());
          continue;
        }

        for (char letter = 'A'; letter <= 'Z'; ++letter) {
          for (size_t k = 0; k < 30; k++) {
            char output_path[512];
            int random_number = (rand() % 61) - 30;
            snprintf(output_path, sizeof(output_path), "%s/%c_%d_%s_n%d.png",
                     output_folder, letter - 'A' + 'a', k, ent->d_name,
                     noise_intensity);
            render_and_save_letter(renderer, font, letter, output_path,
                                   (double)random_number, noise_intensity,
                                   rand() % 2);
          }
        }

        TTF_CloseFont(font);
      }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();

    closedir(dir);
  } else {
    printf("Could not open font directory: %s\n", font_folder);
  }
}

int main(int argc, char* argv[]) {
  if (argc != 4) {
    errx(EXIT_FAILURE,
         "Usage: %s <font_folder> <output_folder> <noise_intensity 0-100>",
         argv[0]);
  }
  srand(time(NULL));

  const char* font_folder = argv[1];
  const char* output_folder = argv[2];
  int noise_intensity = atoi(argv[3]);

  generate_dataset_from_fonts(font_folder, output_folder, noise_intensity);

  return 0;
}