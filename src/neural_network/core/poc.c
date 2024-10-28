/*
 * Neural network PoC - Project Defense #1
 * This scripts trains a simple neural network which outputs the results of 6
 * logic gates (XOR, XNOR, OR, NOR, AND, NAND)
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "neural.h"

#define ITERS 100000

void print_res(double inputs[4][2], Network* network) {
  printf("Results: \n\tXOR\tXNOR\tOR\tAND\tNOR\tNAND\n");
  for (size_t i = 0; i < 4; i++) {
    double* input = inputs[i];
    network_predict(network, input);
    printf("%.0f,%.0f = \t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\n", input[0],
           input[1], network->output[0], network->output[1], network->output[2],
           network->output[3], network->output[4], network->output[5]);
  }
}

int main() {
  Network* network = network_init(2, 2, 6, SIGMOID, SIGMOID);
  Trainer* trainer = trainer_init(network);
  double inputs[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
  double outputs[4][6] = {// XOR XNOR OR AND NOR NAND
                          {0, 1, 0, 0, 1, 1},
                          {1, 0, 1, 0, 0, 1},
                          {1, 0, 1, 0, 0, 1},
                          {0, 1, 1, 1, 0, 0}};
  printf("Results after initializing NN\n");
  print_res(inputs, network);

  for (size_t i = 0; i < ITERS; i++) {
    for (size_t k = 0; k < 4; k++) {
      double* input = inputs[k];
      double* output = outputs[k];

      trainer_train(trainer, network, input, output, 20);
    }
  }
  printf("Results after %d iterations\n", ITERS);

  print_res(inputs, network);
  print_graphviz(network);

  print_network(network);
  save_nn_data(network, "./poc_training.data");
  trainer_free(trainer);
  network_free(network);
  return EXIT_SUCCESS;
}