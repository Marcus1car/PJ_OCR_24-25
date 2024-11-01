#include <err.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// #include <omp.h>

#include "neural.h"

/*
 * Activation functions and their derivatives (prefixed with d_)
 * NOTE: Softmax is not defined as a function here
 */

/*RELU*/
double relu(double x) {
  return x > 0 ? x : 0;
}
double d_relu(double x) {
  return x > 0 ? 1 : 0;
}

/*Exponential relu with some factor*/
double elu(double x) {
  return x > 0 ? x : 0.3 * (exp(x) - 1);
}
double d_elu(double x) {
  return x > 0 ? 1 : 0.3 * (exp(x));
}

/*Leaky relu*/
double lrelu(double x) {
  return x > 0 ? x : 0.01 * x;
}
double d_lrelu(double x) {
  return x > 0 ? 1 : 0.01;
}

/*Sigmoid*/
double sigmoid(double x) {
  return 1.0 / (1.0 + exp(-x));
}
double d_sigmoid(double x) {
  return x * (1.0 - x);
}

/*Hyperbolic tangent*/
double tanh_(double x) {
  return tanh(x);
}
double d_tanh(double x) {
  double t = tanh(x);
  return fma(t, t, -1);  // fma(x,y,z) = x*y+z without losing precision
}

/**
 * @brief Initialize a neural network and returns a pointer to a newly allocated
 * struct.
 *
 * **NOTE**: The struct should be freed using the free_nn() function.
 *
 * @param input_layer_size Size of the input layer
 * @param hidden_layer_size Size of the hidden layer
 * @param output_layer_size Size of the output layer
 * @param activation_hidden Activation function of the hidden layer (Available
 * functions are in the header associated with this file).
 * @param activation_output Activation function of the output layer (Available
 * functions are in the header associated with this file).
 *
 */
Network* init_nn(size_t input_layer_size,
                      size_t hidden_layer_size,
                      size_t output_layer_size,
                      ActivationFunction activation_hidden,
                      ActivationFunction activation_output) {
  Network* network = (Network*)malloc(sizeof(Network));
  if (network == NULL) {
    errx(EXIT_FAILURE, "Memory allocation failed");
  }

  if (activation_hidden == SOFTMAX) {
    errx(EXIT_FAILURE, "Softmax on hidden layer is not possible");
  }

  srand(time(NULL));

  network->nb_input = input_layer_size;
  network->nb_hidden = hidden_layer_size;
  network->nb_output = output_layer_size;

  network->ouput_activation = activation_output;
  network->hidden_activation = activation_hidden;

  network->hidden = calloc(hidden_layer_size, sizeof(double));
  network->output = calloc(output_layer_size, sizeof(double));

  network->hidden_biases = calloc(hidden_layer_size, sizeof(double));
  network->output_biases = calloc(output_layer_size, sizeof(double));

  network->hidden_weights =
      calloc(input_layer_size * hidden_layer_size, sizeof(double));
  network->output_weights =
      calloc(hidden_layer_size * output_layer_size, sizeof(double));

  if (network->output == NULL || network->hidden == NULL ||
      network->hidden_biases == NULL || network->output_biases == NULL ||
      network->hidden_weights == NULL || network->output_weights == NULL) {
    errx(EXIT_FAILURE, "Memory allocation failed");
  }

  // initialize weights to random values
  for (size_t i = 0; i < input_layer_size * hidden_layer_size; i++) {
    network->hidden_weights[i] = ((double)rand() / (RAND_MAX / 2) - 1.) / 2.;
  }

  for (size_t i = 0; i < hidden_layer_size * output_layer_size; i++) {
    network->output_weights[i] = ((double)rand() / (RAND_MAX / 2) - 1.) / 2.;
  }

  // Define function pointers for activation functions of hidden and output
  // layer

  switch (network->hidden_activation) {
    case SIGMOID:
      network->hidden_fct = &sigmoid;
      network->d_hidden_fct = &d_sigmoid;
      break;
    case RELU:
      network->hidden_fct = &relu;
      network->d_hidden_fct = &d_relu;
      break;
    case LRELU:
      network->hidden_fct = &lrelu;
      network->d_hidden_fct = &d_lrelu;
      break;
    case ELU:
      network->hidden_fct = &elu;
      network->d_hidden_fct = &d_elu;
      break;
    case TANH:
      network->hidden_fct = &tanh_;
      network->d_hidden_fct = &d_tanh;
      break;
    default:
      errx(EXIT_FAILURE, "Unknown hidden activation function");
      break;
  }

  // Define function pointers for derivatives of activation functions
  // of hidden and output layer for back propagation
  switch (network->ouput_activation) {
    case SIGMOID:
      network->output_fct = &sigmoid;
      network->d_output_fct = &d_sigmoid;
      break;
    case RELU:
      network->output_fct = &relu;
      network->d_output_fct = &d_relu;
      break;
    case LRELU:
      network->output_fct = &lrelu;
      network->d_output_fct = &d_lrelu;
      break;
    case ELU:
      network->output_fct = &elu;
      network->d_output_fct = &d_elu;
      break;
    case TANH:
      network->output_fct = &tanh_;
      network->d_output_fct = &d_tanh;
      break;
    case SOFTMAX:
      network->output_fct = NULL;
      network->d_output_fct = NULL;
      break;
    default:
      errx(EXIT_FAILURE, "Unknown output activation function");
      break;
  }
  return network;
}

/**
 * @brief Frees a neural network heap-allocated memory
 *
 * @param network A pointer to the neural network structure to free
 *
 */
void free_nn(Network* network) {
  free(network->hidden_weights);
  free(network->hidden_biases);
  free(network->output_weights);
  free(network->output_biases);
  free(network->hidden);
  free(network->output);
  free(network);
}

/**
 * @brief Forward propagates (i.e predicts the result of) the input data.
 * Result is stored in the output pointer list of the neural network struct
 *
 * @param network A pointer to the neural network structure to perform the
 * forward propagation.
 *
 * @param input A double pointer list of size equal to the input layer size
 *
 */
void predict_nn(Network* network, double* input) {
  // * NOTE: Parallelization fails when using function pointers
  // #pragma acc kernels
  {
    // #pragma acc parallel loop
    for (size_t c = 0; c < network->nb_hidden; c++) {
      double sum = 0;
      // #pragma acc loop reduction(+ : sum)
      for (size_t r = 0; r < network->nb_input; r++) {
        sum += input[r] * network->hidden_weights[r * network->nb_hidden + c];
      }
      network->hidden[c] =
          /*relu*/ (*network->hidden_fct)(sum + network->hidden_biases[c]);
    }

    if (network->ouput_activation == SOFTMAX) {
      // #pragma acc parallel loop
      for (size_t c = 0; c < network->nb_output; c++) {
        double sum = 0.0;
        // #pragma acc loop reduction(+ : sum)
        for (size_t r = 0; r < network->nb_hidden; r++) {
          sum += network->hidden[r] *
                 network->output_weights[r * network->nb_output + c];
        }
        network->output[c] = sum + network->output_biases[c];
      }

      double max_output = network->output[0];
      for (size_t i = 0; i < network->nb_output; i++) {
        if (network->output[i] > max_output) {
          max_output = network->output[i];
        }
      }

      double sum_exp = 0;
      // #pragma acc parallel loop reduction(+ : sum_exp)
      for (size_t i = 0; i < network->nb_output; i++) {
        network->output[i] = exp(network->output[i] - max_output);
        sum_exp += network->output[i];
      }

      // #pragma acc parallel loop
      for (size_t i = 0; i < network->nb_output; i++) {
        network->output[i] /= sum_exp;
      }
    } else {
      // #pragma acc parallel loop
      for (size_t c = 0; c < network->nb_output; c++) {
        double sum = 0;
        // #pragma acc loop reduction(+ : sum)
        for (size_t r = 0; r < network->nb_hidden; r++) {
          sum += network->hidden[r] *
                 network->output_weights[r * network->nb_output + c];
        }

        network->output[c] =
            /*relu*/ (*network->output_fct)(sum + network->output_biases[c]);
      }
    }
  }
}

/**
 * @brief Prints in stdout wether the hidden or output layer is dead (i.e. all
 * of the neurons from a layer are equal to 0). Layers die when
 * learning rate is too high and using RELU or its variants.
 *
 * @param network A pointer to the neural network structure to test its layers.
 *
 */
void is_network_dead(const Network* network) {
  const double threshold = 0.00001;
  char hidden_dead = 1;
  char output_dead = 1;
  for (size_t k = 0; k < network->nb_hidden; k++) {
    if (fabs(network->hidden[k]) > threshold) {
      hidden_dead = 0;
      break;
    }
  }
  for (size_t k = 0; k < network->nb_output; k++) {
    if (fabs(network->output[k]) > threshold) {
      output_dead = 0;
      break;
    }
  }

  if (hidden_dead == 1) {
    printf("\033[31m\033[1mHIDDEN LAYER DEAD\033[0m\n");
  }
  if (output_dead == 1) {
    printf("\033[31m\033[1mOUT LAYER DEAD\033[0m\n");
  }
}

/**
 * @brief Returns a trainer struct pointer which is adapted for the specified
 * neural network
 *
 *  **NOTE**: The trainer pointer has to be freed after use
 *
 * @param network A pointer to the neural network structure to train.
 *
 */
NetworkTrainer* init_nt(Network* network) {
  NetworkTrainer* trainer = malloc(sizeof(NetworkTrainer));
  if (trainer == NULL) {
    errx(EXIT_FAILURE, "Error while allocating memory");
  }
  trainer->gradients_hidden = calloc(network->nb_hidden, sizeof(double));
  trainer->gradients_output = calloc(network->nb_output, sizeof(double));
  if (trainer->gradients_hidden == NULL || trainer->gradients_output == NULL) {
    errx(EXIT_FAILURE, "Error while allocating memory");
  }
  return trainer;
}

/**
 * @brief Trains the specified neural network using the specified trainer
 *
 * @param trainer A pointer to the trainer structure
 * @param network A pointer to the neural network structure to train
 * @param input A pointer used as a double list of size equal to the number of
 * input neurons
 * @param target A pointer used as a double list of size equal to the the size
 * of output layer. It represents the expected output of the given input
 * @param lr Learning rate. For RELU, and similar activation functions,
 * appropriate range is about 10^-6, otherwise between 0.1 and 1.
 */
void train_nn(NetworkTrainer* trainer,
              Network* network,
              double* input,
              double* target,
              double lr) {
  predict_nn(network, input);
  // is_network_dead(network);
  // * NOTE: Parallelization fails when using function pointers
  // #pragma acc kernels
  {
    // #pragma acc parallel loop
    for (size_t c = 0; c < network->nb_output; c++) {
      if (network->ouput_activation == SOFTMAX)
        trainer->gradients_output[c] = network->output[c] - target[c];
      else
        trainer->gradients_output[c] =
            (network->output[c] - target[c]) * /*d_relu*/
            (*network->d_output_fct)(network->output[c]);
    }
    // #pragma acc parallel loop
    for (size_t r = 0; r < network->nb_hidden; r++) {
      double sum = 0.0;
      // #pragma acc loop reduction(+ : sum)
      for (size_t c = 0; c < network->nb_output; c++) {
        sum += trainer->gradients_output[c] *
               network->output_weights[r * network->nb_output + c];
      }

      trainer->gradients_hidden[r] =
          sum * (*network->d_hidden_fct) /*d_relu*/ (network->hidden[r]);
    }
    // #pragma acc parallel loop collapse(2)
    for (size_t r = 0; r < network->nb_hidden; r++) {
      for (size_t c = 0; c < network->nb_output; c++) {
        network->output_weights[r * network->nb_output + c] -=
            lr * trainer->gradients_output[c] * network->hidden[r];
      }
    }
    // #pragma acc parallel loop collapse(2)
    for (size_t r = 0; r < network->nb_input; r++) {
      for (size_t c = 0; c < network->nb_hidden; c++) {
        network->hidden_weights[r * network->nb_hidden + c] -=
            lr * trainer->gradients_hidden[c] * input[r];
      }
    }
    // #pragma acc parallel loop
    for (size_t c = 0; c < network->nb_output; c++) {
      network->output_biases[c] -= lr * trainer->gradients_output[c];
    }
    // #pragma acc parallel loop
    for (size_t c = 0; c < network->nb_hidden; c++) {
      network->hidden_biases[c] -= lr * trainer->gradients_hidden[c];
    }
  }
}

/**
 * @brief Frees a trainer heap-allocated memory and invalidates the pointer
 *
 * @param trainer A pointer to the trainer structure
 */
void free_nt(NetworkTrainer* trainer) {
  free(trainer->gradients_hidden);
  free(trainer->gradients_output);
  free(trainer);
}

/**
 * @brief Prints the weights and biases of the neural network.
 *
 * @param network A pointer to the neural network to print data from
 */
void print_nn(const Network* network) {
  printf("Hidden layer weights:\n");
  for (size_t i = 0; i < network->nb_input; i++) {
    printf("\tInput neuron %ld: ", i);
    for (size_t j = 0; j < network->nb_hidden; j++) {
      printf("%9.6f ", network->hidden_weights[network->nb_input * i + j]);
    }

    printf("\n");
  }

  printf("\n");

  printf("Output layer weights:\n");
  for (size_t i = 0; i < network->nb_hidden; i++) {
    printf("\tHidden neuron %ld: ", i);

    for (size_t j = 0; j < network->nb_output; j++) {
      printf("%9.6f ", network->output_weights[i * network->nb_output + j]);
    }

    printf("\n");
  }
  printf("Biases of hidden layer:\n\t");
  for (size_t i = 0; i < network->nb_hidden; i++) {
    printf("%9.6f ", network->hidden_biases[i]);
  }
  printf("\n");

  printf("Biases of output layer:\n\t");
  for (size_t i = 0; i < network->nb_output; i++) {
    printf("%9.6f ", network->output_biases[i]);
  }

  printf("\n");
}

/**
 * @brief Saves the neural network data to a file.
 *
 * This function saves the structure and weights of the neural network to a
 * specified file. The file will contain the number of inputs, hidden neurons,
 * and outputs, followed by the weights of the hidden layer.
 *
 * The file will look like this (where Ln is the line number n):
 *
 * * L1: Hidden activation function
 *
 * * L2: Output activation function
 *
 * * L3: Number of input neurons
 *
 * * L4: Number of hidden neurons
 *
 * * L5: Number of output neurons
 *
 * * L6: Hidden layer weights (separated by ';')
 *
 * * L7: Hidden layer biases (separated by ';')
 *
 * * L8: Output layer weights (separated by ';')
 *
 * * L9: Output layer biases (separated by ';')
 *
 * @param network A pointer to the neural network structure containing the data
 * to be saved.
 * @param path The file path where the neural network data will be saved.
 */
void save_nn_data(const Network* network, const char* path) {
  FILE* fptr;
  fptr = fopen(path, "w");
  if (fptr == NULL)
    errx(EXIT_FAILURE, "Error while opening file to save config");

  fprintf(fptr, "%d\n%d\n%ld\n%ld\n%ld\n", network->hidden_activation,
          network->ouput_activation, network->nb_input, network->nb_hidden,
          network->nb_output);

  for (size_t i = 0; i < network->nb_input * network->nb_hidden; i++) {
    fprintf(fptr, "%9.12f;", network->hidden_weights[i]);
  }
  fprintf(fptr, "\n");
  for (size_t i = 0; i < network->nb_hidden; i++) {
    fprintf(fptr, "%9.12f;", network->hidden_biases[i]);
  }
  fprintf(fptr, "\n");
  for (size_t i = 0; i < network->nb_hidden * network->nb_output; i++) {
    fprintf(fptr, "%9.12f;", network->output_weights[i]);
  }
  fprintf(fptr, "\n");
  for (size_t i = 0; i < network->nb_output; i++) {
    fprintf(fptr, "%9.12f;", network->output_biases[i]);
  }
  fprintf(fptr, "\n");
  fclose(fptr);
}

/**
 * @brief Loads the neural network data from a file.
 *
 * This function loads the structure and weights of a neural network from a
 * specified file generated by the above function.
 *
 * @param network A pointer to the neural network structure which has been
 * previously initialized
 * @param path The file path where the neural network data has been saved.
 */
Network* load_nn_data(const char* path) {
  FILE* file = fopen(path, "r");
  if (!file) {
    errx(EXIT_FAILURE, "Error opening file");
  }

  long int act_hidden, act_output, input_layer_size_, n_hidden_, n_output_;
  if (fscanf(file, "%ld\n%ld\n%ld\n%ld\n%ld\n", &act_hidden, &act_output,
             &input_layer_size_, &n_hidden_, &n_output_) != 5) {
    fclose(file);
    errx(EXIT_FAILURE, "Parsing NN confing failed\n");
  }
  if (act_hidden < 0 || act_output < 0 || n_hidden_ <= 0 ||
      input_layer_size_ <= 0 || n_output_ <= 0) {
    errx(EXIT_FAILURE, "Inavlid NN confing");
  }
  size_t input_layer_size = input_layer_size_;
  size_t hidden_layer_size = n_hidden_;
  size_t output_layer_size = n_output_;

  Network* network = init_nn(input_layer_size, hidden_layer_size,
                                  output_layer_size, act_hidden, act_output);

  for (size_t i = 0; i < input_layer_size * hidden_layer_size; i++) {
    if (fscanf(file, "%lf;", &network->hidden_weights[i]) != 1) {
      free_nn(network);
      errx(EXIT_FAILURE, "Error while parsing WH at %ld", i);
    }
  }
  fscanf(file, "%*[\n;]");
  for (size_t i = 0; i < hidden_layer_size; i++) {
    if (fscanf(file, "%lf;", &network->hidden_biases[i]) != 1) {
      free_nn(network);
      errx(EXIT_FAILURE, "Error while parsing NH at %ld", i);
    }
  }
  fscanf(file, "%*[\n;]");
  for (size_t i = 0; i < hidden_layer_size * output_layer_size; i++) {
    if (fscanf(file, "%lf;", &network->output_weights[i]) != 1) {
      free_nn(network);
      errx(EXIT_FAILURE, "Error while parsing WO at %ld", i);
    }
  }
  fscanf(file, "%*[\n;]");
  for (size_t i = 0; i < output_layer_size; i++) {
    if (fscanf(file, "%lf;", &network->output_biases[i]) != 1) {
      free_nn(network);
      errx(EXIT_FAILURE, "Error while parsing WO at %ld", i);
    }
  }
  fscanf(file, "%*[\n;]");
  fclose(file);
  return network;
}

/**
 * @brief Prints a graphviz representation of the neural network for debugging
 *
 * @param network A pointer to the neural network structure to print
 */
void print_graphviz(const Network* net) {
  printf("digraph NeuralNetwork {\n");
  printf("    rankdir=LR;\n");
  printf("    splines=false;\n");

  printf("    subgraph cluster_input {\n");
  printf("        label=\"Input Layer\";\n");
  for (size_t i = 0; i < net->nb_input; i++) {
    printf(
        "        input%zu [label=\"Input %zu\", shape=circle, color=blue];\n",
        i, i);
  }
  printf("    }\n");

  printf("    subgraph cluster_hidden {\n");
  printf("        label=\"Hidden Layer\";\n");
  for (size_t j = 0; j < net->nb_hidden; j++) {
    printf(
        "        hidden%zu [label=\"Hidden %zu\\nb=%.2f\", shape=circle, "
        "color=green];\n",
        j, j, net->hidden_biases[j]);
  }
  printf("    }\n");

  printf("    subgraph cluster_output {\n");
  printf("        label=\"Output Layer\";\n");
  for (size_t k = 0; k < net->nb_output; k++) {
    printf(
        "        output%zu [label=\"Output %zu\\nb=%.2f\", shape=circle, "
        "color=red];\n",
        k, k, net->output_biases[k]);
  }
  printf("    }\n");

  for (size_t i = 0; i < net->nb_input; i++) {
    for (size_t j = 0; j < net->nb_hidden; j++) {
      double weight = net->hidden_weights[i * net->nb_hidden + j];
      printf("    input%zu -> hidden%zu [label=\"%.2f\"];\n", i, j, weight);
    }
  }

  for (size_t j = 0; j < net->nb_hidden; j++) {
    for (size_t k = 0; k < net->nb_output; k++) {
      double weight = net->output_weights[j * net->nb_output + k];
      printf("    hidden%zu -> output%zu [label=\"%.2f\"];\n", j, k, weight);
    }
  }

  printf("}\n");
}
