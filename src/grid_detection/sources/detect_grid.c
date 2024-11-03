#include "../headers/detect_grid.h"

SDL_Surface *detect_grid(const char *file) {
    SDL_Surface *image = IMG_Load(file);
    SDL_Surface *segmap = SDL_CreateRGBSurface(
        0, image->w, image->h, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0);

    size_t top, right, left, bottom;
    size_t topleftX, topleftY, toprightX, toprightY, bottomleftX, bottomleftY,
        bottomrightX, bottomrightY;

    SDL_Surface *croppedimage = NULL, *croppedsegmap = NULL;

    size_t *coords[] = {&top,         &right,        &left,
                        &bottom,      &topleftX,     &topleftY,
                        &toprightX,   &toprightY,    &bottomleftX,
                        &bottomleftY, &bottomrightX, &bottomrightY};

    SDL_Surface **surf[] = {&image, &segmap, &croppedimage, &croppedsegmap};

    size_t histo_size = 1;
    CCTuple *histo = twopassSegmentation(image, segmap, &histo_size);

    size_t gridval = findgrid(histo, histo_size, coords, surf);

    free(histo);

    remove_grid(croppedimage, croppedsegmap, gridval, 1);

    double size = fmax(croppedimage->h, croppedimage->w);

    double *val = malloc(sizeof(double) * 8);

    val[0] = (double)topleftX;
    val[1] = (double)topleftY;
    val[2] = (double)toprightX;
    val[3] = (double)toprightY;
    val[4] = (double)bottomleftX;
    val[5] = (double)bottomleftY;
    val[6] = (double)bottomrightX;
    val[7] = (double)bottomrightY;

    Matrix *transform = findhomography(val, size);

    SDL_Surface *transformedimage =
        transformimage(transform, croppedimage, size);

    free(val);
    free_matrix(transform);

    SDL_FreeSurface(croppedimage);
    SDL_FreeSurface(croppedsegmap);
    SDL_FreeSurface(segmap);
    SDL_FreeSurface(image);

    return transformedimage;
}

size_t findgrid(CCTuple *histo, size_t histo_size, size_t *coords[],
                SDL_Surface **surf[]) {
    size_t *top = coords[0], *right = coords[1], *left = coords[2],
           *bottom = coords[3], *topleftX = coords[4], *topleftY = coords[5],
           *toprightX = coords[6], *toprightY = coords[7],
           *bottomleftX = coords[8], *bottomleftY = coords[9],
           *bottomrightX = coords[10], *bottomrightY = coords[11];

    SDL_Surface **image = surf[0], **segmap = surf[1], **croppedimage = surf[2],
                **croppedsegmap = surf[3];

    size_t i = histo_size - 1;
    size_t best_index = i;

    size_t limit = (size_t)(*image)->h * (*image)->w / 250;
    size_t area_max = 0;

    size_t best_coords[8] = {0};

    SDL_Surface *max_cropped_img = NULL;
    SDL_Surface *max_cropped_segmap = NULL;

    do {
        size_t current_label = histo[i].label;

        // Get coordinates
        find_extremity_coordinates(*segmap, current_label, top, right, left,
                                   bottom);
        *croppedsegmap = cut_segmap(*segmap, *top, *bottom, *left, *right);
        find_corner_coordinates(*croppedsegmap, current_label, topleftX,
                                topleftY, toprightX, toprightY, bottomleftX,
                                bottomleftY, bottomrightX, bottomrightY);

        size_t area = (size_t)(abs((int)*bottomrightX - (int)*topleftX)) *
                          abs(((int)*bottomleftY - (int)*toprightY)) +
                      abs(((int)*toprightX - (int)*bottomleftX)) *
                          abs(((int)*bottomrightY - (int)*topleftY));

        printf("area for component n°%lu = %lu\n", i, area);

        if (area > area_max) {
            area_max = area;
            best_index = i;

            best_coords[0] = *topleftX;
            best_coords[1] = *topleftY;
            best_coords[2] = *toprightX;
            best_coords[3] = *toprightY;
            best_coords[4] = *bottomleftX;
            best_coords[5] = *bottomleftY;
            best_coords[6] = *bottomrightX;
            best_coords[7] = *bottomrightY;

            SDL_FreeSurface(max_cropped_img);
            SDL_FreeSurface(max_cropped_segmap);

            max_cropped_img = cut_image(*image, *top, *bottom, *left, *right);
            max_cropped_segmap = *croppedsegmap;
        } else {
            SDL_FreeSurface(*croppedsegmap);
        }

        i -= 1;

    } while (i > 0 && histo[i].nb_pixels > limit);

    *croppedimage = max_cropped_img;
    *croppedsegmap = max_cropped_segmap;

    if (i == 0) {
        errx(1, "Error: no grid is found in the given image.");
    }

    printf("limit = %lu\n", limit);
    printf("LABEL -> %zu\n", histo[best_index].label);
    printf("NB PIXELS -> %zu\n", histo[best_index].nb_pixels);

    *topleftX = best_coords[0];
    *topleftY = best_coords[1];
    *toprightX = best_coords[2];
    *toprightY = best_coords[3];
    *bottomleftX = best_coords[4];
    *bottomleftY = best_coords[5];
    *bottomrightX = best_coords[6];
    *bottomrightY = best_coords[7];

    return histo[best_index].label;
}

int get_distance(int X1, int Y1, int X2, int Y2) {
    int d1 = abs(X1 - X2);
    int d2 = abs(Y1 - Y2);

    int distance = (d1 * d1) + (d2 * d2);
    distance = (int)round(sqrt((double)distance));

    return distance;
}