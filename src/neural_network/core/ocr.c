#include <SDL2/SDL.h>
#include "neural.h"

#define OUTPUT_SIZE 26
#define IMG_H 32
#define IMG_W 32

Network* init_ocr(size_t hidden){
    if(hidden <= 0){
        errx(EXIT_FAILURE, "Invalid hidden layer size");
    }
    return network_init(IMG_H * IMG_W, hidden, 26, RELU, SOFTMAX);
}

double* predict_from_surface(Network* ocr, SDL_Surface*){

}