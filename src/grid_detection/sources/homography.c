#include "../headers/homography.h"

Matrix *findhomography(double *src, double size) {
    double x1 = src[0];
    double y1 = src[1];
    double x2 = src[2];
    double y2 = src[3];
    double x3 = src[4];
    double y3 = src[5];
    double x4 = src[6];
    double y4 = src[7];

    Matrix *A = init_matrix(8, 8);

    matrix_setvalue(A, 0, 2, 1);
    matrix_setvalue(A, 1, 5, 1);
    matrix_setvalue(A, 2, 0, size);
    matrix_setvalue(A, 2, 2, 1);
    matrix_setvalue(A, 2, 6, -size * x2);
    matrix_setvalue(A, 3, 3, size);
    matrix_setvalue(A, 3, 5, 1);
    matrix_setvalue(A, 3, 6, -size * y2);
    matrix_setvalue(A, 4, 1, size);
    matrix_setvalue(A, 4, 2, 1);
    matrix_setvalue(A, 4, 7, -size * x3);
    matrix_setvalue(A, 5, 4, size);
    matrix_setvalue(A, 5, 5, 1);
    matrix_setvalue(A, 5, 7, -size * y3);
    matrix_setvalue(A, 6, 0, size);
    matrix_setvalue(A, 6, 1, size);
    matrix_setvalue(A, 6, 2, 1);
    matrix_setvalue(A, 6, 6, -size * x4);
    matrix_setvalue(A, 6, 7, -size * x4);
    matrix_setvalue(A, 7, 3, size);
    matrix_setvalue(A, 7, 4, size);
    matrix_setvalue(A, 7, 5, 1);
    matrix_setvalue(A, 7, 6, -size * y4);
    matrix_setvalue(A, 7, 7, -size * y4);

    Matrix *B = init_matrix(8, 1);

    matrix_setvalue(B, 0, 0, x1);
    matrix_setvalue(B, 1, 0, y1);
    matrix_setvalue(B, 2, 0, x2);
    matrix_setvalue(B, 3, 0, y2);
    matrix_setvalue(B, 4, 0, x3);
    matrix_setvalue(B, 5, 0, y3);
    matrix_setvalue(B, 6, 0, x4);
    matrix_setvalue(B, 7, 0, y4);

    Matrix *A_t = matrix_transpose(A);

    Matrix *MULT1 = matrix_multiply(A_t, A);

    Matrix *INV_MULT1 = matrix_invert(MULT1);

    Matrix *MULT2 = matrix_multiply(A_t, B);

    Matrix *R = matrix_multiply(INV_MULT1, MULT2);

    free_matrix(A);
    free_matrix(B);
    free_matrix(A_t);
    free_matrix(MULT1);
    free_matrix(MULT2);
    free_matrix(INV_MULT1);

    return R;
}

SDL_Surface *transformimage(Matrix *transform, SDL_Surface *image,
                            double size) {
    double a = matrix_getvalue(transform, 0, 0);
    double b = matrix_getvalue(transform, 1, 0);
    double c = matrix_getvalue(transform, 2, 0);
    double d = matrix_getvalue(transform, 3, 0);
    double e = matrix_getvalue(transform, 4, 0);
    double f = matrix_getvalue(transform, 5, 0);
    double g = matrix_getvalue(transform, 6, 0);
    double h = matrix_getvalue(transform, 7, 0);

    SDL_Surface *output = SDL_CreateRGBSurface(
        0, (int)size, (int)size, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0);

    for (int y = 0; y < (int)size; y++) {
        double sxPre1 = b * (double)y + c;
        double sxPre2 = h * (double)y + 1;
        double syPre1 = e * (double)y + f;
        double syPre2 = h * (double)y + 1;

        for (int x = 0; x < (int)size; x++) {
            double sx =
                floor((a * (double)x + sxPre1) / (g * (double)x + sxPre2));
            double sy =
                floor((d * (double)x + syPre1) / (g * (double)x + syPre2));

            // Clamp values
            if (sx < 0)
                sx = 0;

            if (sy < 0)
                sy = 0;

            if (sx > (double)image->w - 1)
                sx = (double)image->w - 1;

            if (sy > (double)image->h - 1)
                sy = (double)image->h - 1;

            // printf("(sx, sy) = (%lf, %lf)\n", sx, sy);

            Uint32 pix = get_pixel(image, (unsigned int)sx, (unsigned int)sy);
            Uint8 color;
            SDL_GetRGB(pix, image->format, &color, &color, &color);

            put_pixel(output, (unsigned int)x, (unsigned int)y,
                      SDL_MapRGB(output->format, color, color, color));
        }
    }
    return output;
}
