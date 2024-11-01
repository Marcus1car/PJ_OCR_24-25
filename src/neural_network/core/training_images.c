#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <dirent.h>
#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "SDL2/SDL.h"
#include "core_network.h"
#include "ocr.h"

// #define IMG_W 32
// #define IMG_H 32

#define INPUT_LAYER_SIZE IMG_H* IMG_W
#define HIDDEN_LAYER_SIZE 256  // Arbitrary
#define OUTPUT_LAYER_SIZE 26

int main(int argc, char** argv) {
  if (argc != 8)
    errx(EXIT_FAILURE,
         "Usage: %s <hidden_fct> <output_fct> <training_steps> "
         "<training_dataset_directory> <testing_dataset_directory> <0|1, 0 = "
         "grayscale, 1 = bw> <lr>",
         argv[0]);
  ActivationFunction hidden_fct = (ActivationFunction)atoi(argv[1]);
  ActivationFunction output_fct = (ActivationFunction)atoi(argv[2]);
  int training_steps_ = atoi(argv[3]);
  int is_bw = atoi(argv[6]);
  double lr = atof(argv[7]);

  if (hidden_fct <= SOFTMAX || hidden_fct > TANH)
    errx(EXIT_FAILURE,
         "Hidden activation fct invalid. %d \nSOFTMAX \t= 0\nSIGMOID \t= "
         "1\n\n\n",
         hidden_fct);
  if (output_fct < SOFTMAX || output_fct > TANH)
    errx(EXIT_FAILURE,
         "Output activation fct invalid. \nSOFTMAX \t= 0\nSIGMOID \t= 1\n\n\n");
  if (training_steps_ <= 0)
    errx(EXIT_FAILURE, "Training step invalid");
  size_t training_steps = training_steps_;

  /*printf("hidden_fct = %ld\noutput_fct = %ld\nsteps = %ld\n", hidden_fct,
         output_fct, training_steps);*/
  Network* network = init_nn(INPUT_LAYER_SIZE, HIDDEN_LAYER_SIZE,
                             OUTPUT_LAYER_SIZE, hidden_fct, output_fct);
  NetworkTrainer* trainer = init_nt(network);

  if (trainer == NULL || network == NULL)
    errx(EXIT_FAILURE, "Error while allocating network and trainer");

  DIR* training_directory = opendir(argv[4]);

  if (training_directory == NULL)
    err(EXIT_FAILURE, "Error while opening training directory");

  DIR* testing_directory = opendir(argv[5]);

  if (testing_directory == NULL)
    err(EXIT_FAILURE, "Error while opening testing directory");

  struct dirent* entry;
  size_t sample_training_size = 0;
  size_t sample_testing_size = 0;

  while ((entry = readdir(training_directory)) != NULL) {
    sample_training_size++;
  }

  while ((entry = readdir(testing_directory)) != NULL) {
    sample_testing_size++;
  }
  sample_testing_size -= 2;
  sample_training_size -= 2;

  closedir(testing_directory);
  closedir(training_directory);

  char** training_img_path = calloc(sample_training_size, sizeof(char*));
  char** testing_img_path = calloc(sample_testing_size, sizeof(char*));

  if (training_img_path == NULL || testing_img_path == NULL) {
    errx(EXIT_FAILURE, "Error while allocating memory");
  }

  training_directory = opendir(argv[4]);

  if (training_directory == NULL)
    err(EXIT_FAILURE, "Error while opening training directory");

  testing_directory = opendir(argv[5]);

  if (testing_directory == NULL)
    err(EXIT_FAILURE, "Error while opening testing directory");

  size_t idx = 0;
  while ((entry = readdir(training_directory)) != NULL) {
    if (entry->d_name[0] == '.')
      continue;
    training_img_path[idx] = calloc(strlen(entry->d_name) + 1, sizeof(char));
    if (training_img_path[idx] == NULL) {
      errx(EXIT_FAILURE, "Error while allocating memory");
    }
    strcpy(training_img_path[idx], entry->d_name);
    idx++;
  }

  idx = 0;
  struct dirent* entry2;

  while ((entry2 = readdir(testing_directory)) != NULL) {
    if (entry2->d_name[0] == '.')
      continue;
    testing_img_path[idx] = calloc(strlen(entry2->d_name) + 1, sizeof(char));
    if (testing_img_path[idx] == NULL) {
      errx(EXIT_FAILURE, "Error while allocating memory");
    }
    strcpy(testing_img_path[idx++], entry2->d_name);
  }

  closedir(testing_directory);
  closedir(training_directory);

  double** training_data = calloc(sample_training_size, sizeof(double*));
  if (training_data == NULL) {
    errx(EXIT_FAILURE, "Error while allocating memory");
  }
  double** testing_data = calloc(sample_testing_size, sizeof(double*));
  if (testing_data == NULL) {
    errx(EXIT_FAILURE, "Error while allocating memory");
  }

  double** targeted_data = calloc(sample_training_size, sizeof(double*));
  if (targeted_data == NULL) {
    errx(EXIT_FAILURE, "Error while allocating memory");
  }
  idx = 0;
  while (idx < sample_training_size) {
    char* path1 = calloc(
        strlen(argv[4]) + 1 + strlen(training_img_path[idx]) + 1, sizeof(char));
    if (path1 == NULL) {
      errx(EXIT_FAILURE, "Error while allocating memory");
    }
    strcpy(path1, argv[4]);
    strcat(path1, "/");
    strcat(path1, training_img_path[idx]);

    SDL_Surface* a = load_image(path1);
    if (is_bw == 1) {
      to_bw(a);
    } else {
      to_gs(a);
    }

    training_data[idx] = to_double_array(a);
    targeted_data[idx] = get_target(training_img_path[idx]);
    SDL_FreeSurface(a);
    free(path1);
    idx++;
  }
  for (size_t j = 0; j < sample_testing_size; j++) {
    char* path2 = calloc(strlen(argv[5]) + 1 + strlen(testing_img_path[j]) + 1,
                         sizeof(char));
    if (path2 == NULL) {
      errx(EXIT_FAILURE, "Error while allocating memory");
    }
    strcpy(path2, argv[5]);
    strcat(path2, "/");
    strcat(path2, testing_img_path[j]);

    SDL_Surface* a = load_image(path2);
    if (is_bw == 1) {
      to_bw(a);
    } else {
      to_gs(a);
    }

    testing_data[j] = to_double_array(a);
    SDL_FreeSurface(a);
    free(path2);
  }

  for (size_t i = 0; i < training_steps; i++) {
    shuffle(training_data, targeted_data, sample_training_size);
    printf("Current iter: %ld\n", i);
    // #pragma acc parallel loop

    for (size_t j = 0; j < sample_training_size; j++) {
      train_nn(trainer, network, training_data[j], targeted_data[j], lr);
      if (i == training_steps - 1)
        print_current_iter(network, indexOfMax(targeted_data[j], 26) + 'A', j,
                           training_steps * sample_training_size);
    }
  }

  printf("Training done - Testing the results (%ld) (%ld)\n",
         sample_testing_size, sample_training_size);
  printf("Letter\t");
  for (char k = 0; k < 26; k++)
    printf("%c\t", k + 'A');
  printf("\n");

  for (size_t k = 0; k < sample_testing_size; k++) {
    predict_nn(network, testing_data[k]);
    printf("%c\t", testing_img_path[k][0]);
    for (size_t f = 0; f < 26; f++) {
      printColor(network->output[f]);
      printf("\t");
    }
    if (get_rank(network->output, 26, testing_img_path[k][0] - 'a') == 1)
      printf("✅");
    else
      printf("❌");

    printf("\n");
  }

  save_nn_data(network, "./ocr.data");

  for (size_t i = 0; i < sample_training_size; i++) {
    free(targeted_data[i]);
    free(training_data[i]);
    free(training_img_path[i]);
  }
  free(targeted_data);
  free(training_data);
  free(training_img_path);

  for (size_t i = 0; i < sample_testing_size; i++) {
    free(testing_data[i]);
    free(testing_img_path[i]);
  }
  free(testing_data);
  free(testing_img_path);

  free_nt(trainer);
  free_nn(network);
  return EXIT_SUCCESS;
}