#include "neural.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define ITERS 100000

int main()
{
    Network *network = network_init(2, 2, 6, SIGMOID, SIGMOID);
    Trainer *trainer = trainer_init(network);
    double inputs[4][2] = {
        {0, 0},
        {0, 1},
        {1, 0},
        {1, 1}};
    double outputs[4][6] = {
        // XOR XNOR OR AND NOR NAND
        {0, 1, 0, 0, 1, 1},
        {1, 0, 1, 0, 0, 1},
        {1, 0, 1, 0, 0, 1},
        {0, 1, 1, 1, 0, 0}};

    for (size_t i = 0; i < ITERS; i++)
    {
        double *input = inputs[i % 4];
        double *output = outputs[i % 4];

        trainer_train(trainer, network, input, output, 20);
        // printf("Training iter %u, XOR %lf %lf = %lf\n", i, input[0], input[1], network->output[0]);
    }

    printf(
        "Result after %d iterations\n\tXOR\tXNOR\tOR\tAND\tNOR\tNAND\n",
        ITERS);
    for (size_t i = 0; i < 4; i++)
    {
        double *input = inputs[i % 4];
        network_predict(network, input);
        printf(
            "%.0f,%.0f = \t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\n",
            input[0],
            input[1],
            network->output[0],
            network->output[1],
            network->output[2],
            network->output[3],
            network->output[4],
            network->output[5]);
    }

    print_network(network);
    trainer_free(trainer);
    network_free(network);
    return EXIT_SUCCESS;
}