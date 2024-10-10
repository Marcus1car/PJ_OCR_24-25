#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    int input_size;
    int hidden_size;
    int output_size;
    double *weights_input_hidden;
    double *weights_hidden_output;
    double *bias_hidden;
    double *bias_output;
} NeuralNetwork;


NeuralNetwork* create_network(int input_size, int hidden_size, int output_size) {
    NeuralNetwork *nn = (NeuralNetwork *)malloc(sizeof(NeuralNetwork));
    nn->input_size = input_size;
    nn->hidden_size = hidden_size;
    nn->output_size = output_size;

    // Allocate memory for weights and biases
    nn->weights_input_hidden = (double *)malloc(input_size * hidden_size * sizeof(double));
    nn->weights_hidden_output = (double *)malloc(hidden_size * output_size * sizeof(double));
    nn->bias_hidden = (double *)malloc(hidden_size * sizeof(double));
    nn->bias_output = (double *)malloc(output_size * sizeof(double));

    // Initialize weights and biases with random values
    for (int i = 0; i < input_size * hidden_size; i++) {
        nn->weights_input_hidden[i] = ((double)rand() / RAND_MAX) * 2 - 1; // Random values between -1 and 1
    }
    for (int i = 0; i < hidden_size * output_size; i++) {
        nn->weights_hidden_output[i] = ((double)rand() / RAND_MAX) * 2 - 1;
    }
    for (int i = 0; i < hidden_size; i++) {
        nn->bias_hidden[i] = 0; // Initialize biases to zero
    }
    for (int i = 0; i < output_size; i++) {
        nn->bias_output[i] = 0;
    }

    return nn;
}


double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

double relu(double x) {
    return x > 0 ? x : 0;
}


void forward(NeuralNetwork *nn, double *input, double *output) {
    double *hidden_layer = (double *)malloc(nn->hidden_size * sizeof(double));

    // Calculate hidden layer activations
    for (int i = 0; i < nn->hidden_size; i++) {
        hidden_layer[i] = nn->bias_hidden[i];
        for (int j = 0; j < nn->input_size; j++) {
            hidden_layer[i] += input[j] * nn->weights_input_hidden[j * nn->hidden_size + i];
        }
        hidden_layer[i] = sigmoid(hidden_layer[i]); // Apply activation function
    }

    // Calculate output layer activations
    for (int i = 0; i < nn->output_size; i++) {
        output[i] = nn->bias_output[i];
        for (int j = 0; j < nn->hidden_size; j++) {
            output[i] += hidden_layer[j] * nn->weights_hidden_output[j * nn->output_size + i];
        }
        output[i] = sigmoid(output[i]); // Apply activation function
    }

    free(hidden_layer);
}

void backward(NeuralNetwork *nn, double *input, double *output, double *target, double learning_rate) {
    double *hidden_layer = (double *)malloc(nn->hidden_size * sizeof(double));
    double *output_error = (double *)malloc(nn->output_size * sizeof(double));
    double *hidden_error = (double *)malloc(nn->hidden_size * sizeof(double));

    // Calculate hidden layer activations
    for (int i = 0; i < nn->hidden_size; i++) {
        hidden_layer[i] = nn->bias_hidden[i];
        for (int j = 0; j < nn->input_size; j++) {
            hidden_layer[i] += input[j] * nn->weights_input_hidden[j * nn->hidden_size + i];
        }
        hidden_layer[i] = relu(hidden_layer[i]);
    }

    // Calculate output layer error
    for (int i = 0; i < nn->output_size; i++) {
        output_error[i] = target[i] - output[i];
    }

    // Calculate hidden layer error
    for (int i = 0; i < nn->hidden_size; i++) {
        hidden_error[i] = 0;
        for (int j = 0; j < nn->output_size; j++) {
            hidden_error[i] += output_error[j] * nn->weights_hidden_output[i * nn->output_size + j];
        }
    }

    // Update output layer weights and biases
    for (int i = 0; i < nn->output_size; i++) {
        for (int j = 0; j < nn->hidden_size; j++) {
            nn->weights_hidden_output[j * nn->output_size + i] += learning_rate * output_error[i] * hidden_layer[j];
        }
        nn->bias_output[i] += learning_rate * output_error[i];
    }

    // Update hidden layer weights and biases
    for (int i = 0; i < nn->hidden_size; i++) {
        for (int j = 0; j < nn->input_size; j++) {
            nn->weights_input_hidden[j * nn->hidden_size + i] += learning_rate * hidden_error[i] * input[j];
        }
        nn->bias_hidden[i] += learning_rate * hidden_error[i];
    }

    free(hidden_layer);
    free(output_error);
    free(hidden_error);
}

void train(NeuralNetwork *nn, double **inputs, double **targets, int num_samples, int epochs, double learning_rate) {
    double *output = (double *)malloc(nn->output_size * sizeof(double));
    
    for (int epoch = 0; epoch < epochs; epoch++) {
        for (int i = 0; i < num_samples; i++) {
            forward(nn, inputs[i], output);
            backward(nn, inputs[i], output, targets[i], learning_rate);
            printf("Step %d,%d: Target: %lf, Actual %f\n", epoch, i, targets[i][0], *output);
        }
    }

    free(output);
}


int main() {
    int input_size = 2;
    int hidden_size = 2;
    int output_size = 1;
    int num_samples = 4;

    // Create the neural network
    NeuralNetwork *nn = create_network(input_size, hidden_size, output_size);

    // Define training data for XOR problem
    double *inputs[4];
    double *targets[4];
    for (int i = 0; i < num_samples; i++) {
        inputs[i] = (double *)malloc(input_size * sizeof(double));
        targets[i] = (double *)malloc(output_size * sizeof(double));
    }
    inputs[0][0] = 0; inputs[0][1] = 0; targets[0][0] = 0;
    inputs[1][0] = 0; inputs[1][1] = 1; targets[1][0] = 1;
    inputs[2][0] = 1; inputs[2][1] = 0; targets[2][0] = 1;
    inputs[3][0] = 1; inputs[3][1] = 1; targets[3][0] = 0;

    // Train the network
    train(nn, inputs, targets, num_samples, 100000, 10);

    // Test the network
    double output[1];
    for (int i = 0; i < num_samples; i++) {
        forward(nn, inputs[i], output);
        printf("Input: %f %f, Expected: %f, Predicted: %f\n", inputs[i][0], inputs[i][1], targets[i][0], output[0]);
    }
    int correct_predictions = 0;
    for (int i = 0; i < num_samples; i++) {
        forward(nn, inputs[i], output);
        int predicted = output[0] > 0.5 ? 1 : 0; // Assuming threshold of 0.5
        if (predicted == (int)targets[i][0]) {
            correct_predictions++;
        }
    }
    double accuracy = (double)correct_predictions / num_samples * 100;
    printf("Accuracy: %.2f%%\n", accuracy);

    // Free dynamically allocated memory
    for (int i = 0; i < num_samples; i++) {
        free(inputs[i]);
        free(targets[i]);
    }
    free(nn->weights_input_hidden);
    free(nn->weights_hidden_output);
    free(nn->bias_hidden);
    free(nn->bias_output);
    free(nn);

    return 0;
}
