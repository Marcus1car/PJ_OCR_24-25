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

/**
 * @brief Prints the result of the neural network for boolean values
 *
 * @param inputs a nested list of double. Should normally be equal to {{0, 0},
 * {0, 1}, {1, 0}, {1, 1}}
 * @param network The neural network to print results from
 */
void print_res(double inputs[4][2], Network* network) {
  printf("Results: \n\tXOR\tXNOR\tOR\tAND\tNOR\tNAND\n");
  for (size_t i = 0; i < 4; i++) {
    double* input = inputs[i];
    predict_nn(network, input);
    printf("%.0f,%.0f = \t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\n", input[0],
           input[1], network->output[0], network->output[1], network->output[2],
           network->output[3], network->output[4], network->output[5]);
  }
}

int main(void) {
  Network* network = init_nn(2, 2, 6, SIGMOID, SIGMOID);
  NetworkTrainer* trainer = init_nt(network);
  double inputs[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
  double outputs[4][6] = {// XOR XNOR OR AND NOR NAND
                          {0, 1, 0, 0, 1, 1},
                          {1, 0, 1, 0, 0, 1},
                          {1, 0, 1, 0, 0, 1},
                          {0, 1, 1, 1, 0, 0}};
  printf("[I]: Results after initializing NN with random values\n");
  print_res(inputs, network);

  for (size_t i = 0; i < ITERS; i++) {
    for (size_t k = 0; k < 4; k++) {
      train_nn(trainer, network, inputs[k], outputs[k], 20);
    }
  }
  printf("[I]: Results after %d iterations\n", ITERS);

  print_res(inputs, network);

  printf("[I]: Printing graphviz representation\n");

  print_graphviz(network);

  print_nn(network);

  printf("[I]: Saving data to ./poc_training.data\n");

  save_nn_data(network, "./poc_training.data");

  printf("[I]: Freeing network trainer and network\n");

  free_nt(trainer);
  free_nn(network);
  return EXIT_SUCCESS;
}