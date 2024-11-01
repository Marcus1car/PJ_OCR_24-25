#include <dirent.h>
#include <err.h>
#include <stdlib.h>

#include "core_network.h"
#include "ocr.h"

void printColor(double value) {
  // Ensure value is between 0.0 and 1.0
  if (value < 0.0)
    value = 0.0;
  if (value > 1.0)
    value = 1.0;

  // Calculate red and blue components for blue-to-purple transition
  int red = (int)(value * 128);  // Red component increases from 0 to 128
  int green = 0;                 // Green component stays at 0
  int blue =
      (int)(255 - value * 127);  // Blue component decreases from 255 to 128

  // ANSI escape code for RGB foreground color
  printf("\033[38;2;%d;%d;%dm%.3f\033[0m", red, green, blue, value);
}
int indexOfMax(double* arr, int size) {
  if (size <= 0) {
    return -1;  // Return -1 for empty array or invalid size
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

int main(int argc, char** argv) {
  if (argc != 3)
    errx(EXIT_FAILURE, "Usage: %s <model data> <testing images directory>",
         argv[0]);

  Network* network = load_nn_data(argv[1]);

  DIR* testing_directory = opendir(argv[2]);

  if (testing_directory == NULL)
    err(EXIT_FAILURE, "Error while opening testing directory");

  struct dirent* entry;
  size_t sample_testing_size = 0;

  while ((entry = readdir(testing_directory)) != NULL) {
    sample_testing_size++;
  }
  sample_testing_size -= 2;

  closedir(testing_directory);

  char** testing_img_path = calloc(sample_testing_size, sizeof(char*));

  if (testing_img_path == NULL) {
    errx(EXIT_FAILURE, "Error while allocating memory");
  }

  testing_directory = opendir(argv[2]);
  size_t idx = 0;

  while ((entry = readdir(testing_directory)) != NULL) {
    if (entry->d_name[0] == '.')
      continue;
    testing_img_path[idx] = calloc(strlen(entry->d_name) + 1, sizeof(char));
    if (testing_img_path[idx] == NULL) {
      errx(EXIT_FAILURE, "Error while allocating memory");
    }
    strcpy(testing_img_path[idx++], entry->d_name);
  }

  closedir(testing_directory);

  for (size_t j = 0; j < sample_testing_size; j++) {
    char* path2 = calloc(strlen(argv[2]) + 1 + strlen(testing_img_path[j]) + 1,
                         sizeof(char));
    if (path2 == NULL) {
      errx(EXIT_FAILURE, "Error while allocating memory");
    }
    strcpy(path2, argv[2]);
    strcat(path2, testing_img_path[j]);
    free(testing_img_path[j]);
    testing_img_path[j] = path2;
    // free(path2);
  }
  size_t nbgood = 0;
  printf("Letter\t");
  for (char k = 0; k < 26; k++)
    printf("%c\t", k + 'A');
  printf("\n");

  for (size_t k = 0; k < sample_testing_size; k++) {
    SDL_Surface* a = load_image(testing_img_path[k]);

    double* res = predict_from_surface(network, a);
    SDL_FreeSurface(a);
    printf("%c\t", testing_img_path[k][strlen(argv[2])]);
    for (size_t k = 0; k < 26; k++) {
      printColor(res[k]);
      printf("\t");
    }
    if (get_rank(res, 26, testing_img_path[k][+strlen(argv[2])] - 'a') == 1) {
      printf("✅");
      nbgood++;
    } else
      printf("❌");

    printf("\n");
    free(res);
  }
  printf("Accuracy: %9.3lf%% (%ld/%ld) \n",
         (double)nbgood / sample_testing_size * 100, nbgood,
         sample_testing_size);
  network_free(network);
  for (size_t k = 0; k < sample_testing_size; k++)
    free(testing_img_path[k]);
  free(testing_img_path);
  return EXIT_SUCCESS;
}