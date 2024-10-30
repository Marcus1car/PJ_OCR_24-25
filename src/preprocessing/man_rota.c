#include <SDL2/SDL.h>        // SDL core library for rendering and surfaces
#include <SDL2/SDL_image.h>   // SDL image library, if needed for loading non-BMP images
#include <math.h>             

SDL_Surface* rotate_image(SDL_Surface *image, double angle) {
    double radians = angle * 3.141593 / 180.0;

    int width = image->w;
    int height = image->h;

    //Calculate the new dimensions for the image to be rotated
    int new_width = (int)(fabs(width * cos(radians)) + fabs(height * sin(radians)));
    int new_height = (int)(fabs(width * sin(radians)) + fabs(height * cos(radians)));

    SDL_Surface *rotated_image = SDL_CreateRGBSurface(
        0, new_width, new_height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000
        );

    if (!rotated_image) {
        printf("Failed to create rotated surface: %s\n", SDL_GetError());
        return NULL;
    }

    //Get the center of the coordinates of the new and the current image 
    int middle_x_old = width / 2;
    int middle_y_old = height / 2;
    int middle_x_new = new_width / 2;
    int middle_y_new = new_height / 2;

    Uint32 *original_pixels = (Uint32 *)image->pixels;
    Uint32 *rotated_pixels = (Uint32 *)rotated_image->pixels;

    // Iterate through all pixels of the new image
    for (int y_new = 0; y_new < new_height; y_new++) 
    {
        for (int x_new = 0; x_new < new_width; x_new++) 
        {
            //Calculate the current coordinates of the pixel
            int x_old = (int)((x_new - middle_x_new) * cos(radians) + (y_new - middle_y_new) * sin(radians)) + middle_x_old;
            int y_old = (int)(-(x_new - middle_x_new) * sin(radians) + (y_new - middle_y_new) * cos(radians)) + middle_y_old;

            //If the coordinates are within the bounds then it is put in the new image
            if (x_old >= 0 && x_old < width && y_old >= 0 && y_old < height) {
                rotated_pixels[y_new * new_width + x_new] = original_pixels[y_old * width + x_old];
            } else {
                rotated_pixels[y_new * new_width + x_new] = SDL_MapRGB(rotated_image->format, 255, 255, 255); 
            }
        }
    }

    return rotated_image;
}