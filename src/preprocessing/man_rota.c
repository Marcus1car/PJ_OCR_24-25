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



int man_rota_main(int argc, char *argv[]) 
{
    if (argc != 4) 
    {
        printf("Usage: %s <input_image.bmp> <angle> <output_image>\n", argv[0]);
        return 1;
    }
    const char *input = argv[1];
    double angle = atof(argv[2]);

    // Load the image
    SDL_Surface *image = loadImage(input); 
    if (!image) 
    {
        printf("Failed to load image: %s\n", IMG_GetError());
        return 1;
    }
    // Rotate the image
    SDL_Surface *res = manualrota(image, angle);
    if (!res) 
    {
        SDL_FreeSurface(image);
        return 1;
    }

    if (SDL_SaveBMP(res, argv[3]) != 0) 
        {fprintf(stderr, "Error saving image: %s\n", SDL_GetError());}
    else 
        {printf("Rotated image saved to %s\n", argv[3]);}

    
    // Cleanup
    SDL_FreeSurface(image);
    SDL_FreeSurface(res);
    return 0;
}


int main(int argc, char *argv[]) {
    return man_rota_main(argc, argv);
}