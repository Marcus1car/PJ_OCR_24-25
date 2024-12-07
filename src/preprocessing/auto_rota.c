#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>        
#include <SDL2/SDL_image.h>   
#include <math.h>    
#include <err.h>
#include "preprocess_utils.h"

#define M_PI 3.14159265358979323846    //Just in case    


//Sobel matrices 

int Gx[3][3] = 
{
    {1, 0, -1},
    {2, 0, -2},
    {1, 0, -1}
};

int Gy[3][3] = 
{
    { 1, 2, 1},
    { 0, 0, 0},
    {-1,-2,-1}
};




SDL_Surface* manualrota(SDL_Surface *image, double angle) 
{
    double radians = angle * 3.141593 / 180.0; // convert to radians
    int wid = image->w, hei = image->h;

    int updateW = (int)(fabs(wid *cos(radians))+ fabs(hei* sin( radians)));
    int updateH = (int)(fabs(wid *sin(radians))+ fabs(hei* cos( radians)));

    SDL_Surface *res_image = SDL_CreateRGBSurface(0, updateW, updateH, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000 );

    if (!res_image) return NULL;

    int midX = wid / 2, midY = hei / 2;
    int res_midX = updateW / 2, res_midY = updateH / 2;

    Uint32 *pixels = (Uint32 *)image->pixels; // Original image center
    Uint32 *res_pixels = (Uint32 *)res_image->pixels;// Rotated image center

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

//Edge detection
SDL_Surface* Sobel_Matrix(SDL_Surface* image) 
{
    int width = image->w;
    int height = image->h;
    
    // Lock surface for safe pixel access
    if (SDL_LockSurface(image) < 0) 
        {fprintf(stderr, "Surface lock failed - %s\n", SDL_GetError()); return NULL;}

       SDL_Surface* edges = SDL_CreateRGBSurface(
        0,                      
        width, 
        height, 
        32,                     
        0xFF000000,             
        0x00FF0000,             
        0x0000FF00,             
        0x000000FF              
    );
    

    if (!edges) 
    {
        SDL_UnlockSurface(image);
        fprintf(stderr, "Error creating edge surface: %s\n", SDL_GetError());
        return NULL;
    }
    
    SDL_LockSurface(edges);

    for (int y = 1; y < height - 1; y++) 
    {
        for (int x = 1; x < width - 1; x++) 
        {
            float gradientAccumX = 0, gradientAccumY = 0;
            for (int kernelY = -1; kernelY <= 1; kernelY++) 
            {
                for (int kernelX = -1; kernelX <= 1; kernelX++) 
                {
                    Uint8 red, green, blue;
                    SDL_GetRGB(
                        *((Uint32*)image->pixels +(y+ kernelY) *width + (x +kernelX)), 
                        image->format, &red, &green, &blue
                    );
                    float intensity = (float)blue; 
                    gradientAccumX += intensity * Gx[kernelY+ 1][kernelX +1];
                    gradientAccumY += intensity * Gy[kernelY+ 1][kernelX +1];
                }
            }



            float gradientMagnitude = sqrtf(gradientAccumX * gradientAccumX + gradientAccumY * gradientAccumY);
            gradientMagnitude = fminf(gradientMagnitude, 255.0f);



            Uint8 edgeIntensity = (Uint8)gradientMagnitude;
            *((Uint32*)edges->pixels + y * width + x) = SDL_MapRGB(edges->format, edgeIntensity, edgeIntensity, edgeIntensity);
        }
    }

    SDL_UnlockSurface(edges);
    SDL_UnlockSurface(image);
    return edges;
}


void Hough_Funtion(SDL_Surface* edges, float* voteMatrix, int maxRadius, int width, int height)
 {
    
    memset(voteMatrix, 0, (2 * maxRadius * 180) * sizeof(float));

    for (int y = 0; y < height; y++)
     {
        for (int x = 0; x < width; x++) 
        {
            Uint8 r, g, b;
            SDL_GetRGB(
                *((Uint32*)edges->pixels + y * width + x), 
                edges->format, &r, &g, &b
            );

            //Verify pixel intensity for edge
            if (b > 0) 
            {
                for (int k = 0; k < 180; k++) 
                {
                    // Map theta into valid range
                     double theta = (k - 90) * (M_PI / 180.0);//convert to
                    float radius = x * cos(theta) + y * sin(theta);
                    // Map radius to maxRadius into radiusIndex
                    int radiusIndex = (int)radius + maxRadius;
                    voteMatrix[radiusIndex * 180 + k] ++;
                }
            }
        }
    }
}





double Dominant_Angle(float* voteMatrix, int maxRadius, int width, int height, int cap) 
{
    int diag = maxRadius;  
    int lineCount = 0;  
    double angleSum = 0;  

    for (int radIndex = 0; radIndex < 2 * diag; radIndex++) 
    {  
        for (int thetaIndex = 0; thetaIndex < 180; thetaIndex++) 
        {  
            if (voteMatrix[radIndex * 180 + thetaIndex] > cap) 
            {
                double angle = (thetaIndex-(90)) * (M_PI/180.0);  
                angleSum += angle;
                lineCount++;
            }
        }
    }
    //No lines found
    if (lineCount == 0) { return 0.0; } 

    double averageAngle = angleSum / lineCount;
    double DomAngle = averageAngle*180.0 / M_PI;

    if (DomAngle>90.0) {DomAngle-=180.0;}// Normalize angle within range
    return DomAngle;
}






int main(int argc, char* argv[]) 
{
    if (argc != 3) 
    {
        fprintf(stderr, "Usage: %s <input_image> <output_image>\n", argv[0]);
        return 1;
    }
  
    if (SDL_Init(SDL_INIT_VIDEO) != 0) 
    {
        fprintf(stderr, "Error from SDL: %s\n", SDL_GetError());
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))) 
    {
        fprintf(stderr, "Error from SDL_image: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

 
    SDL_Surface *image = loadImage(argv[1]);
    if (!image) 
    {
        fprintf(stderr, "Failed to load image: %s\n", IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    FinalFunc(image);
    SDL_Surface* edges = Sobel_Matrix(image);
    if (!edges) 
    {
        printf("Sobel Matrix Edge detection failed!\n");
        SDL_FreeSurface(image);
        SDL_Quit();
        return 1;
    }

 
    int maxR = sqrt(image->w * image->w + image->h * image->h);
    float* voteMatrix = malloc(180 * (2 * maxR) * sizeof(float));
 
    Hough_Funtion(edges, voteMatrix, maxR, edges->w, edges->h);
    double DomAngle = Dominant_Angle(voteMatrix, maxR, edges->w, edges->h, 0);



    if (!(-2 < DomAngle && DomAngle < 2)) 
    {
        printf("Rotation Detected: %f degrees\n", floor(DomAngle*(-9)));
        image = manualrota(image, floor(DomAngle*(-9)));
    }

    if (SDL_SaveBMP(image, argv[2]) != 0) 
    {
        fprintf(stderr, "Error saving image: %s\n", SDL_GetError());
    } 
    else 
    {
        printf("Processed image saved to %s\n", argv[2]);
    }

    SDL_FreeSurface(edges);
    SDL_FreeSurface(image);
    IMG_Quit();
    SDL_Quit();
    return 0;
}

