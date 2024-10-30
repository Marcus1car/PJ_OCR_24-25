#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_rotozoom.h>
#include <stdio.h>
#include <math.h> 
#include<stdlib.h>



//----------------------------------------------------------------
//Import function 
//----------------------------------------------------------------
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
// ----------------------------------------------------------------
//Contrast function  Y
//----------------------------------------------------------------
void enhanceContrast(SDL_Surface *surface, double noiseLevel) 
{
    Uint32 *pixels =(Uint32 *)surface->pixels;
    int width = surface->w;
    int height = surface->h;
    Uint8 minGray = 255, maxGray = 0;
    // Find minimum and maximum grayscale values in the image
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Uint32 pixel = pixels[y * width + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            if (r < minGray) minGray = r;
            if (r > maxGray) maxGray = r;
        }
    }
    // Define contrast strength based on noise level (higher noise level = stronger scaling)
    double contrastStrength = (noiseLevel >= 60.0) ? 1.5 : (noiseLevel >= 20.0) ? 1.2 : 1.0;
    Uint8 minScaled = (Uint8)(minGray * contrastStrength);
    Uint8 maxScaled = (Uint8)(maxGray * contrastStrength);
    for (int y = 0; y < height; ++y) 
    {
        for (int x = 0; x < width; ++x) 
        {
            Uint32 pixel = pixels[y * width + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            Uint8 stretched = (Uint8)(((r - minGray) * (maxScaled - minScaled)) / (maxGray - minGray) + minScaled);
            pixels[y * width + x] = SDL_MapRGB(surface->format, stretched, stretched, stretched);
        }
    }
}
//----------------------------------------------------------------
//Binarize functions
//----------------------------------------------------------------
Uint8 calculateMeanLight(SDL_Surface *surface) 
{
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int height = surface->h;
    int width = surface->w;
    unsigned long totalLight = 0;
    int totalPixels = height * width;
    for (int x = 0; x < totalPixels; x++) 
    {
        Uint8 r,g,b;
        SDL_GetRGB(pixels[x], surface->format, &r, &g, &b);
        totalLight = totalLight + r;
    }
    Uint8 res = (Uint8)(totalLight / totalPixels);
    return res;
}
void convertToBlackAndWhite(SDL_Surface *surface) {
    Uint8 threshold = calculateMeanLight(surface);
    printf("Binarize with %d threshold \n", threshold);
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int width = surface->w;
    int height = surface->h;

    for (int y = 0; y < height; ++y) 
    {
        for (int x = 0; x < width; ++x) 
        {
            Uint32 pixel = pixels[y * width + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            Uint8 bw = (r >= threshold) ? 255 : 0;
            pixels[y * width + x] = SDL_MapRGB(surface->format, bw, bw, bw);
        }
    }
}
//----------------------------------------------------------------
//Denoise functions 
//----------------------------------------------------------------
double calculateNoiseLevel(SDL_Surface *surface) 
{
    Uint32 *pixels = (Uint32 *)surface->pixels;
    double sum = 0.0;
    double sumsq = 0.0;
    int height = surface->h;
    int width = surface->w;
    int tot = width * height;
    for (int x = 0; x < tot; x++) 
    {
        Uint8 r,g,b;
        SDL_GetRGB(pixels[x], surface->format, &r, &g, &b);
        sumsq += r*r;
        sum += r;
    }
    double mean = sum/tot;
    double var = (sumsq/tot)-(mean*mean);
    double res = sqrt(var);
    return res;
}

void medianFilter(SDL_Surface *surface) 
{
    int height = surface->h;
    int width = surface->w;  
    Uint32 *pixels = (Uint32 *)surface->pixels;
    Uint32 *copy = (Uint32 *)malloc(width * height * sizeof(Uint32));
    memcpy(copy, pixels, width * height * sizeof(Uint32));
    for (int y = 1; y < height - 1; ++y) 
    {
        for (int x = 1; x < width - 1; ++x) 
        {
            Uint8 neighborhood[9];
            int index = 0;
            for (int dy = -1; dy <= 1; ++dy) 
            {
                for (int dx = -1; dx <= 1; ++dx)
                 {
                    Uint32 pixel = copy[(y + dy) * width + (x + dx)];
                    Uint8 r, g, b;
                    SDL_GetRGB(pixel, surface->format, &r, &g, &b);
                    neighborhood[index++] = r; // Assuming grayscale
                }
            }
            // Sort the neighborhood array and take the median
            for (int i = 0; i < 9; i++) 
            {
                for (int j = i + 1; j < 9; j++) 
                {
                    if (neighborhood[j] < neighborhood[i])
                     {
                        Uint8 temp = neighborhood[i];
                        neighborhood[i] = neighborhood[j];
                        neighborhood[j] = temp;
                    }
                }
            }
            Uint8 median = neighborhood[4];
            pixels[y * width + x] = SDL_MapRGB(surface->format, median, median, median);
        }
    }
    free(copy);
}
//----------------------------------------------------------------
//Greyscale functions 
//----------------------------------------------------------------
void convertToGrayscale(SDL_Surface *surface) 
{
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int height = surface->h;
    int width = surface->w;


    for (int y = 0; y < height; ++y) 
    {
        for (int x = 0; x < width; ++x) 
        {
            Uint32 pixel = pixels[y * width + x];
            Uint8 r,g,b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            Uint8 gray = (Uint8)(0.299 * r + 0.587 * g + 0.114 * b);
            pixels[y * width + x] = SDL_MapRGB(surface->format, gray, gray, gray);
        }
    }
}
//----------------------------------------------------------------
//Manual Rotation function 
//----------------------------------------------------------------
SDL_Surface * man_rotation (SDL_Surface * given_image , double angle) 
{
    if (!given_image) 
    {fprintf(stderr, "Error: NULL image provided\n");return NULL;}
    
    angle = fmod(angle, 360.0);
    if (angle < 0) angle += 360.0;
    if (angle == 0.0 || angle == 360.0) 
    {
        SDL_Surface* copy = SDL_ConvertSurface(given_image,given_image->format, 0);
        if (!copy) 
        {fprintf(stderr, "Error creating surface copy: %s\n",SDL_GetError());}
        return copy;
    }
    SDL_Surface* res = rotozoomSurface(given_image, angle, 1.0, 1);
    if (!res) 
    {fprintf(stderr, "Error rotating image: %s\n", SDL_GetError());return NULL; }
    
    return res;
}

//----------------------------------------------------------------
//Final functions 
//----------------------------------------------------------------

void adaptivePreprocessing(SDL_Surface *surface) {
    // Estimate noise
    double noiseLevel = calculateNoiseLevel(surface);
    printf("Noise level %f \n", noiseLevel);


    // Step 1: Convert to Grayscale
    convertToGrayscale(surface);

    // Step 2: Apply Noise-Sensitive Filters
    if (noiseLevel < 40) {
        // Low noise: Minimal filtering
        printf("Applying median filter.\n");
        medianFilter(surface);
    }

    // Step 3: Contrast Enhancement (with adaptive scaling)
    enhanceContrast(surface, noiseLevel);
    convertToBlackAndWhite(surface);
}


int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))) {
        printf("Error initializing SDL_image: %s\n", IMG_GetError());
        return 1;
    }

    // Load the image
    SDL_Surface *image = load_image(argv[1]);

    if (!image) {
        printf("Failed to load image: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    printf("%s \n", argv[1]);

    adaptivePreprocessing(image);
    
    printf("\n");

    if (SDL_LockSurface(image) < 0) {
        return 1;
    }

    SDL_SaveBMP(image, argv[2]);

    SDL_UnlockSurface(image);
    SDL_FreeSurface(image);
    SDL_Quit();

    return 0;
}
