#include "neural.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <omp.h>

double sigmoid(double f) { return 1.0 / (1.0 + exp(-f)); }
double d_sigmoid(double f) { return f * (1.0 - f); }
double relu(double f) { return f > 0 ? f : 0; }
double d_relu(double f) { return f > 0 ? 1 : 0; }
double elu(double f) { return f > 0 ? f : 0.3 * (exp(f) - 1); }
double d_elu(double f) { return f > 0 ? 1 : 0.3 * (exp(f)); }
double lrelu(double f) { return f > 0 ? f : 0.01 * f; }
double d_lrelu(double f) { return f > 0 ? 1 : 0.01; }
double tanh_(double f) {return tanh(f);}
double d_tanh(double f) {double t = tanh(f); return fma(t,t,-1);} 
//fma(x,y,z) = x*y+z without losing precision

Network *network_init(
    size_t n_inputs,
    size_t n_hidden,
    size_t n_outputs,
    ActivationFunction activation_hidden,
    ActivationFunction activation_output)
{
    Network *network = (Network *)malloc(sizeof(Network));
    if (network == NULL)
    {
        errx(EXIT_FAILURE, "Memory allocation failed");
    }

    if (activation_hidden == SOFTMAX)
    {
        errx(EXIT_FAILURE, "Softmax on hidden layer is not possible");
    }

    network->n_inputs = n_inputs;
    network->n_hidden = n_hidden;
    network->n_outputs = n_outputs;

    network->ouput_activation = activation_output;
    network->hidden_activation = activation_hidden;

    network->weights_hidden = calloc(
        n_inputs * n_hidden,
        sizeof(*network->weights_hidden));
    network->biases_hidden = calloc(
        n_hidden,
        sizeof(*network->biases_hidden));
    network->weights_output = calloc(
        n_hidden * n_outputs,
        sizeof(*network->weights_output));
    network->biases_output = calloc(
        n_outputs,
        sizeof(*network->biases_output));
    network->hidden = calloc(
        n_hidden,
        sizeof(*network->hidden));
    network->output = calloc(
        n_outputs,
        sizeof(*network->output));

    if (
        network->output == NULL ||
        network->hidden == NULL ||
        network->biases_hidden == NULL ||
        network->biases_output == NULL ||
        network->weights_hidden == NULL ||
        network->weights_output == NULL)
    {
        errx(EXIT_FAILURE, "Memory allocation failed");
    }

    // initialize everything but the biases
    for (size_t i = 0; i < n_inputs * n_hidden; i++)
    {
        // network->weights_hidden[i] = ((double)rand() / RAND_MAX) * 2 - 1;
        network->weights_hidden[i] = ((double)rand() / (RAND_MAX / 2) - 1.) / 2.;
    }

    for (size_t i = 0; i < n_hidden * n_outputs; i++)
    {
        // network->weights_output[i] = ((double)rand() / RAND_MAX) * 2 - 1;
        network->weights_output[i] = ((double)rand() / (RAND_MAX / 2) - 1.) / 2.;

    } /*
     for (size_t i = 0; i < n_outputs; i++) {
         network->output[i] = ((double)rand() / RAND_MAX) * 2 - 1;
     }*/

    switch (network->hidden_activation)
    {
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

    switch (network->ouput_activation)
    {
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

void network_free(Network *network)
{
    free(network->weights_hidden);
    free(network->biases_hidden);
    free(network->weights_output);
    free(network->biases_output);
    free(network->hidden);
    free(network->output);
    free(network);
    network = NULL;
}

void network_predict(Network *network, double *input)
{
    for (size_t c = 0; c < network->n_hidden; c++)
    {
        double sum = 0;
        for (size_t r = 0; r < network->n_inputs; r++)
        {
            sum += input[r] * network->weights_hidden[r * network->n_hidden + c];
        }

        /*
        switch (network->hidden_activation)
        {
            case SIGMOID:
                network->hidden[c] = sigmoid(sum + network->biases_hidden[c]);
                break;
            case RELU:
                network->hidden[c] = relu(sum + network->biases_hidden[c]);
                break;
            case LRELU:
                network->hidden[c] = lrelu(sum + network->biases_hidden[c]);
                break;
            case ELU:
                network->hidden[c] = elu(sum + network->biases_hidden[c]);
                break;
            default:
                break;
        }*/
        network->hidden[c] = (*network->hidden_fct)(sum + network->biases_hidden[c]);
        // network->hidden[c] = elu(sum + network->biases_hidden[c]);
    }

    /// Following 4 lines for sigmoid on ouput
    /*
    for (size_t c = 0; c < network->n_outputs; c++) {
        double sum = 0;
        for (size_t r = 0; r < network->n_hidden; r++) {
            sum += network->hidden[r] * network->weights_output[r * network->n_outputs + c];
        }

        network->output[c] = sigmoid(sum + network->biases_output[c]);
    }*/
    ////

    if (network->ouput_activation == SOFTMAX)
    {
        for (size_t c = 0; c < network->n_outputs; c++)
        {
            double sum = 0.0;
            for (size_t r = 0; r < network->n_hidden; r++)
            {
                sum += network->hidden[r] * network->weights_output[r * network->n_outputs + c];
            }
            network->output[c] = sum + network->biases_output[c];
        }

        double max_output = network->output[0];
        for (size_t i = 1; i < network->n_outputs; i++)
        {
            if (network->output[i] > max_output)
            {
                max_output = network->output[i];
            }
        }

        // Compute the exponentials and sum them
        double sum_exp = 0;
        for (size_t i = 0; i < network->n_outputs; i++)
        {
            network->output[i] = exp(network->output[i] - max_output);
            sum_exp += network->output[i];
        }

        // Normalize by dividing each by the sum of exponentials
        for (size_t i = 0; i < network->n_outputs; i++)
        {
            network->output[i] /= sum_exp;
        }
    } else {
        for (size_t c = 0; c < network->n_outputs; c++)
        {
            double sum = 0;
            for (size_t r = 0; r < network->n_hidden; r++)
            {
                sum += network->hidden[r] * network->weights_output[r * network->n_outputs + c];
            }

            network->output[c] = (*network->output_fct)(sum + network->biases_output[c]);
        }
    }
}

/* trainer */

void is_network_dead(Network *network)
{
    const double threshold = 0.00001;
    char hidden_dead = 1;
    char output_dead = 1;
    for (size_t k = 0; k < network->n_hidden; k++)
    {
        if (fabs(network->hidden[k]) > threshold)
        {
            hidden_dead = 0;
            printf("ici");
            break;
        }
    }
    for (size_t k = 0; k < network->n_outputs; k++)
    {
        if (fabs(network->output[k]) > threshold)
        {
            output_dead = 0;
            printf("ici");

            break;
        }
    }

    if (hidden_dead == 1)
    {
        printf("\033[31m\033[1mHIDDEN LAYER DEAD\033[0m\n");
    }
    if (output_dead == 1)
    {
        printf("\033[31m\033[1mOUT LAYER DEAD\033[0m\n");
    }
}

Trainer *trainer_init(Trainer *trainer, Network *network)
{
    trainer->grad_hidden = calloc(network->n_hidden, sizeof(*trainer->grad_hidden));
    trainer->grad_output = calloc(network->n_outputs, sizeof(*trainer->grad_output));
    return trainer;
}

void trainer_train(Trainer *trainer, Network *network, double *input, double *target, double lr)
{
    network_predict(network, input);
    // is_network_dead(network);

    for (size_t c = 0; c < network->n_outputs; c++)
    {
        // trainer->grad_output[c] = (network->output[c] - target[c]) * sigmoid_prim(network->output[c]);
        if(network->ouput_activation == SOFTMAX) trainer->grad_output[c] = network->output[c] - target[c];
        else trainer->grad_output[c] = (network->output[c] - target[c]) * (*network->d_output_fct)(network->output[c]);
    }

    for (size_t r = 0; r < network->n_hidden; r++)
    {
        double sum = 0.0;
        for (size_t c = 0; c < network->n_outputs; c++)
        {
            sum += trainer->grad_output[c] * network->weights_output[r * network->n_outputs + c];
        }

        trainer->grad_hidden[r] = sum * (*network->d_hidden_fct)(network->hidden[r]);
    }

    for (size_t r = 0; r < network->n_hidden; r++)
    {
        for (size_t c = 0; c < network->n_outputs; c++)
        {
            network->weights_output[r * network->n_outputs + c] -= lr * trainer->grad_output[c] * network->hidden[r];
        }
    }

    for (size_t r = 0; r < network->n_inputs; r++)
    {
        for (size_t c = 0; c < network->n_hidden; c++)
        {
            network->weights_hidden[r * network->n_hidden + c] -= lr * trainer->grad_hidden[c] * input[r];
        }
    }

    for (size_t c = 0; c < network->n_outputs; c++)
    {
        network->biases_output[c] -= lr * trainer->grad_output[c];
    }

    for (size_t c = 0; c < network->n_hidden; c++)
    {
        network->biases_hidden[c] -= lr * trainer->grad_hidden[c];
    }
}

void trainer_free(Trainer *trainer)
{
    free(trainer->grad_hidden);
    free(trainer->grad_output);
}

void print_network(const Network *network)
{
    printf("weights hidden:\n");
    for (size_t i = 0; i < network->n_inputs; i++)
    {
        for (size_t j = 0; j < network->n_hidden; j++)
        {
            printf(" %9.6f", network->weights_hidden[network->n_inputs * i + j]);
        }

        printf("\n");
    }

    printf("biases hidden:\n");
    for (size_t i = 0; i < network->n_hidden; i++)
    {
        printf(" %9.6f", network->biases_hidden[i]);
    }

    printf("\n");

    printf("weights output:\n");
    for (size_t i = 0; i < network->n_hidden; i++)
    {
        for (size_t j = 0; j < network->n_outputs; j++)
        {
            printf(" %9.6f", network->weights_output[i * network->n_outputs + j]);
        }

        printf("\n");
    }

    printf("biases output:\n");
    for (size_t i = 0; i < network->n_outputs; i++)
    {
        printf(" %9.6f", network->biases_output[i]);
    }

    printf("\n");
}

void save_nn_data(Network *network, const char *path)
{

    // size_t length = snprintf( NULL, 0, "%d;%d;%d",  network->n_inputs, network->n_hidden, network->n_outputs);
    // char* fst_line = calloc(length+1, sizeof(char));
    // snprintf( fst_line, length+1, );
    FILE *fptr;
    fptr = fopen(path, "w");
    if (fptr == NULL)
        fprintf(stderr, "Error while opening file to save weights");
    printf("COUCOU1;;;;;;\n");

    fprintf(fptr, "%ld;%ld;%ld\n", network->n_inputs, network->n_hidden, network->n_outputs);
    printf("COUCOU2232323232;;;;;;\n");

    for (size_t i = 0; i < network->n_inputs; i++)
    {
        for (size_t j = 0; j < network->n_hidden; j++)
        {
            printf("%ld,%ld\n", i, j);
            fprintf(fptr, "%9.6f;", network->weights_hidden[network->n_inputs * j + i]);
        }
    }
    printf("COUCOU12;;;;;;\n");

    fprintf(fptr, "\n");
    for (size_t i = 0; i < network->n_hidden; i++)
    {
        fprintf(fptr, "%9.6f;", network->biases_hidden[i]);
    }
    printf("COUCOU123;;;;;;\n");

    fprintf(fptr, "\n");
    for (size_t i = 0; i < network->n_hidden * network->n_outputs; i++)
    {
        fprintf(fptr, "%9.6f;", network->weights_output[i]);
    }
    printf("COUCOU1234;;;;;;\n");

    fprintf(fptr, "\n");
    for (size_t i = 0; i < network->n_outputs; i++)
    {
        fprintf(fptr, "%9.6f;", network->biases_output[i]);
    }
    fprintf(fptr, "\n");
    printf("COUCOU;;;;;;\n");
    fclose(fptr);
}

void load_nn_data(Network *network, const char *path)
{
}
