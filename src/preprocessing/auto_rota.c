#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>        
#include <SDL2/SDL_image.h>   
#include <math.h>    
#include "preprocess.h"

#define M_PI 3.14159265358979323846    //Just in case    


SDL_Surface* manualrota(SDL_Surface *image, double angle) 
{
    double radians = angle * 3.141593 / 180.0;
    int wid = image->w, hei = image->h;

    int updateW = (int)(fabs(wid *cos(radians))+ fabs(hei* sin( radians)));
    int updateH = (int)(fabs(wid *sin(radians))+ fabs(hei* cos( radians)));

    SDL_Surface *res_image = SDL_CreateRGBSurface(0, updateW, updateH, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000 );

    if (!res_image) return NULL;

    int midX = wid / 2, midY = hei / 2;
    int res_midX = updateW / 2, res_midY = updateH / 2;

    Uint32 *pixels = (Uint32 *)image->pixels;
    Uint32 *res_pixels = (Uint32 *)res_image->pixels;

    for (int y_new = 0; y_new < updateH; y_new++) 
    {
        for (int x_new = 0; x_new < updateW; x_new++) 
        {
            int x_old = (int)((x_new - res_midX) * cos(radians) + (y_new - res_midY) * sin(radians)) + midX;
            int y_old = (int)(-(x_new - res_midX) * sin(radians) + (y_new - res_midY) * cos(radians)) + midY;

            res_pixels[y_new * updateW + x_new] = 
                (x_old >= 0 && x_old < wid && y_old >= 0 && y_old < hei) 
                ? pixels[y_old * wid + x_old]
                : SDL_MapRGB(res_image->format, 255, 255, 255);
        }
    }

    return res_image;
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