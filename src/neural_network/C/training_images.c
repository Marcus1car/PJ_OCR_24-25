#include <SDL2/SDL_pixels.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "SDL2/SDL.h"
#include <err.h>
#include "neural.h"
#include <dirent.h>

// Constant macros to define constants
#define IMG_W 32
#define IMG_H 32

#define INPUT_LAYER_SIZE IMG_H *IMG_W
#define HIDDEN_LAYER_SIZE 128 // Arbitrary
#define OUTPUT_LAYER_SIZE 26

double* get_target(char* filename)
{
    double *res = calloc(26,sizeof(double));
    res[filename[0]-'a'] = 1;
    return res;
}

Uint8 calculate_otsu_threshold(SDL_Surface *surface)
{
    int width = surface->w;
    int height = surface->h;
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int histogram[256] = {0};
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Uint32 pixel = pixels[(y * width) + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            Uint8 gray = (Uint8)(0.299 * r + 0.587 * g + 0.114 * b);
            histogram[gray]++;
        }
    }

    int total_pixels = width * height;
    int sumB = 0;
    int wB = 0;
    float max_variance = 0;
    Uint8 threshold = 0;
    int sum1 = 0;

    for (int i = 0; i < 256; i++)
    {
        sum1 += i * histogram[i];
    }

    for (int i = 0; i < 256; i++)
    {
        wB += histogram[i];
        if (wB == 0)
            continue;

        int wF = total_pixels - wB;
        if (wF == 0)
            break;

        sumB += i * histogram[i];
        float mB = (float)sumB / wB;
        float mF = (float)(sum1 - sumB) / wF;

        // Calculate Between-Class Variance
        float variance = (float)wB * wF * (mB - mF) * (mB - mF);
        if (variance > max_variance)
        {
            max_variance = variance;
            threshold = i;
        }
    }

    return threshold;
}
void swap(double **a, double **b) {
    double *temp = *a;
    *a = *b;
    *b = temp;
}
void shuffle(double **array1,double **array2, size_t size) {
    srand(time(NULL));
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        swap(&array1[i], &array1[j]);
        swap(&array2[i], &array2[j]);
    }
}

double *load_image_bw(char *path)
{
    SDL_Surface *img = IMG_Load(path);

    if (SDL_MUSTLOCK(img))
    {
        SDL_LockSurface(img);
    }

    Uint32 *pixels = (Uint32 *)img->pixels;
    int width = img->w;
    int height = img->h;
    if (height != IMG_H || width != IMG_W)
        err(EXIT_FAILURE, "Img with path %s doesn't have the required dimensions.", path);

    Uint8 threshold = calculate_otsu_threshold(img);

    double *array = calloc(IMG_W * IMG_H, sizeof(double));
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Uint32 pixel = pixels[(y * width) + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, img->format, &r, &g, &b);
            Uint8 gray = (Uint8)(0.299 * r + 0.587 * g + 0.114 * b);
            Uint32 bw_pixel = gray > threshold ? SDL_MapRGB(img->format, 255, 255, 255)
                                               : SDL_MapRGB(img->format, 0, 0, 0);

            pixels[(y * width) + x] = bw_pixel;
            SDL_GetRGB(pixel, img->format, &r, &g, &b);
            array[y * IMG_W + x] = ((double)g) / 255;
        }
    }

    if (SDL_MUSTLOCK(img))
    {
        SDL_UnlockSurface(img);
    }
    return array;
}

int main(int argc, char **argv)
{
    if (argc != 3)
        errx(EXIT_FAILURE, "Usage: ./training <training_dataset_directory> <testing_dataset_directory>");

    Network *network = calloc(1, sizeof(Network));
    network_init(&network, INPUT_LAYER_SIZE, HIDDEN_LAYER_SIZE, OUTPUT_LAYER_SIZE);
    Trainer *trainer = calloc(1, sizeof(Trainer));
    trainer_init(&trainer, &network);

    DIR *training_directory = opendir(argv[1]);

    if (training_directory == NULL)
        err(EXIT_FAILURE, "Error while opening training directory");

    DIR *testing_directory = opendir(argv[2]);

    if (testing_directory == NULL)
        err(EXIT_FAILURE, "Error while opening testing directory");

    struct dirent *entry;
    size_t sample_training_size = 0;
    size_t sample_testing_size = 0;

    while ((entry = readdir(training_directory)) != NULL)
    {
        sample_training_size++;
    }

    while ((entry = readdir(testing_directory)) != NULL)
    {
        sample_testing_size++;
    }
    sample_testing_size -= 2;
    sample_training_size -= 2;

    char **training_img_path = calloc(sample_training_size, sizeof(char *));
    char **testing_img_path = calloc(sample_testing_size, sizeof(char *));

    size_t idx = 0;
    while ((entry = readdir(training_directory)) != NULL)
    {
        if(entry->d_name[0] == '.') continue;
        training_img_path[idx] = calloc(strlen(entry->d_name), sizeof(char));
        strcpy(training_img_path[idx++], entry->d_name);
    }

    idx = 0;
    while ((entry = readdir(testing_directory)) != NULL)
    {
        if(entry->d_name[0] == '.') continue;
        testing_img_path[idx] = calloc(strlen(entry->d_name), sizeof(char));
        strcpy(testing_img_path[idx++], entry->d_name);
    }

    closedir(testing_directory);
    closedir(training_directory);

    double **training_data = calloc(sample_training_size, sizeof(double*));
    double **testing_data = calloc(sample_testing_size, sizeof(double*));

    double **targeted_data = calloc(sample_testing_size, sizeof(double*));
    idx = 0;
    while (idx < sample_training_size) 
    {
        char* path1 = calloc(strlen(argv[1])+1+strlen(training_img_path[idx])+1, sizeof(char));
        char* path2 = calloc(strlen(argv[2])+1+strlen(testing_img_path[idx])+1, sizeof(char));
        strcpy(path1, argv[1]);
        strcat(path1, "/");
        strcat(path1, training_img_path[idx]);

        strcpy(path2, argv[2]);
        strcat(path2, "/");
        strcat(path2, testing_img_path[idx]);
        
        training_data[idx] = load_image_bw(path1);
        testing_data[idx] = load_image_bw(path2);
        targeted_data[idx] = get_target(path1);
        idx++;
    }
    shuffle(training_data,targeted_data,sample_testing_size);

    return EXIT_SUCCESS;
}

//TODO: 
// - Function that gets the char** of the training images, then adds its data to the double** and adds the expected result (i.e. the probability of the letter) to an other double **. target[i][proba_letter]
