#ifndef NEURAL_H
#define NEURAL_H

#include <stdlib.h>

typedef enum ActivationFunction {
  SOFTMAX,
  SIGMOID,
  RELU,
  ELU,
  LRELU,
  TANH

} ActivationFunction;

/*
 * Struct definition for neural network
 *
 */
typedef struct Network {
  size_t nb_input;
  size_t nb_hidden;
  size_t nb_output;
  double* hidden_weights;
  double* hidden_biases;
  double* output_weights;
  double* output_biases;
  double* hidden;
  double* output;
  ActivationFunction hidden_activation;
  ActivationFunction ouput_activation;
  // function pointer should be faster than checking manually at each training
  // step which function to use
  double (*hidden_fct)(double);
  double (*output_fct)(double);
  double (*d_hidden_fct)(double);
  double (*d_output_fct)(double);

} Network;

typedef struct NetworkTrainer {
  double* gradients_hidden;
  double* gradients_output;
} NetworkTrainer;

Network* init_nn(size_t input_layer_size,
                      size_t hidden_layer_size,
                      size_t output_layer_size,
                      ActivationFunction activation_hidden,
                      ActivationFunction activation_output);
void free_nn(Network* network);
void predict_nn(Network* network, double* input);

NetworkTrainer* init_nt(Network* network);

void train_nn(NetworkTrainer* trainer,
                   Network* network,
                   double* input,
                   double* target,
                   double lr);
void free_nt(NetworkTrainer* trainer);
void print_nn(const Network* network);
void save_nn_data(const Network* network, const char* path);
Network* load_nn_data(const char* path);
void is_network_dead(const Network* network);
void print_graphviz(const Network* net);

#endif
