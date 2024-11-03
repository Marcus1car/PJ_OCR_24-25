#include "../headers/tuple.h"

Tuple *createTuple(size_t *values, size_t num_values) {
    Tuple *tuple = malloc(sizeof(Tuple));
    tuple->num_values = num_values;
    tuple->values = values;
    return tuple;
}

void freeTuple(Tuple *tuple) {
    if (tuple == NULL) {
        exit(EXIT_FAILURE);
    }
    if (tuple->values == NULL) {
        exit(EXIT_FAILURE);
    }
    free(tuple->values);
    free(tuple);
}
