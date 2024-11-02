/*
 * Neural network PoC - Project Defense #1
 * This scripts loads the data saved by poc.c and performs predictions according
 * to the config and outputs the results of 6 logic gates (XOR, XNOR, OR, NOR,
 * AND, NAND)
 */

#include <err.h>
#include <stdio.h>
#include "lib/core_network.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    errx(EXIT_FAILURE, "Usage %s <NN config path>", argv[0]);
  }
  printf("Loading Neural network data\n");

  Network* network = load_nn_data(argv[1]);

  printf("Neural network data loaded. Printing results:\n");

  double inputs[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
  printf("Results: \n\tOR\tNOR\tAND\tNAND\tXOR\tXNOR\n");

  for (size_t i = 0; i < 4; i++) {
    double* input = inputs[i];
    predict_nn(network, input);
    printf("%.0f,%.0f = \t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\n", input[0],
           input[1], network->output[0], network->output[1], network->output[2],
           network->output[3], network->output[4], network->output[5]);
  }
  free_nn(network);
  return EXIT_SUCCESS;
}