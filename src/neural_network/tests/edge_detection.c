#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>

#define WIDTH 640
#define HEIGHT 480

Uint32 getPixel(SDL_Surface *surface, int x, int y) {
    Uint32 *pixels = (Uint32 *)surface->pixels;
    return pixels[(y * surface->w) + x];
}

void setPixel(SDL_Surface *surface, int x, int y, Uint32 pixel) {
    Uint32 *pixels = (Uint32 *)surface->pixels;
    pixels[(y * surface->w) + x] = pixel;
}

Uint8 grayscale(Uint32 pixel, SDL_PixelFormat *format) {
    Uint8 r, g, b;
    SDL_GetRGB(pixel, format, &r, &g, &b);
    return (Uint8)(0.3 * r + 0.59 * g + 0.11 * b);
}

void sobelEdgeDetection(SDL_Surface *src, SDL_Surface *dst) {
    int gx[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };
    int gy[3][3] = {
        {-1, -2, -1},
        {0, 0, 0},
        {1, 2, 1}
    };

    SDL_LockSurface(src);
    SDL_LockSurface(dst);

    for (int y = 1; y < src->h - 1; y++) {
        for (int x = 1; x < src->w - 1; x++) {
            int sumX = 0;
            int sumY = 0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int px = x + kx;
                    int py = y + ky;
                    Uint8 gray = grayscale(getPixel(src, px, py), src->format);
                    sumX += gray * gx[ky + 1][kx + 1];
                    sumY += gray * gy[ky + 1][kx + 1];
                }
            }

            int magnitude = sqrt(sumX * sumX + sumY * sumY);
            magnitude = magnitude > 255 ? 255 : magnitude;
            Uint32 edgeColor = SDL_MapRGB(dst->format, magnitude, magnitude, magnitude);
            setPixel(dst, x, y, edgeColor);
        }
    }

    SDL_UnlockSurface(src);
    SDL_UnlockSurface(dst);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <image.bmp>\n", argv[0]);
        return 1;
    }

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Edge Detection", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    //SDL_Surface *image = SDL_LoadBMP(argv[1]);
    SDL_Surface* img = IMG_Load(argv[1]);
    SDL_Surface* img_cvt = img;//SDL_ConvertSurface(img, SDL_PIXELFORMAT_RGB888, 0);
    SDL_Surface *edgeImage = SDL_CreateRGBSurfaceWithFormat(0, img_cvt->w, img_cvt->h, 32, SDL_PIXELFORMAT_RGBA32);

    sobelEdgeDetection(img_cvt, edgeImage);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, edgeImage);

    int quit = 0;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(edgeImage);
    SDL_FreeSurface(img_cvt);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}