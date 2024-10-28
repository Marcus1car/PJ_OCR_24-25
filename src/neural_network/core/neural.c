#include "neural.h"

#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

// #include <omp.h>

/*
 * Activation functions and their derivatives (prefixed with d_)
 */
double sigmoid(double f) {
  return 1.0 / (1.0 + exp(-f));
}
double d_sigmoid(double f) {
  return f * (1.0 - f);
}
double relu(double f) {
  return f > 0 ? f : 0;
}
double d_relu(double f) {
  return f > 0 ? 1 : 0;
}
double elu(double f) {
  return f > 0 ? f : 0.3 * (exp(f) - 1);
}
double d_elu(double f) {
  return f > 0 ? 1 : 0.3 * (exp(f));
}
double lrelu(double f) {
  return f > 0 ? f : 0.01 * f;
}
double d_lrelu(double f) {
  return f > 0 ? 1 : 0.01;
}
double tanh_(double f) {
  return tanh(f);
}
double d_tanh(double f) {
  double t = tanh(f);
  return fma(t, t, -1);
}
// fma(x,y,z) = x*y+z without losing precision

/**
 * @brief Initialize a neural network and returns a pointer to a newly allocated
 * struct.
 *
 * **NOTE**: The struct should be freed using the network_free() function.
 *
 * @param n_inputs Size of the input layer
 * @param n_hidden Size of the hidden layer
 * @param n_outputs Size of the output layer
 * @param activation_hidden Activation function of the hidden layer (Available
 * functions are in the header associated with this file).
 * @param activation_output Activation function of the output layer (Available
 * functions are in the header associated with this file).
 *
 */
Network* network_init(size_t n_inputs,
                      size_t n_hidden,
                      size_t n_outputs,
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

  network->n_inputs = n_inputs;
  network->n_hidden = n_hidden;
  network->n_outputs = n_outputs;

  network->ouput_activation = activation_output;
  network->hidden_activation = activation_hidden;

  network->weights_hidden =
      calloc(n_inputs * n_hidden, sizeof(*network->weights_hidden));
  network->biases_hidden = calloc(n_hidden, sizeof(*network->biases_hidden));
  network->weights_output =
      calloc(n_hidden * n_outputs, sizeof(*network->weights_output));
  network->biases_output = calloc(n_outputs, sizeof(*network->biases_output));
  network->hidden = calloc(n_hidden, sizeof(*network->hidden));
  network->output = calloc(n_outputs, sizeof(*network->output));

  if (network->output == NULL || network->hidden == NULL ||
      network->biases_hidden == NULL || network->biases_output == NULL ||
      network->weights_hidden == NULL || network->weights_output == NULL) {
    errx(EXIT_FAILURE, "Memory allocation failed");
  }

  // initialize weights to random values
  for (size_t i = 0; i < n_inputs * n_hidden; i++) {
    // network->weights_hidden[i] = ((double)rand() / RAND_MAX) * 2 - 1;
    network->weights_hidden[i] = ((double)rand() / (RAND_MAX / 2) - 1.) / 2.;
  }

  for (size_t i = 0; i < n_hidden * n_outputs; i++) {
    // network->weights_output[i] = ((double)rand() / RAND_MAX) * 2 - 1;
    network->weights_output[i] = ((double)rand() / (RAND_MAX / 2) - 1.) / 2.;

  } /*
   for (size_t i = 0; i < n_outputs; i++) {
       network->output[i] = ((double)rand() / RAND_MAX) * 2 - 1;
   }*/

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
 * @brief Frees a neural network heap-allocated memory and invalidates its
 * pointer
 *
 * @param network A pointer to the neural network structure to free
 *
 */
void network_free(Network* network) {
  free(network->weights_hidden);
  free(network->biases_hidden);
  free(network->weights_output);
  free(network->biases_output);
  free(network->hidden);
  free(network->output);
  free(network);
  network = NULL;
}

/**
 * @brief Forward propagates the input data to the neural network. Result is
 * stored in the output pointer list of the neural network struct
 *
 * @param network A pointer to the neural network structure to perform the
 * forward propagation.
 *
 * @param input A double pointer list of size equal to the input layer size
 *
 */
void network_predict(Network* network, double* input) {
  for (size_t c = 0; c < network->n_hidden; c++) {
    double sum = 0;
    for (size_t r = 0; r < network->n_inputs; r++) {
      sum += input[r] * network->weights_hidden[r * network->n_hidden + c];
    }
    network->hidden[c] =
        (*network->hidden_fct)(sum + network->biases_hidden[c]);
  }
  if (network->ouput_activation == SOFTMAX) {
    for (size_t c = 0; c < network->n_outputs; c++) {
      double sum = 0.0;
      for (size_t r = 0; r < network->n_hidden; r++) {
        sum += network->hidden[r] *
               network->weights_output[r * network->n_outputs + c];
      }
      network->output[c] = sum + network->biases_output[c];
    }

    double max_output = network->output[0];
    for (size_t i = 1; i < network->n_outputs; i++) {
      if (network->output[i] > max_output) {
        max_output = network->output[i];
      }
    }

    // Compute the exponentials and sum them
    double sum_exp = 0;
    for (size_t i = 0; i < network->n_outputs; i++) {
      network->output[i] = exp(network->output[i] - max_output);
      sum_exp += network->output[i];
    }

    // Normalize by dividing each by the sum of exponentials
    for (size_t i = 0; i < network->n_outputs; i++) {
      network->output[i] /= sum_exp;
    }
  } else {
    for (size_t c = 0; c < network->n_outputs; c++) {
      double sum = 0;
      for (size_t r = 0; r < network->n_hidden; r++) {
        sum += network->hidden[r] *
               network->weights_output[r * network->n_outputs + c];
      }

      network->output[c] =
          (*network->output_fct)(sum + network->biases_output[c]);
    }
  }
}

/**
 * @brief Prints in stdout wether the hidden or output layer is dead (i.e. all
 * of the neuron from a layer are equal to 0). Happends frequently when learning
 * rate is too high while using RELU.
 *
 * @param network A pointer to the neural network structure to test its layers.
 *
 */
void is_network_dead(const Network* network) {
  const double threshold = 0.00001;
  char hidden_dead = 1;
  char output_dead = 1;
  for (size_t k = 0; k < network->n_hidden; k++) {
    if (fabs(network->hidden[k]) > threshold) {
      hidden_dead = 0;
      printf("ici");
      break;
    }
  }
  for (size_t k = 0; k < network->n_outputs; k++) {
    if (fabs(network->output[k]) > threshold) {
      output_dead = 0;
      printf("ici");

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
Trainer* trainer_init(Network* network) {
  Trainer* trainer = malloc(sizeof(Trainer));
  if (trainer == NULL) {
    errx(EXIT_FAILURE, "Error while allocating memory");
  }
  trainer->grad_hidden =
      calloc(network->n_hidden, sizeof(*trainer->grad_hidden));
  trainer->grad_output =
      calloc(network->n_outputs, sizeof(*trainer->grad_output));
  if (trainer->grad_hidden == NULL || trainer->grad_output == NULL) {
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
void trainer_train(Trainer* trainer,
                   Network* network,
                   double* input,
                   double* target,
                   double lr) {
  network_predict(network, input);
  // is_network_dead(network);

  for (size_t c = 0; c < network->n_outputs; c++) {
    // trainer->grad_output[c] = (network->output[c] - target[c]) *
    // sigmoid_prim(network->output[c]);
    if (network->ouput_activation == SOFTMAX)
      trainer->grad_output[c] = network->output[c] - target[c];
    else
      trainer->grad_output[c] = (network->output[c] - target[c]) *
                                (*network->d_output_fct)(network->output[c]);
  }

  for (size_t r = 0; r < network->n_hidden; r++) {
    double sum = 0.0;
    for (size_t c = 0; c < network->n_outputs; c++) {
      sum += trainer->grad_output[c] *
             network->weights_output[r * network->n_outputs + c];
    }

    trainer->grad_hidden[r] =
        sum * (*network->d_hidden_fct)(network->hidden[r]);
  }

  for (size_t r = 0; r < network->n_hidden; r++) {
    for (size_t c = 0; c < network->n_outputs; c++) {
      network->weights_output[r * network->n_outputs + c] -=
          lr * trainer->grad_output[c] * network->hidden[r];
    }
  }

  for (size_t r = 0; r < network->n_inputs; r++) {
    for (size_t c = 0; c < network->n_hidden; c++) {
      network->weights_hidden[r * network->n_hidden + c] -=
          lr * trainer->grad_hidden[c] * input[r];
    }
  }

  for (size_t c = 0; c < network->n_outputs; c++) {
    network->biases_output[c] -= lr * trainer->grad_output[c];
  }

  for (size_t c = 0; c < network->n_hidden; c++) {
    network->biases_hidden[c] -= lr * trainer->grad_hidden[c];
  }
}

/**
 * @brief Frees a trainer heap-allocated memory and invalidates the pointer
 *
 * @param trainer A pointer to the trainer structure
 */
void trainer_free(Trainer* trainer) {
  free(trainer->grad_hidden);
  free(trainer->grad_output);
  free(trainer);
  trainer = NULL;
}

/**
 * @brief Prints the weights and biases of the neural network.
 *
 * @param network A pointer to the neural network to print data from
 */
void print_network(const Network* network) {
  printf("Hidden layer weights:\n");
  for (size_t i = 0; i < network->n_inputs; i++) {
    printf("\tInput neuron %ld: ", i);
    for (size_t j = 0; j < network->n_hidden; j++) {
      printf("%9.6f ", network->weights_hidden[network->n_inputs * i + j]);
    }

    printf("\n");
  }

  printf("\n");

  printf("Output layer weights:\n");
  for (size_t i = 0; i < network->n_hidden; i++) {
    printf("\tHidden neuron %ld: ", i);

    for (size_t j = 0; j < network->n_outputs; j++) {
      printf("%9.6f ", network->weights_output[i * network->n_outputs + j]);
    }

    printf("\n");
  }
  printf("Biases of hidden layer:\n\t");
  for (size_t i = 0; i < network->n_hidden; i++) {
    printf("%9.6f ", network->biases_hidden[i]);
  }
  printf("\n");

  printf("Biases of output layer:\n\t");
  for (size_t i = 0; i < network->n_outputs; i++) {
    printf("%9.6f ", network->biases_output[i]);
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
  // size_t length = snprintf( NULL, 0, "%d;%d;%d",  network->n_inputs,
  // network->n_hidden, network->n_outputs); char* fst_line = calloc(length+1,
  // sizeof(char)); snprintf( fst_line, length+1, );
  FILE* fptr;
  fptr = fopen(path, "w");
  if (fptr == NULL)
    errx(EXIT_FAILURE, "Error while opening file to save config");

  fprintf(fptr, "%d\n%d\n%ld\n%ld\n%ld\n", network->hidden_activation,
          network->ouput_activation, network->n_inputs, network->n_hidden,
          network->n_outputs);

  for (size_t i = 0; i < network->n_inputs * network->n_hidden; i++) {
    fprintf(fptr, "%9.6f;", network->weights_hidden[i]);
  }
  fprintf(fptr, "\n");
  for (size_t i = 0; i < network->n_hidden; i++) {
    fprintf(fptr, "%9.6f;", network->biases_hidden[i]);
  }
  fprintf(fptr, "\n");
  for (size_t i = 0; i < network->n_hidden * network->n_outputs; i++) {
    fprintf(fptr, "%9.6f;", network->weights_output[i]);
  }
  fprintf(fptr, "\n");
  for (size_t i = 0; i < network->n_outputs; i++) {
    fprintf(fptr, "%9.6f;", network->biases_output[i]);
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

  long int act_hidden, act_output, n_input_, n_hidden_, n_output_;
  if (fscanf(file, "%ld\n%ld\n%ld\n%ld\n%ld\n", &act_hidden, &act_output, &n_input_,
             &n_hidden_, &n_output_) != 5) {
    fclose(file);
    errx(EXIT_FAILURE, "Parsing NN confing failed\n");
  }
  if(act_hidden < 0 || act_output < 0 || n_hidden_ <= 0 || n_input_ <= 0 || n_output_ <= 0){
    errx(EXIT_FAILURE, "Inavlid NN confing");
  } 
  size_t n_input = n_input_;
  size_t n_hidden = n_hidden_;
  size_t n_output = n_output_;

  Network* network =
      network_init(n_input, n_hidden, n_output, act_hidden, act_output);

  for (size_t i = 0; i < n_input * n_hidden; i++) {
    if (fscanf(file, "%lf;", &network->weights_hidden[i]) != 1) {
      errx(EXIT_FAILURE, "Error while parsing WH at %ld", i);
      network_free(network);
    }
  }
  size_t a = fscanf(file, "%*[\n;]");
  printf("a = %ld\n", a);
  for (size_t i = 0; i < n_hidden; i++) {
    if (fscanf(file, "%lf;", &network->biases_hidden[i]) != 1) {
      errx(EXIT_FAILURE, "Error while parsing NH at %ld", i);
      network_free(network);
    }
  }
  fscanf(file, "%*[\n;]");
  for (size_t i = 0; i < n_hidden * n_output; i++) {
    if (fscanf(file, "%lf;", &network->weights_output[i]) != 1) {
      errx(EXIT_FAILURE, "Error while parsing WO at %ld", i);
      network_free(network);
    }
  }
  fscanf(file, "%*[\n;]");
  for (size_t i = 0; i < n_output; i++) {
    if (fscanf(file, "%lf;", &network->biases_output[i]) != 1) {
      errx(EXIT_FAILURE, "Error while parsing WO at %ld", i);
      network_free(network);
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
  printf("    rankdir=LR;\n");     // Left to right layout
  printf("    splines=false;\n");  // Left to right layout

  // Input nodes
  printf("    subgraph cluster_input {\n");
  printf("        label=\"Input Layer\";\n");
  for (size_t i = 0; i < net->n_inputs; i++) {
    printf(
        "        input%zu [label=\"Input %zu\", shape=circle, color=blue];\n",
        i, i);
  }
  printf("    }\n");

  // Hidden layer nodes with biases
  printf("    subgraph cluster_hidden {\n");
  printf("        label=\"Hidden Layer\";\n");
  for (size_t j = 0; j < net->n_hidden; j++) {
    printf(
        "        hidden%zu [label=\"Hidden %zu\\nb=%.2f\", shape=circle, "
        "color=green];\n",
        j, j, net->biases_hidden[j]);
  }
  printf("    }\n");

  // Output layer nodes with biases
  printf("    subgraph cluster_output {\n");
  printf("        label=\"Output Layer\";\n");
  for (size_t k = 0; k < net->n_outputs; k++) {
    printf(
        "        output%zu [label=\"Output %zu\\nb=%.2f\", shape=circle, "
        "color=red];\n",
        k, k, net->biases_output[k]);
  }
  printf("    }\n");

  // Connections from input layer to hidden layer
  for (size_t i = 0; i < net->n_inputs; i++) {
    for (size_t j = 0; j < net->n_hidden; j++) {
      double weight = net->weights_hidden[i * net->n_hidden + j];
      printf("    input%zu -> hidden%zu [label=\"%.2f\"];\n", i, j, weight);
    }
  }

  // Connections from hidden layer to output layer
  for (size_t j = 0; j < net->n_hidden; j++) {
    for (size_t k = 0; k < net->n_outputs; k++) {
      double weight = net->weights_output[j * net->n_outputs + k];
      printf("    hidden%zu -> output%zu [label=\"%.2f\"];\n", j, k, weight);
    }
  }

  printf("}\n");
}
