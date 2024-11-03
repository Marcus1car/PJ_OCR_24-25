#ifndef MATRIX_H
#define MATRIX_H

#include <err.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Matrix {
    int h;
    int w;
    double *values;
} Matrix;

Matrix *init_matrix(int height, int width);

void free_matrix(Matrix *M);

void matrix_setvalue(Matrix *M, int i, int j, double value);

double matrix_getvalue(Matrix *M, int i, int j);

Matrix *copy_matrix(Matrix *M);

Matrix *matrix_multiply(Matrix *M1, Matrix *M2);

Matrix *matrix_transpose(Matrix *M);

Matrix *matrix_invert(Matrix *M);

void print_matrix(Matrix *M);

#endif