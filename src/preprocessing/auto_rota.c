#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>        
#include <SDL2/SDL_image.h>   
#include <math.h>    
#include "preprocess.h"

#define M_PI 3.14159265358979323846    //Just in case    

SDL_Surface* manualrota(SDL_Surface *image, double angle) {
    double radians = angle * 3.141593 / 180.0;

    int width = image->w;
    int height = image->h;

    // Calculate the new dimensions for the image to be rotated
    int new_width = (int)(fabs(width * cos(radians)) + fabs(height * sin(radians)));
    int new_height = (int)(fabs(width * sin(radians)) + fabs(height * cos(radians)));

    SDL_Surface *res = SDL_CreateRGBSurface(
        0, new_width, new_height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000
    );

    if (!res) {
        printf("Failed to create rotated surface: %s\n", SDL_GetError());
        return NULL;
    }

    // Get the center of the coordinates of the new and the current image 
    int middle_x_old = width / 2;
    int middle_y_old = height / 2;
    int middle_x_new = new_width / 2;
    int middle_y_new = new_height / 2;

    Uint32 *original_pixels = (Uint32 *)image->pixels;
    Uint32 *rotated_pixels = (Uint32 *)res->pixels;

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
                rotated_pixels[y_new * new_width + x_new] = SDL_MapRGBA(res->format, 0, 0, 0, 0);
            }
        }
    }

    return res;
}




double Houghangle(SDL_Surface *surface) 
{
    int width = surface->w;
    int height = surface->h;


    int diag = (int)sqrt(width * width + height * height); // Diagonal length
    int numAngles = 180; 

    // Create angle accumulator
    int *angleVotes = calloc(numAngles, sizeof(int));
    if (!angleVotes) 
    {
        fprintf(stderr, "Failed to allocate memory for angle votes.\n");
        return 0.0;
    }

    Uint32 *pixels = (Uint32 *)surface->pixels;




    // Iterate through each pixel to find "white" pixels (edges)
    for (int y = 0; y < height; y++) 
    {
        for (int x = 0; x < width; x++) 
        {
            Uint8 r, g, b;
            SDL_GetRGB(pixels[y * width + x], surface->format, &r, &g, &b);

            // only white /black
            if (r > 200 && g > 200 && b > 200) 
            {
                for (int t = 0; t < numAngles; t++) 
                {
                    double theta = (t - 90) * M_PI / 180.0; // Convert angle to radians
                    int rho = (int)(x * cos(theta) + y * sin(theta));
                    if (rho >= 0 && rho < diag) 
                        {angleVotes[t]++;}
                }
            }
        }
    }

    // Find the angle with the highest votes
    int maxVotes = 0;
    double dominantAngle = 0.0;
    for (int t = 0; t < numAngles; t++) {
        if (angleVotes[t] > maxVotes) {
            maxVotes = angleVotes[t];
            dominantAngle = (t - 90); // Convert back to degrees
        }
    }

    free(angleVotes);
    return dominantAngle;
}




// Main function
int main(int argc, char *argv[]) 
{
    if (argc != 3) 
    {
        fprintf(stderr, "Usage: %s <input_image> <output_image>\n", argv[0]);
        return 1;
    }

    // Initialize SDL and SDL_image
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))) {
        fprintf(stderr, "IMG_Init Error: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    // Load the input image
    SDL_Surface *image = loadImage(argv[1]);
    if (!image) {
        fprintf(stderr, "Failed to load image: %s\n", argv[1]);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    printf("Starting auto-rotation detection...\n");
    double dominantAngle = Houghangle(image);
    printf("Detected skew angle: %.2f degrees\n", dominantAngle);

    // Preprocess the image to create a clean binary image
    printf("Preprocessing the image...\n");
    FinalFunc(image); // Apply preprocessing (from preprocess.c)

    // Perform auto-rotation
    SDL_Surface *rotatedImage = manualrota(image, -dominantAngle);
    if (!rotatedImage) 
    {
        fprintf(stderr, "Auto-rotation failed.\n");
        SDL_FreeSurface(image);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    printf("Image rotated by %.2f degrees to correct skew.\n", dominantAngle);

    // Save the rotated image
    if (SDL_SaveBMP(rotatedImage, argv[2]) != 0) {
        fprintf(stderr, "Error saving rotated image: %s\n", SDL_GetError());
    } else {
        printf("Rotated image saved to %s.\n", argv[2]);
    }

    // Cleanup
    SDL_FreeSurface(image);
    if (rotatedImage != image) {
        SDL_FreeSurface(rotatedImage);
    }
    IMG_Quit();
    SDL_Quit();

    return 0;
}   