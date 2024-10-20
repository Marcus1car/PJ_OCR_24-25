#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <dirent.h> // For reading directory files
#include <stdio.h>
/*
void render_and_save_letter(SDL_Renderer *renderer, TTF_Font *font, char letter, const char *output_path)
{
    // Create the text surface
    SDL_Color color = {0, 0, 0, 255}; // White text
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    char text[2] = {letter, '\0'};
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    if (!surface) {
        printf("TTF_RenderText_Solid error: %s\n", TTF_GetError());
        return;
    }

    // Create a texture from the surface
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    // Create a new renderer target
    SDL_SetRenderTarget(renderer, texture);

    // Save the surface as a .png file
    if (IMG_SavePNG(surface, output_path) != 0) {
        printf("Failed to save image: %s\n", IMG_GetError());
    } else {
        printf("Saved image: %s\n", output_path);
    }

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}*/
/*
void render_and_save_letter(SDL_Renderer *renderer, TTF_Font *font, char letter, const char *output_path)
{
    // Create the text surface with black text
    SDL_Color text_color = {0, 0, 0, 255}; // Black text
    char text[2] = {letter, '\0'};
    
    // Render the text to a surface
    SDL_Surface *text_surface = TTF_RenderText_Solid(font, text, text_color);
    if (!text_surface) {
        printf("TTF_RenderText_Solid error: %s\n", TTF_GetError());
        return;
    }

    // Create a 32x32 surface for final image
    SDL_Surface *image_surface = SDL_CreateRGBSurface(0, 32, 32, 32, 0, 0, 0, 0);
    if (!image_surface) {
        printf("SDL_CreateRGBSurface error: %s\n", SDL_GetError());
        SDL_FreeSurface(text_surface);
        return;
    }

    // Fill the background with white
    SDL_FillRect(image_surface, NULL, SDL_MapRGB(image_surface->format, 255, 255, 255)); // White background

    // Blit the text onto the 32x32 surface (center the text)
    SDL_Rect dstrect;
    dstrect.x = (32 - text_surface->w) / 2;
    dstrect.y = (32 - text_surface->h) / 2;
    dstrect.w = text_surface->w;
    dstrect.h = text_surface->h;
    
    SDL_BlitSurface(text_surface, NULL, image_surface, &dstrect);

    // Save the surface as a PNG file
    if (IMG_SavePNG(image_surface, output_path) != 0) {
        printf("Failed to save image: %s\n", IMG_GetError());
    } else {
        printf("Saved image: %s\n", output_path);
    }

    SDL_FreeSurface(text_surface);
    SDL_FreeSurface(image_surface);
}

void generate_dataset_from_fonts(const char *font_folder, const char *output_folder)
{
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(font_folder)) != NULL) {
        SDL_Init(SDL_INIT_VIDEO);
        TTF_Init();

        SDL_Window *window = SDL_CreateWindow("Font Rendering", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_HIDDEN);
        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        // Loop through all files in the folder
        while ((ent = readdir(dir)) != NULL) {
            char *ext = strrchr(ent->d_name, '.');
            if (ext && strcmp(ext, ".ttf") == 0) {
                char font_path[512];
                snprintf(font_path, sizeof(font_path), "%s/%s", font_folder, ent->d_name);

                // Load the font
                TTF_Font *font = TTF_OpenFont(font_path, 64); // 64 is font size
                if (!font) {
                    printf("Failed to load font: %s\n", TTF_GetError());
                    continue;
                }

                // Generate letters A-Z
                for (char letter = 'A'; letter <= 'Z'; ++letter) {
                    char output_path[512];
                    snprintf(output_path, sizeof(output_path), "%s/%c_%s.png", output_folder, letter-'A'+'a', ent->d_name);
                    render_and_save_letter(renderer, font, letter, output_path);
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

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Usage: %s <font_folder> <output_folder>\n", argv[0]);
        return 1;
    }

    const char *font_folder = argv[1];
    const char *output_folder = argv[2];

    generate_dataset_from_fonts(font_folder, output_folder);

    return 0;
}*/

//WORKS
/*
void render_and_save_letter(SDL_Renderer *renderer, TTF_Font *font, char letter, const char *output_path)
{
    // Create the text surface with black text
    SDL_Color text_color = {0, 0, 0, 255}; // Black text
    char text[2] = {letter, '\0'};
    
    // Render the text to a surface
    SDL_Surface *text_surface = TTF_RenderText_Solid(font, text, text_color);
    if (!text_surface) {
        printf("TTF_RenderText_Solid error: %s\n", TTF_GetError());
        return;
    }

    // Create a 32x32 surface for final image
    SDL_Surface *image_surface = SDL_CreateRGBSurface(0, 32, 32, 32, 0, 0, 0, 0);
    if (!image_surface) {
        printf("SDL_CreateRGBSurface error: %s\n", SDL_GetError());
        SDL_FreeSurface(text_surface);
        return;
    }

    // Fill the background with white
    SDL_FillRect(image_surface, NULL, SDL_MapRGB(image_surface->format, 255, 255, 255)); // White background

    // Blit the text onto the 32x32 surface (center the text)
    SDL_Rect dstrect;
    dstrect.x = (32 - text_surface->w) / 2;
    dstrect.y = (32 - text_surface->h) / 2;
    dstrect.w = text_surface->w;
    dstrect.h = text_surface->h;
    
    SDL_BlitSurface(text_surface, NULL, image_surface, &dstrect);

    // Save the surface as a PNG file
    if (IMG_SavePNG(image_surface, output_path) != 0) {
        printf("Failed to save image: %s\n", IMG_GetError());
    } else {
        printf("Saved image: %s\n", output_path);
    }

    SDL_FreeSurface(text_surface);
    SDL_FreeSurface(image_surface);
}

void generate_dataset_from_fonts(const char *font_folder, const char *output_folder)
{
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(font_folder)) != NULL) {
        SDL_Init(SDL_INIT_VIDEO);
        TTF_Init();

        SDL_Window *window = SDL_CreateWindow("Font Rendering", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_HIDDEN);
        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        // Loop through all files in the folder
        while ((ent = readdir(dir)) != NULL) {
            char *ext = strrchr(ent->d_name, '.');
            if (ext && strcmp(ext, ".ttf") == 0) {
                char font_path[512];
                snprintf(font_path, sizeof(font_path), "%s/%s", font_folder, ent->d_name);

                // Load the font
                TTF_Font *font = TTF_OpenFont(font_path, 30); // Adjust size to fit 32x32
                if (!font) {
                    printf("Failed to load font: %s\n", TTF_GetError());
                    continue;
                }

                // Generate letters A-Z
                for (char letter = 'A'; letter <= 'Z'; ++letter) {
                    char output_path[512];
                    snprintf(output_path, sizeof(output_path), "%s/%c_%s.png", output_folder, letter-'A' + 'a', ent->d_name);
                    render_and_save_letter(renderer, font, letter, output_path);
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

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Usage: %s <font_folder> <output_folder>\n", argv[0]);
        return 1;
    }

    const char *font_folder = argv[1];
    const char *output_folder = argv[2];

    generate_dataset_from_fonts(font_folder, output_folder);

    return 0;
}*/
/*
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <dirent.h>  // For reading directory files
#include <stdio.h>
#include <stdlib.h>  */// For random noise

// Function to add noise to the image surface
void add_noise(SDL_Surface *surface, int intensity)
{
    int num_pixels = (surface->w * surface->h * intensity) / 100; // Define number of noisy pixels based on intensity
    for (int i = 0; i < num_pixels; ++i) {
        int x = rand() % surface->w;
        int y = rand() % surface->h;
        Uint32 *pixels = (Uint32 *)surface->pixels;
        Uint32 color = SDL_MapRGB(surface->format, rand() % 256, rand() % 256, rand() % 256); // Random color
        pixels[y * surface->w + x] = color;
    }
}
void render_and_save_letter(SDL_Renderer *renderer, TTF_Font *font, char letter, const char *output_path, double angle, int noise_intensity)
{
    SDL_Color text_color = {0, 0, 0, 255};  // Black text
    char text[2] = {letter, '\0'};

    // Render the text to a surface
    SDL_Surface *text_surface = TTF_RenderText_Solid(font, text, text_color);
    if (!text_surface) {
        printf("TTF_RenderText_Solid error: %s\n", TTF_GetError());
        return;
    }

    // Create a texture from the text surface
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_FreeSurface(text_surface);  // Free the surface after creating the texture

    if (!text_texture) {
        printf("SDL_CreateTextureFromSurface error: %s\n", SDL_GetError());
        return;
    }

    // Get the width/height of the text texture
    int tex_w = 0, tex_h = 0;
    SDL_QueryTexture(text_texture, NULL, NULL, &tex_w, &tex_h);

    // Set destination rect for the text, centered in the 32x32 image
    SDL_Rect dstrect = {16 - tex_w / 2, 16 - tex_h / 2, tex_w, tex_h};

    // Clear the renderer (fill with white background)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // White background
    SDL_RenderClear(renderer);

    // Render with rotation
    SDL_RenderCopyEx(renderer, text_texture, NULL, &dstrect, angle, NULL, SDL_FLIP_NONE);

    // Get the renderer's pixel format and size
    Uint32 pixel_format;
    int renderer_w, renderer_h;
    SDL_RendererInfo renderer_info;
    SDL_GetRendererInfo(renderer, &renderer_info);
    pixel_format = renderer_info.texture_formats[0];  // Use the first supported format
    SDL_GetRendererOutputSize(renderer, &renderer_w, &renderer_h);

    // Ensure renderer_w and renderer_h are 32x32
    if (renderer_w != 32 || renderer_h != 32) {
        printf("Renderer output size mismatch! Expected 32x32, got %dx%d\n", renderer_w, renderer_h);
        SDL_DestroyTexture(text_texture);
        return;
    }

    // Create a surface matching the renderer's format
    SDL_Surface *image_surface = SDL_CreateRGBSurfaceWithFormat(0, 32, 32, 32, pixel_format);
    if (!image_surface) {
        printf("SDL_CreateRGBSurfaceWithFormat error: %s\n", SDL_GetError());
        SDL_DestroyTexture(text_texture);
        return;
    }

    // Read the pixels from the renderer into the surface
    if (SDL_RenderReadPixels(renderer, NULL, image_surface->format->format, image_surface->pixels, image_surface->pitch) != 0) {
        printf("SDL_RenderReadPixels error: %s\n", SDL_GetError());
        SDL_FreeSurface(image_surface);
        SDL_DestroyTexture(text_texture);
        return;
    }

    // Add noise to the image surface
    add_noise(image_surface, noise_intensity);

    // Save the final surface as a PNG file
    if (IMG_SavePNG(image_surface, output_path) != 0) {
        printf("Failed to save image: %s\n", IMG_GetError());
    } else {
        printf("Saved image: %s\n", output_path);
    }

    SDL_FreeSurface(image_surface);
    SDL_DestroyTexture(text_texture);
}
/*
void render_and_save_letter(SDL_Renderer *renderer, TTF_Font *font, char letter, const char *output_path, double angle, int noise_intensity)
{
    SDL_Color text_color = {0, 0, 0, 255};  // Black text
    char text[2] = {letter, '\0'};

    // Render the text to a surface
    SDL_Surface *text_surface = TTF_RenderText_Solid(font, text, text_color);
    if (!text_surface) {
        printf("TTF_RenderText_Solid error: %s\n", TTF_GetError());
        return;
    }

    // Create a 32x32 surface for the final image
    SDL_Surface *image_surface = SDL_CreateRGBSurface(0, 32, 32, 32, 0, 0, 0, 0);
    if (!image_surface) {
        printf("SDL_CreateRGBSurface error: %s\n", SDL_GetError());
        SDL_FreeSurface(text_surface);
        return;
    }

    // Fill the background with white
    SDL_FillRect(image_surface, NULL, SDL_MapRGB(image_surface->format, 255, 255, 255));  // White background

    // Convert the text surface into a texture for rotation
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_FreeSurface(text_surface);  // Free the surface after creating the texture

    // Clear the renderer (fill with white background)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // Get the width/height of the text texture
    int tex_w = 0;
    int tex_h = 0;
    SDL_QueryTexture(text_texture, NULL, NULL, &tex_w, &tex_h);

    // Set destination rect for the text, centered in the 32x32 image
    SDL_Rect dstrect = {16 - tex_w / 2, 16 - tex_h / 2, tex_w, tex_h};

    // Render with rotation
    SDL_RenderCopyEx(renderer, text_texture, NULL, &dstrect, angle, NULL, SDL_FLIP_NONE);

    // Render to the 32x32 image surface
    SDL_RenderReadPixels(renderer, NULL, image_surface->format->format, image_surface->pixels, image_surface->pitch);

    // Add noise
    add_noise(image_surface, noise_intensity);

    // Save the final surface as a PNG file
    if (IMG_SavePNG(image_surface, output_path) != 0) {
        printf("Failed to save image: %s\n", IMG_GetError());
    } else {
        printf("Saved image: %s\n", output_path);
    }

    SDL_FreeSurface(image_surface);
    SDL_DestroyTexture(text_texture);
}*/

void generate_dataset_from_fonts(const char *font_folder, const char *output_folder, int noise_intensity)
{
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(font_folder)) != NULL) {
        SDL_Init(SDL_INIT_VIDEO);
        TTF_Init();

        SDL_Window *window = SDL_CreateWindow("Font Rendering", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 32, 32, SDL_WINDOW_HIDDEN);
        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        // Loop through all files in the folder
        while ((ent = readdir(dir)) != NULL) {
            char *ext = strrchr(ent->d_name, '.');
            if (ext && strcmp(ext, ".ttf") == 0) {
                char font_path[512];
                snprintf(font_path, sizeof(font_path), "%s/%s", font_folder, ent->d_name);

                // Load the font
                TTF_Font *font = TTF_OpenFont(font_path, 32);  // Adjust size if needed
                if (!font) {
                    printf("Failed to load font: %s\n", TTF_GetError());
                    continue;
                }

                // Generate letters A-Z with rotation and noise variations
                for (char letter = 'A'; letter <= 'Z'; ++letter) {
                    for (size_t k = 0; k < 5; k++) {  // Rotations from -20 to 20 degrees
                        char output_path[512];
                        int random_number = (rand() % 61) - 30;
                        snprintf(output_path, sizeof(output_path), "%s/%c_%d_%s.png", output_folder, letter-'A'+'a', random_number, ent->d_name);
                        render_and_save_letter(renderer, font, letter, output_path, (double)random_number, noise_intensity);
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

int main(int argc, char *argv[])
{
    if (argc != 4) {
        printf("Usage: %s <font_folder> <output_folder> <noise_intensity>\n", argv[0]);
        return 1;
    }

    const char *font_folder = argv[1];
    const char *output_folder = argv[2];
    int noise_intensity = atoi(argv[3]);  // Noise intensity as a percentage

    generate_dataset_from_fonts(font_folder, output_folder, noise_intensity);

    return 0;
}