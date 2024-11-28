#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

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



//----------------------------------------------------------------
//Denoise  function 
//----------------------------------------------------------------
double noiselevel_weighted(SDL_Surface *surface) 
{
    Uint32 *pixels = (Uint32 *)surface->pixels;
    double sumsq = 0.0;
    double sum = 0.0;
    int hei = surface->h;
    int wid = surface->w;
    int totpix = wid * hei;
 

    for (int i = 0; i < totpix; i++) 
    {
        Uint8 r, g, b;
        SDL_GetRGB(pixels[i], surface->format, &r, &g, &b);
        double weightedValue = 0.3 *r  + 0.59*g+0.11  * b; 
        sum +=weightedValue;
        sumsq +=(weightedValue * weightedValue);
    }
    double mean = sum/totpix;
    double var = (sumsq/totpix)-(mean*mean);
    return sqrt(var);
}

// Helper function to sort 
void sortFilter(Uint8 close[], int size) 
{
    for (int i = 0; i < size; i++) {
        for (int j = i + 1; j < size; j++) 
        {
            if (close[j] <close[i]) 
            {
                // Swap elements if they are out of order
                Uint8 tmp = close[i];
                close[i] = close[j];
                close[j] = tmp;
            }
        }
    }
}

// Main  filter function
void Filterfunc(SDL_Surface *surface)
{
    int wid = surface->w;
    int hei = surface->h;
    Uint32 *clone = (Uint32 *)malloc(wid * hei * sizeof(Uint32));// Create a clone of the original pixels 
    Uint32 *pixels = (Uint32 *)surface->pixels;
    memcpy(clone, pixels, wid * hei * sizeof(Uint32));

    // Iterate through each pixel, no border
    for (int y = 1; y < hei - 1; ++y) 
    {
        for (int x = 1; x < wid - 1; ++x) 
        {
            int ind = 0;
            Uint8 close[9];
            // 3*3
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    Uint32 pixel = clone[(y + dy) * wid + (x + dx)];
                    Uint8 r, g, b;
                    SDL_GetRGB(pixel, surface->format, &r, &g, &b);
                    close[ind++] = r; 
                }
            }
            sortFilter(close, 9);
            // Take the average value 
            Uint8 avr = close[4];
            pixels[(wid*y) + x] = SDL_MapRGB(surface->format, avr, avr, avr);
        }
    }
    free(clone);
}
// ----------------------------------------------------------------
//Grayscale function 
//----------------------------------------------------------------
void Grayscalefunct(SDL_Surface *surface) 
{
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int wid = surface->w;
    int hei = surface->h;
    for (int i = 0; i <(wid*hei); ++i) 
    {
        Uint8 r, g, b;
        SDL_GetRGB(pixels[i], surface->format, &r, &g, &b);
        Uint8 graypix = (Uint8)(0.299*r+0.587*g+0.114*b);
        pixels[i] = SDL_MapRGB(surface->format, graypix, graypix, graypix);
    }
}

// ----------------------------------------------------------------
//Binarize function 
//----------------------------------------------------------------




Uint8 meanLight(SDL_Surface *surface) 
{
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int totpix = surface->w * surface->h;
    unsigned long totli = 0;
    Uint32 *pixelEnd = pixels + totpix;
    while (pixels < pixelEnd)
    {
        Uint8 r, g, b;
        SDL_GetRGB(*pixels++, surface->format, &r, &g, &b);
        totli += r;
    }
    return (Uint8)(totli/totpix);
}


Uint8 medianLight(SDL_Surface *surface) {
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int totpix = surface->w * surface->h;
    
    unsigned long pixelval[256] =  {0};
    unsigned long median =  0;
    unsigned long c = 0;

    // First pass
    Uint32 *pixelPtr = pixels;
    Uint32 *pixelEnd = pixels + totpix;
    while (pixelPtr< pixelEnd) 
    {
        Uint8 r, g, b;
        SDL_GetRGB(*pixelPtr++, surface->format, &r, &g, &b);
        pixelval[r] = 1;
    }

    // Second pass
    for (int intensity = 0; intensity < 256; intensity++) 
    {
        if (pixelval[intensity]) 
        {
            median += intensity;
            c++;
        }
    }

    return (Uint8)(median/c);
}


/* TOO THICK , OVERLAPS

Uint8 calculateMedianLight(SDL_Surface *surface)
{
    histogram
}


*/
void binarize(SDL_Surface *surface) 
{
   
    Uint8 meanCap = meanLight(surface);
    Uint8 medianCap = medianLight(surface);
    // Use the average mean & median 
    Uint8 cap = (meanCap + medianCap) /  2;
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int totpix = surface->w * surface->h;
    
    for (int i = 0; i < totpix; i++)
    {
        Uint8 r, g, b;
        SDL_GetRGB(pixels[i], surface->format, &r, &g, &b);
        
        // bitwise operation
        Uint8 bw = -(r >= cap);  // 0x00 or 0xFF
        pixels[i] = SDL_MapRGB(surface->format, bw, bw, bw);
    }
}


/*
void binarize(SDL_Surface *surface) 
{
    Uint8 cap = varlight(surface);
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int totpix = surface->w * surface->h;
    for (int i = 0; i < totpix; i++)
    {
        Uint8 r, g, b;
        SDL_GetRGB(pixels[i], surface->format, &r, &g, &b);
        
        // bitwise operation
        Uint8 bw = -(r >= cap);  //  0x00 or 0xFF
        pixels[i] = SDL_MapRGB(surface->format, bw, bw, bw);
    }
}
*/






// ----------------------------------------------------------------
//Contrast function 
//----------------------------------------------------------------


void more_contrast(SDL_Surface *surface, double noiseLevel) 
{
    Uint32 *pixels = (Uint32 *)surface->pixels;
    Uint8 minGray = 255;
    Uint8 maxGray = 0;
    int wid = surface->w;
    int hei = surface->h;
    
    // Find min & max
    for (int i = 0; i < wid * hei; i++) 
    {
        Uint8 r, g, b;
        SDL_GetRGB(pixels[i], surface->format, &r, &g, &b);
        minGray = (r < minGray) ? r : minGray;
        maxGray = (r > maxGray) ? r : maxGray;
    }
    
    double contrastStrength = (noiseLevel >= 50.0) ? 1.5 : 
                               (noiseLevel >= 30.0) ? 1.2 : 1.0;
    Uint8 minScaled = (Uint8)(minGray * contrastStrength);
    Uint8 maxScaled = (Uint8)(maxGray * contrastStrength);
    
    // Create lookup table 
    Uint8 contrastLUT[256];
    for (int i = 0; i < 256; i++) 
    {
        contrastLUT[i] = (Uint8)(
            ((i - minGray) * (maxScaled - minScaled)) / 
            (maxGray - minGray) + minScaled);
    }
    
    // Apply using lookup table
    for (int i = 0; i < wid * hei; i++) 
    {
        Uint8 r, g, b;
        SDL_GetRGB(pixels[i], surface->format, &r, &g, &b);
        
        Uint8 stretched = contrastLUT[r];
        pixels[i] = SDL_MapRGB(surface->format, stretched, stretched, stretched);
    }
}







// ----------------------------------------------------------------
//Final function 
//----------------------------------------------------------------


void FinalFunc(SDL_Surface *surface) 
{
    double noiseLevel = noiselevel_weighted(surface);
    Grayscalefunct(surface);
    if (noiseLevel < 35) 
    {
        Filterfunc(surface);
        printf("Flter applied.\n");
    }
    more_contrast(surface, noiseLevel);
    binarize(surface);
}

int main(int argc, char *argv[]) 
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

    printf("Processing image: %s\n", argv[1]);
    FinalFunc(image);

    if (SDL_SaveBMP(image, argv[2]) != 0) 
    {
        fprintf(stderr, "Error saving image: %s\n", SDL_GetError());
    } else 
    {
        printf("Processed image saved to %s\n", argv[2]);
    }

    SDL_FreeSurface(image);
    IMG_Quit();
    SDL_Quit();
    return 0;
}