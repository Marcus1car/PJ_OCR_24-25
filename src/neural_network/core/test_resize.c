#include <SDL2/SDL.h>

// Function to resize and pad a surface
SDL_Surface* resize_and_pad_surface(SDL_Surface* original, int IMG_W, int IMG_H) {
    if (!original) {
        SDL_Log("Original surface is NULL!");
        return NULL;
    }

    // Create a new surface with the target size
    SDL_Surface* resized = SDL_CreateRGBSurface(0, IMG_W, IMG_H, 32,
                                                0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (!resized) {
        SDL_Log("Failed to create target surface: %s", SDL_GetError());
        return NULL;
    }

    // Fill the new surface with white (RGBA: 255, 255, 255, 255)
    SDL_FillRect(resized, NULL, SDL_MapRGBA(resized->format, 255, 255, 255, 255));

    // Calculate scaling factors
    float scale_w = (float)IMG_W / original->w;
    float scale_h = (float)IMG_H / original->h;
    float scale = scale_w < scale_h ? scale_w : scale_h;

    // Calculate the dimensions of the scaled surface
    int scaled_w = (int)(original->w * scale);
    int scaled_h = (int)(original->h * scale);

    // Create a scaled copy of the original surface
    SDL_Surface* scaled = SDL_CreateRGBSurface(0, scaled_w, scaled_h, 32,
                                               0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (!scaled) {
        SDL_Log("Failed to create scaled surface: %s", SDL_GetError());
        SDL_FreeSurface(resized);
        return NULL;
    }

    SDL_Rect src_rect = { 0, 0, original->w, original->h };
    SDL_Rect dst_rect = { 0, 0, scaled_w, scaled_h };

    // Perform the scaling using SDL_BlitScaled
    if (SDL_BlitScaled(original, &src_rect, scaled, &dst_rect) < 0) {
        SDL_Log("Scaling failed: %s", SDL_GetError());
        SDL_FreeSurface(resized);
        SDL_FreeSurface(scaled);
        return NULL;
    }

    // Center the scaled surface onto the resized surface
    dst_rect.x = (IMG_W - scaled_w) / 2;
    dst_rect.y = (IMG_H - scaled_h) / 2;
    dst_rect.w = scaled_w;
    dst_rect.h = scaled_h;

    // Blit the scaled surface onto the resized surface
    if (SDL_BlitSurface(scaled, NULL, resized, &dst_rect) < 0) {
        SDL_Log("Blitting scaled surface failed: %s", SDL_GetError());
        SDL_FreeSurface(resized);
        SDL_FreeSurface(scaled);
        return NULL;
    }

    // Free the intermediate scaled surface
    SDL_FreeSurface(scaled);

    return resized;
}

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return -1;
    }

    // Load an image into an SDL_Surface
    SDL_Surface* original = SDL_LoadBMP(argv[1]);
    if (!original) {
        SDL_Log("Failed to load image: %s", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // Resize and pad the surface
    int IMG_W = 32, IMG_H = 32;
    SDL_Surface* resized = resize_and_pad_surface(original, IMG_W, IMG_H);
    if (!resized) {
        SDL_Log("Failed to resize and pad surface.");
        SDL_FreeSurface(original);
        SDL_Quit();
        return -1;
    }

    // Save the new surface to a file
    SDL_SaveBMP(resized, "resized.bmp");

    // Clean up
    SDL_FreeSurface(original);
    SDL_FreeSurface(resized);
    SDL_Quit();

    return 0;
}
