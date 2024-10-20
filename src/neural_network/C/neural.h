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
typedef struct Network {
    double* weights_hidden;
    double* biases_hidden;
    double* weights_output;
    double* biases_output;
    double* hidden;
    double* output;
    enum ActivationFunction hidden_activation;
    enum ActivationFunction ouput_activation;
    //function pointer should be faster than checking manually at each training step which function to use
    double (*hidden_fct) (double);
    double (*output_fct) (double);
    double (*d_hidden_fct) (double);
    double (*d_output_fct) (double);
    size_t n_inputs;
    size_t n_hidden;
    size_t n_outputs;
} Network;

Network* network_init(
    size_t n_inputs,
    size_t n_hidden,
    size_t n_outputs,
    enum ActivationFunction activation_hidden, 
    enum ActivationFunction activation_output
    );
void network_free(Network* network);
void network_predict(Network* network, double* input);

typedef struct Trainer {
    double* grad_hidden;
    double* grad_output;
} Trainer;
void print_network(const Network* network);

Trainer* trainer_init(Trainer* trainer, Network* network);
void trainer_train(Trainer* trainer, Network* network, double* input, double* output, double lr);
void trainer_free(Trainer* trainer);
void save_nn_data(Network* network, const char* path);
void is_network_dead(Network* network);

#endif
