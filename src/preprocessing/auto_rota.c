#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>        
#include <SDL2/SDL_image.h>   
#include <math.h>    
#define M_PI 3.14159265358979323846    //Just in case      
 
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


int isWhite(Uint8 r, Uint8 g, Uint8 b) // Adjust cap
{ return (r > 200 && g > 200 && b > 200);}

double estimateRotationAngle(SDL_Surface *surface) 
{
    int width = surface->w;
    int height = surface->h;

    int maxTheta = 180; // Sampling angles from 0 to 179 degrees
    int* accumulator = calloc(maxTheta, sizeof(int)); // Hough-like accumulator
    if (!accumulator) 
    {fprintf(stderr, "Failed to allocate memory for accumulator.\n");return 0.0;}


    Uint8* pixels = (Uint8*)surface->pixels;
        for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint8 r, g, b;
            SDL_GetRGB(pixels[y * width + x], surface->format, &r, &g, &b);
            if (isWhite(r, g, b)) {
                // Accumulate votes for different angles based on edge orientation
                for (int theta = 0; theta < maxTheta; theta++) {
                    double radians = theta * M_PI / 180.0;
                    int rho = (int)(x * cos(radians) + y * sin(radians));
                    if (rho >= 0 && rho < maxTheta) {
                        accumulator[theta]++;
                    }
                }
            }
        }
    }

    // Find the angle with the maximum votes
    int bestTheta = 0;
    int maxVotes = 0;
    for (int theta = 0; theta < maxTheta; theta++) {
        if (accumulator[theta] > maxVotes) {
            maxVotes = accumulator[theta];
            bestTheta = theta;
        }
    }

    free(accumulator);
    return (double)bestTheta;

}

int main(int argc, char *argv[]) {
    if (argc != 3)
    {
        printf("Usage: %s <input_image.bmp> <output_image.bmp>\n", argv[0]);
        return 1;
    }

    const char *input = argv[1];
    const char *output = argv[2];

    SDL_Surface *image = loadImage(input);
    if (!image) 
    {
        fprintf(stderr, "Failed to load image: %s\n", IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Estimate rotation angle
    double angle = estimateRotationAngle(image);
    printf("Estimated rotation angle: %.2f degrees\n", angle);

    // Rotate the image
    SDL_Surface *res = manualrota(image, angle);
    if (!res) {
        fprintf(stderr, "Failed to rotate image.\n");
        SDL_FreeSurface(image);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Save the rotated image
    if (SDL_SaveBMP(res, output) != 0)
    {fprintf(stderr, "Failed to save rotated image: %s\n", SDL_GetError());}
    else 
    {printf("Rotated image saved to %s\n", output);}

    SDL_FreeSurface(image);
    SDL_FreeSurface(res);
    IMG_Quit();
    SDL_Quit();
    return 0;
}