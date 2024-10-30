#include <SDL2/SDL.h>        // SDL core library for rendering and surfaces
#include <SDL2/SDL_image.h>   // SDL image library, if needed for loading non-BMP images
#include <math.h>    
#define M_PI 3.14159265358979323846         

SDL_Surface* loadImage(const char* given_path) 
{
    if (!given_path) 
    {
        fprintf(stderr, "Error NULL path provided\n");
        return NULL;
    }

    SDL_Surface* image = IMG_Load(given_path);
    
    if (!image) 
    {
    fprintf(stderr, "Error loading image '%s': %s\n",given_path, IMG_GetError()); 
    return NULL;
    }
    return image;
}
SDL_Surface* rotate_image(SDL_Surface *image, double angle) {
    double radians = angle * 3.141593 / 180.0;

    int width = image->w;
    int height = image->h;

    // Calculate the new dimensions for the image to be rotated
    int new_width = (int)(fabs(width * cos(radians)) + fabs(height * sin(radians)));
    int new_height = (int)(fabs(width * sin(radians)) + fabs(height * cos(radians)));

    SDL_Surface *rotated_image = SDL_CreateRGBSurface(
        0, new_width, new_height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000
    );

    if (!rotated_image) {
        printf("Failed to create rotated surface: %s\n", SDL_GetError());
        return NULL;
    }

    // Get the center of the coordinates of the new and the current image 
    int middle_x_old = width / 2;
    int middle_y_old = height / 2;
    int middle_x_new = new_width / 2;
    int middle_y_new = new_height / 2;

    Uint32 *original_pixels = (Uint32 *)image->pixels;
    Uint32 *rotated_pixels = (Uint32 *)rotated_image->pixels;

    // Iterate through all pixels of the new image
    for (int y_new = 0; y_new < new_height; y_new++) {
        for (int x_new = 0; x_new < new_width; x_new++) {
            // Calculate the current coordinates of the pixel
            int x_old = (int)((x_new - middle_x_new) * cos(radians) + (y_new - middle_y_new) * sin(radians)) + middle_x_old;
            int y_old = (int)(-(x_new - middle_x_new) * sin(radians) + (y_new - middle_y_new) * cos(radians)) + middle_y_old;

            // If the coordinates are within the bounds then it is put in the new image
            if (x_old >= 0 && x_old < width && y_old >= 0 && y_old < height) {
                rotated_pixels[y_new * new_width + x_new] = original_pixels[y_old * width + x_old];
            } else {
                // Set to transparent for out-of-bounds pixels
                rotated_pixels[y_new * new_width + x_new] = SDL_MapRGBA(rotated_image->format, 0, 0, 0, 0);
            }
        }
    }

    return rotated_image;
}


int main(int argc, char *argv[]) 
{
    if (argc != 3) {
        printf("Usage: %s <input_image.bmp> <angle>\n", argv[0]);
        return 1;
    }

    const char *input_file = argv[1];
    double angle = atof(argv[2]);

    // Load the image
    SDL_Surface *image = loadImage(input_file); // Ensure this function is defined or included
    if (!image) {
        printf("Failed to load image: %s\n", IMG_GetError());
        return 1;
    }

    // Rotate the image
    SDL_Surface *rotated_image = rotate_image(image, angle);
    if (!rotated_image) {
        SDL_FreeSurface(image);
        return 1;
    }

    // Save the rotated image or display it
    SDL_SaveBMP(rotated_image, "rotated_image.bmp");

    // Cleanup
    SDL_FreeSurface(image);
    SDL_FreeSurface(rotated_image);
    return 0;
}