#ifndef CORE_NETWORK_H
#define CORE_NETWORK_H

#include <stdlib.h>


/// Activation functions

double relu(double x);
double d_relu(double x);
double elu(double x);
double d_elu(double x);
double lrelu(double x);
double d_lrelu(double x);
double sigmoid(double x);
double d_sigmoid(double x);
double tanh_(double x);
double d_tanh(double x);

typedef enum ActivationFunction {
  SOFTMAX,
  SIGMOID,
  RELU,
  ELU,
  LRELU,
  TANH

} ActivationFunction;

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

NetworkTrainer* init_nt(Network* network);

void predict_nn(Network* network, double* input);
void train_nn(NetworkTrainer* trainer,
                   Network* network,
                   double* input,
                   double* target,
                   double lr);

void print_nn(const Network* network);
void save_nn_data(const Network* network, const char* path);
Network* load_nn_data(const char* path);

void is_network_dead(const Network* network);
void print_graphviz(const Network* net);

void free_nt(NetworkTrainer* trainer);
void free_nn(Network* network);


#endif
