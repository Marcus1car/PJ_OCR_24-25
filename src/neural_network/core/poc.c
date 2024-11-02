/*
 * Neural network PoC - Project Defense #1
 * This scripts trains a simple neural network which outputs the results of 6
 * logic gates (XOR, XNOR, OR, NOR, AND, NAND)
 */

#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "lib/core_network.h"

/**
 * @brief Prints the result of the neural network for boolean values
 *
 * @param inputs a nested list of double. Should normally be equal to {{0, 0},
 * {0, 1}, {1, 0}, {1, 1}}
 * @param network The neural network to print results from
 */
void print_res(double inputs[4][2], Network* network) {
  printf("Results: \n\tOR\tNOR\tAND\tNAND\tXOR\tNXOR\n");
  for (size_t i = 0; i < 4; i++) {
    double* input = inputs[i];
    predict_nn(network, input);
    printf("%.0f,%.0f = \t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\n", input[0],
           input[1], network->output[0], network->output[1], network->output[2],
           network->output[3], network->output[4], network->output[5]);
  }
}

int main(int argc, char** argv) {
  if (argc != 5) {
    errx(EXIT_FAILURE,
         "Usage %s <training steps> <learning rate> <hidden activation "
         "funtion> <output activation function>. For activation functions:\n"
         "0 = SOFTMAX (not allowed here)\n"
         "1 = SIGMOID\n"
         "2 = RELU\n"
         "3 = ELU\n"
         "4 = LRELU\n"
         "5 = TANH\n",
         argv[0]);
  }
  long steps = atol(argv[1]);
  double lr = atof(argv[2]);
  int h_act_fct = atoi(argv[3]);
  int o_act_fct = atoi(argv[3]);
  if (lr < 0 || steps < 0)
    errx(EXIT_FAILURE, "Wrong learning rate or training steps");
  if (h_act_fct <= SOFTMAX || h_act_fct > TANH)
    errx(EXIT_FAILURE, "Wrong hidden activation function");
  if (o_act_fct <= SOFTMAX || o_act_fct > TANH)
    errx(EXIT_FAILURE, "Wrong output activation function");
  Network* network = init_nn(2, 2, 6, h_act_fct, o_act_fct);
  NetworkTrainer* trainer = init_nt(network);
  double inputs[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
  double outputs[4][6] = {// OR NOR AND NAND XOR XNOR
                          {0, 1, 0, 1, 0, 1},
                          {1, 0, 0, 1, 1, 0},
                          {1, 0, 0, 1, 1, 0},
                          {1, 0, 1, 0, 0, 1}};
  print_nn(network);
  printf("[I]: Results after initializing NN with random values\n");
  print_res(inputs, network);

  for (size_t i = 0; i < (size_t)steps; i++) {
    for (size_t k = 0; k < 4; k++) {
      train_nn(trainer, network, inputs[k], outputs[k], lr);
    }
  }
  printf("[I]: Results after %ld iterations\n", steps);

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