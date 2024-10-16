#include <SDL2/SDL_pixels.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "SDL2/SDL.h"
#include <err.h>
#include "neural.h"
#include <dirent.h>
#include <time.h>
#include <SDL2/SDL_image.h>

// Constant macros to define constants
#define IMG_W 32
#define IMG_H 32

#define INPUT_LAYER_SIZE IMG_H *IMG_W
#define HIDDEN_LAYER_SIZE 256 // Arbitrary
#define OUTPUT_LAYER_SIZE 26

double* get_target(char* filename)
{
    double *res = calloc(26,sizeof(double));
    res[filename[0]-'a'] = 1;
    return res;
}

Uint8 calculate_otsu_threshold(SDL_Surface *surface)
{
    //printf("jaajruresaslol 1 jaaj %s\n",surface->format);

    int width = surface->w;
    int height = surface->h;
    //printf("jaajruresaslol 2\n");

    Uint32 *pixels = (Uint32 *)surface->pixels;
    //printf("jaajruresaslol 3\n");

    int histogram[256] = {0};
    //printf("jaajruresaslol 4\n");

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
   // printf("jaajruresaslol64 %d %d\n", x, y);

            Uint32 pixel = pixels[(y * width) + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            //printf("color= %d %d %d\n",r, g, b);

            Uint8 gray = (Uint8)(0.299 * r + 0.587 * g + 0.114 * b);
            histogram[gray]++;
        }
    }
   // printf("jaajruresaslol 5\n");

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
    //printf("dans fct1\n");

    SDL_Surface *aaa = IMG_Load(path);
    SDL_Surface *img = SDL_ConvertSurfaceFormat(aaa, SDL_PIXELFORMAT_RGB888, 0);
    if (SDL_MUSTLOCK(img))
    {
        SDL_LockSurface(img);
    }
   // printf("dans fct1\n");
    Uint32 *pixels = (Uint32 *)img->pixels;
    //printf("dans fctffff1\n");

    int width = img->w;
    int height = img->h;
   // printf("dans fctfffffffffffffffffeefefe1\n");

    if (height != IMG_H || width != IMG_W)
        err(EXIT_FAILURE, "Img with path %s doesn't have the required dimensions.", path);
   // printf("dans fct555fffffffffffffffffffffffffffffffffffffffffffzzz51\n");

    //Uint8 threshold = calculate_otsu_threshold(img);
    //printf("dans fct55551\n");

    double *array = calloc(IMG_W * IMG_H, sizeof(double));
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Uint32 pixel = pixels[(y * width) + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, img->format, &r, &g, &b);
           // Uint8 gray = (Uint8)(0.299 * r + 0.587 * g + 0.114 * b);
            //Uint32 bw_pixel = gray > threshold ? SDL_MapRGB(img->format, 255, 255, 255)
            //                                   : SDL_MapRGB(img->format, 0, 0, 0);

            //pixels[(y * width) + x] = bw_pixel;
            SDL_GetRGB(pixel, img->format, &r, &g, &b);
            array[y * IMG_W + x] = ((double)g) / 255;
        }
    }
    //printf("dans icici coucou\n");


    if (SDL_MUSTLOCK(img))
    {
        SDL_UnlockSurface(img);
    }
    //printf("dans icici bquoichoufh\n");

    return array;
}

/*
void print_result(Network* network){
    printf("-- RESULTS --\n\t");

    for(size_t let = 0; let < 26; let ++){
        printf("%c\t", 'A'+let);
    }
    for(size_t letter = 0; letter < 26; letter++ ){
        network_predict(network, get_letter(letter));
        printf("%c", letter+'A');
        for(size_t proba_letter = 0; proba_letter < 26; proba_letter){
            
        }
        printf("\n");
    }

}*/
void printColor(double value) {
    // Ensure value is between 0.0 and 1.0
    if (value < 0.0) value = 0.0;
    if (value > 1.0) value = 1.0;

    // Calculate red and blue components for blue-to-purple transition
    int red = (int)(value * 128);         // Red component increases from 0 to 128
    int green = 0;                        // Green component stays at 0
    int blue = (int)(255 - value * 127);  // Blue component decreases from 255 to 128

    // ANSI escape code for RGB foreground color
    printf("\033[38;2;%d;%d;%dm%.3f\033[0m", red, green, blue, value);
}
int indexOfMax(double *arr, int size) {
    if (size <= 0) {
        return -1; // Return -1 for empty array or invalid size
    }

    int maxIndex = 0;
    for (int i = 1; i < size; i++) {
        if (arr[i] > arr[maxIndex]) {
            maxIndex = i;
        }
    }
    
    return maxIndex;
}
int get_rank(double arr[], int size, int i) {   
    int rank = 1;
    double target = arr[i];
    
    for (int j = 0; j < size; j++) {
        if (arr[j] > target) {
            rank++;
        }
    }
    
    return rank;
}

void print_current_iter(const Network* net, const char current_letter, const size_t iter, const size_t max_iter){
    printf("--- %ld/%ld ---\n", iter,max_iter);
    printf("CHAR = %c: [", current_letter);
    for(char i = 0; i <26; i++){
        printf("%c = ",'A'+i);
        printColor(net->output[i]);
        printf(", ");
    }
    printf("] Best guess: %c ", indexOfMax(net->output, 26)+'A');
    printf("Rank: ");
    int rank = get_rank(net->output, 26, current_letter - 'A');
    if(rank == 1){
        printf("\033[32m\033[1m%d\033[0m", rank);
    } else if (rank < 5){
        printf("\033[33m\033[1m%d\033[0m", rank);

    } else {
        printf("\033[31m\033[1m%d\033[0m", rank);

    }
    printf("\n");
    printf("-----\n");

}


int main(int argc, char **argv)
{
    if (argc != 3)
        errx(EXIT_FAILURE, "Usage: ./training <training_dataset_directory> <testing_dataset_directory> TRAILING SLASH IS REQUIRED SINON CA MARCHE PAS JAIME PAS LE C");

    Network *network = calloc(1, sizeof(Network));
    if(network == NULL)
        errx(EXIT_FAILURE, "jaaj");
    network_init(network, INPUT_LAYER_SIZE, HIDDEN_LAYER_SIZE, OUTPUT_LAYER_SIZE);
    Trainer *trainer = calloc(1, sizeof(Trainer));
    if(trainer == NULL)
        errx(EXIT_FAILURE, "jaaj2");
    trainer_init(trainer, network);

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

    closedir(testing_directory);
    closedir(training_directory);

   // printf("sample1 sample2 %d %d\n", sample_testing_size, sample_training_size);

    char **training_img_path = calloc(sample_training_size, sizeof(char *));
    char **testing_img_path = calloc(sample_testing_size, sizeof(char *));

    training_directory = opendir(argv[1]);

    if (training_directory == NULL)
        err(EXIT_FAILURE, "Error while opening training directory");

    testing_directory = opendir(argv[2]);

    if (testing_directory == NULL)
        err(EXIT_FAILURE, "Error while opening testing directory");

    size_t idx = 0;
    while ((entry = readdir(training_directory)) != NULL)
    {
        //printf("Jaaj %s\n", entry->d_name);
        if(entry->d_name[0] == '.') continue;
        training_img_path[idx] = calloc(strlen(entry->d_name)+1, sizeof(char));
        //printf("different de noune %d -strlen %d string: %s\n", training_img_path!=NULL, strlen(training_img_path[idx]),training_img_path[idx]);
        strcpy(training_img_path[idx], entry->d_name); // putain de ta mere pq ca bug
        //printf("icijaaj\n");
        idx++;
    }

    idx = 0;
    struct dirent *entry2;

    while ((entry2 = readdir(testing_directory)) != NULL)
    {
        if(entry2->d_name[0] == '.') continue;
        testing_img_path[idx] = calloc(strlen(entry2->d_name)+1, sizeof(char));
        strcpy(testing_img_path[idx++], entry2->d_name);
    }

    closedir(testing_directory);
    closedir(training_directory);
        //printf("JAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAJ\n");

    double **training_data = calloc(sample_training_size, sizeof(double*));
    double **testing_data = calloc(sample_testing_size, sizeof(double*));

    double **targeted_data = calloc(sample_training_size, sizeof(double*));
    idx = 0;
    while (idx < sample_training_size) 
    {
                     // printf("JAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAJ\n");


        char* path1 = /*callloc(12*1024, sizeof(char));*/calloc(strlen(argv[1])+1+strlen(training_img_path[idx])+1, sizeof(char));
        //printf("JAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAJ\n");

        char* path2 = /*callloc(12*1024, sizeof(char));*/calloc(strlen(argv[2])+1+strlen(testing_img_path[idx])+1, sizeof(char));
        strcpy(path1, argv[1]);
        //printf("JAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAJ1\n");

        
        //printf("JAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAJ2\n");

        strcat(path1, training_img_path[idx]);
        //printf("JAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAJ3\n");

        strcpy(path2, argv[2]);
        
        strcat(path2, testing_img_path[idx]);
        //printf("JAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAJ4\n");

        //printf("stringjaaj loading path1=%s path2=%s\n", path1, path2);
        //training_data[idx] = callloc(IMG_H * IMG_W, sizeof(double));
        training_data[idx] = load_image_bw(path1);
        //printf("JAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAJ5\n");

        //testing_data[idx] = callloc(IMG_H * IMG_W, sizeof(double));
        testing_data[idx] = load_image_bw(path2);
        //targeted_data[idx] = callloc(26, sizeof(double));
        targeted_data[idx] = get_target(training_img_path[idx]);
        free(path1);
        free(path2);
        idx++;
    }
  //  shuffle(training_data,targeted_data,sample_testing_size);

    size_t idx_2 = 150;
    for(size_t i = 0; i <= idx_2; i++){
        shuffle(training_data,targeted_data,sample_training_size);
        printf("Current iter: %ld\n", i);
        for (size_t j = 0; j < sample_training_size; j++)
        {
            trainer_train(trainer,network,training_data[j],targeted_data[j],0.1);
            if(i == idx_2) print_current_iter(network, indexOfMax(targeted_data[j], 26)+'A' , j, idx_2*sample_training_size);
            //j++;
            //coun++;
        //printf("%d %d \n", i, j);

        }
    }

    //print_network(network);

    //free

    for(size_t i = 0; i < sample_training_size; i++){
        free(targeted_data[i]);
        free(training_data[i]);
        free(training_img_path[i]);
    }
    free(targeted_data);
    free(training_data);
    free(training_img_path);

     for(size_t i = 0; i < sample_testing_size; i++){
        free(testing_data[i]);
        free(testing_img_path[i]);
    }
    free(testing_data);
    free(testing_img_path);

    
    trainer_free(trainer);
    network_free(network);

    free(network);
    free(trainer);
    return EXIT_SUCCESS;
}

//TODO: 
// - Function that gets the char** of the training images, then adds its data to the double** and adds the expected result (i.e. the probability of the letter) to an other double **. target[i][proba_letter]
