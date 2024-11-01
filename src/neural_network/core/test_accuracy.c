#include <dirent.h>
#include <err.h>
#include <stdlib.h>

#include "core_network.h"
#include "ocr.h"

// TODO: Add param for gs or nb
int main(int argc, char** argv) {
  if (argc != 4)
    errx(EXIT_FAILURE,
         "Usage: %s <model data> <testing images directory> <0|1, 1 = bw; 0 = "
         "gray scale>",
         argv[0]);

  Network* network = load_nn_data(argv[1]);
  int is_bw = atoi(argv[3]);
  char* testing_directory = argv[2];

  size_t sample_testing_size = 0;
  char** testing_img_path =
      get_filenames_in_dir(testing_directory, &sample_testing_size);

  double** testing_data = calloc(sample_testing_size, sizeof(double*));
  if (testing_data == NULL) {
    errx(EXIT_FAILURE, "Error while allocating memory");
  }

  for (size_t j = 0; j < sample_testing_size; j++) {
    size_t path_length =
        strlen(testing_directory) + 1 + strlen(testing_img_path[j]) + 1;

    char* path2 = calloc(path_length, sizeof(char));
    if (path2 == NULL) {
      errx(EXIT_FAILURE, "Error while allocating memory");
    }

    snprintf(path2, path_length, "%s/%s", testing_directory,
             testing_img_path[j]);

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
 
  print_table(network, &testing_img_path, &testing_data, sample_testing_size);

  free_nn(network);
  for (size_t k = 0; k < sample_testing_size; k++)
    free(testing_img_path[k]);
  free(testing_img_path);
  return EXIT_SUCCESS;
}