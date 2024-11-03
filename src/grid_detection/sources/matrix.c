#include "../headers/matrix.h"

Matrix *init_matrix(int height, int width) {
    Matrix *M = malloc(sizeof(Matrix));
    M->h = height;
    M->w = width;
    M->values = calloc(height * width, sizeof(double));

    return M;
}

void free_matrix(Matrix *M) {
    free(M->values);
    M->values = NULL;
    free(M);
    M = NULL;
}

void matrix_setvalue(Matrix *M, int i, int j, double value) {
    if (M == NULL)
        printf("Error: Cannot set matrix value -> empty matrix given.\n");
    else if ((i >= 0 && i < M->h) && (j >= 0 && j < M->w))
        M->values[i * M->w + j] = value;
    else {
        printf("Error: Cannot set matrix value -> index out of bound.\n");
    }
}

double matrix_getvalue(Matrix *M, int i, int j) {
    if (M == NULL) {
        printf("Error: Cannot get matrix value -> empty matrix given.\n");
        return 0;
    }
    if ((i >= 0 && i < M->h) && (j >= 0 && j < M->w)) {
        return M->values[i * M->w + j];
    } else {
        printf("Error: Cannot get matrix value -> index out of bound.\n");
        return 0;
    }
}

Matrix *copy_matrix(Matrix *M) {
    Matrix *C = init_matrix(M->h, M->w);
    for (int i = 0; i < M->h; i++) {
        for (int j = 0; j < M->w; j++) {
            matrix_setvalue(C, i, j, matrix_getvalue(M, i, j));
        }
    }

    return C;
}

Matrix *matrix_multiply(Matrix *M1, Matrix *M2) {
    if (M1 == NULL || M2 == NULL) {
        printf("Error: Cannot multiply the matrices -> empty matrix given.\n");
        return NULL;
    }
    if (M1->w == M2->h) {
        Matrix *MR = init_matrix(M1->h, M2->w);

        for (int i = 0; i < M1->h; i++) {
            for (int j = 0; j < M2->w; j++) {
                double sum = 0;
                for (int k = 0; k < M1->w; k++)
                    sum +=
                        matrix_getvalue(M1, i, k) * matrix_getvalue(M2, k, j);

                matrix_setvalue(MR, i, j, sum);
            }
        }

        return MR;
    } else {
        printf("Error: Cannot multiply the matrices -> wrong size.\n");
        return NULL;
    }
}

Matrix *matrix_transpose(Matrix *M) {
    if (M == NULL) {
        printf("Error: Cannot transpose the matrix -> empty matrix given.\n");
        return NULL;
    }

    Matrix *MR = init_matrix(M->w, M->h);
    for (int i = 0; i < MR->h; i++) {
        for (int j = 0; j < MR->w; j++) {
            matrix_setvalue(MR, i, j, matrix_getvalue(M, j, i));
        }
    }
    return MR;
}

Matrix *matrix_invert(Matrix *M) {
    if (M == NULL) {
        printf("Error: Cannot invert matrix -> empty matrix given.\n");
        return NULL;
    }
    if (M->h == M->w) {
        double i, j, k;

        Matrix *C = copy_matrix(M);
        Matrix *MR = init_matrix(M->h, M->h);

        for (int i = 0; i < M->h; i++)
            matrix_setvalue(MR, i, i, 1);

        for (k = 0; k < M->h; k++) {
            double temp = matrix_getvalue(C, k, k);
            for (j = 0; j < M->h; j++) {
                matrix_setvalue(C, k, j, matrix_getvalue(C, k, j) / temp);
                matrix_setvalue(MR, k, j, matrix_getvalue(MR, k, j) / temp);
            }

            for (i = 0; i < M->h; i++) {
                temp = matrix_getvalue(C, i, k);
                for (j = 0; j < M->h; j++) {
                    if (i == k)
                        break;

                    matrix_setvalue(C, i, j,
                                    matrix_getvalue(C, i, j) -
                                        matrix_getvalue(C, k, j) * temp);
                    matrix_setvalue(MR, i, j,
                                    matrix_getvalue(MR, i, j) -
                                        matrix_getvalue(MR, k, j) * temp);
                }
            }
        }

        free_matrix(C);
        return MR;
    } else {
        printf("Error: Cannot invert matrix -> non squared matrix given.\n");
        return NULL;
    }
}

void print_matrix(Matrix *M) {
    for (int i = 0; i < M->h; i++) {
        for (int j = 0; j < M->w; j++) {
            printf("%lf\t", matrix_getvalue(M, i, j));
        }
        printf("\n");
    }
}