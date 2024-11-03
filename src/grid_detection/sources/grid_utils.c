#include "../headers/grid_utils.h"

#define PATH "cells/"

void colorizeimage(SDL_Surface *image, SDL_Surface *segmap) {
    Uint8 r, g, b;
    for (size_t x = 0; x < (size_t)image->w; x++) {
        for (size_t y = 0; y < (size_t)image->h; y++) {
            Uint32 val = get_pixel(segmap, x, y);
            r = (29 * val) % 256;
            g = (53 * val) % 256;
            b = (97 * val) % 256;
            put_pixel(image, x, y, SDL_MapRGB(image->format, r, g, b));
        }
    }
}

void remove_grid(SDL_Surface *image, SDL_Surface *segmap, size_t objval,
                 int b) {
    for (size_t x = 0; x < (size_t)image->w; x++) {
        for (size_t y = 0; y < (size_t)image->h; y++) {
            Uint32 pix = get_pixel(segmap, x, y);
            if ((size_t)pix == objval) {
                if (b) {
                    put_pixel(image, x, y, SDL_MapRGB(image->format, 0, 0, 0));
                }
            } else {
                if (!b) {
                    put_pixel(image, x, y, SDL_MapRGB(image->format, 0, 0, 0));
                }
            }
        }
    }
}

void find_extremity_coordinates(SDL_Surface *segmap, size_t objval, size_t *top,
                                size_t *right, size_t *left, size_t *bottom) {
    int b = 1;
    Uint32 pix;
    for (size_t y = 0; y < (size_t)segmap->h && b; y++) {
        for (size_t x = 0; x < (size_t)segmap->w && b; x++) {
            pix = get_pixel(segmap, x, y);
            if (pix == objval) {
                *top = y;
                b = 0;
            }
        }
    }
    b = 1;
    for (size_t y = (size_t)segmap->h - 1; y < (size_t)segmap->h && b; y--) {
        for (size_t x = 0; x < (size_t)segmap->w && b; x++) {
            pix = get_pixel(segmap, x, y);
            if (pix == objval) {
                *bottom = y;
                b = 0;
            }
        }
    }
    b = 1;
    for (size_t x = 0; x < (size_t)segmap->w && b; x++) {
        for (size_t y = 0; y < (size_t)segmap->h && b; y++) {
            pix = get_pixel(segmap, x, y);
            if (pix == objval) {
                *left = x;
                b = 0;
            }
        }
    }
    b = 1;
    for (size_t x = (size_t)segmap->w - 1; x < (size_t)segmap->w && b; x--) {
        for (size_t y = 0; y < (size_t)segmap->h && b; y++) {
            pix = get_pixel(segmap, x, y);
            if (pix == objval) {
                *right = x;
                b = 0;
            }
        }
    }
}

void find_corner_coordinates(SDL_Surface *segmap, size_t objval,
                             size_t *topleftX, size_t *topleftY,
                             size_t *toprightX, size_t *toprightY,
                             size_t *bottomleftX, size_t *bottomleftY,
                             size_t *bottomrightX, size_t *bottomrightY) {
    size_t min = segmap->h + segmap->w;
    Uint32 pix;

    for (size_t x = 0; x < (size_t)segmap->w / 2; x++) {
        for (size_t y = 0; y < (size_t)segmap->h / 2; y++) {
            pix = get_pixel(segmap, x, y);
            if (pix == objval) {
                if (x + y < min) {
                    min = x + y;
                    *topleftX = x;
                    *topleftY = y;
                }
                break;
            }
        }
    }

    min = segmap->h + segmap->w;

    for (size_t x = 0; x < (size_t)segmap->w / 2; x++) {
        for (size_t y = 0; y < (size_t)segmap->h / 2; y++) {
            pix = get_pixel(segmap, x, segmap->h - 1 - y);
            if (pix == objval) {
                if (x + y < min) {
                    min = x + y;
                    *bottomleftX = x;
                    *bottomleftY = y;
                }
                break;
            }
        }
    }

    *bottomleftY = segmap->h - 1 - *bottomleftY;

    min = segmap->h + segmap->w;

    for (size_t x = 0; x < (size_t)segmap->w / 2; x++) {
        for (size_t y = 0; y < (size_t)segmap->h / 2; y++) {
            pix = get_pixel(segmap, segmap->w - 1 - x, y);
            if (pix == objval) {
                if (x + y < min) {
                    min = x + y;
                    *toprightX = x;
                    *toprightY = y;
                }
                break;
            }
        }
    }

    *toprightX = segmap->w - 1 - *toprightX;

    min = segmap->h + segmap->w;

    for (size_t x = 0; x < (size_t)segmap->w / 2; x++) {
        for (size_t y = 0; y < (size_t)segmap->h / 2; y++) {
            pix = get_pixel(segmap, segmap->w - 1 - x, segmap->h - 1 - y);
            if (pix == objval) {
                if (x + y < min) {
                    min = x + y;
                    *bottomrightX = x;
                    *bottomrightY = y;
                }
            }
        }
    }

    *bottomrightX = segmap->w - 1 - *bottomrightX;
    *bottomrightY = segmap->h - 1 - *bottomrightY;
}

SDL_Surface *cut_segmap(SDL_Surface *image, size_t top, size_t bottom,
                        size_t left, size_t right) {
    SDL_Surface *newsegmap =
        SDL_CreateRGBSurface(0, right - left + 1, bottom - top + 1, 32,
                             0x000000ff, 0x0000ff00, 0x00ff0000, 0);
    for (size_t y = top; y <= bottom; y++) {
        for (size_t x = left; x <= right; x++) {
            Uint32 pix = get_pixel(image, x, y);
            put_pixel(newsegmap, x - left, y - top, pix);
        }
    }
    return newsegmap;
}

SDL_Surface *cut_image(SDL_Surface *image, size_t top, size_t bottom,
                       size_t left, size_t right) {
    SDL_Surface *cell =
        SDL_CreateRGBSurface(0, right - left + 1, bottom - top + 1, 32,
                             0x000000ff, 0x0000ff00, 0x00ff0000, 0);
    for (size_t y = top; y <= bottom; y++) {
        for (size_t x = left; x <= right; x++) {
            Uint32 pix = get_pixel(image, x, y);
            Uint8 color;
            SDL_GetRGB(pix, image->format, &color, &color, &color);
            put_pixel(cell, x - left, y - top,
                      SDL_MapRGB(cell->format, color, color, color));
        }
    }
    return cell;
}

void save_cells(const char *file) {
    SDL_Surface *image = IMG_Load(file);
    char folder_name[] = PATH;
    remove_dir(folder_name);
    mkdir(folder_name, S_IRWXU);

    size_t w = (size_t)image->w;
    size_t h = (size_t)image->h;

    size_t left_bound, right_bound, top_bound, bottom_bound;

    for (size_t x = 0; x < 9; x++) // first for
    {

        size_t mid_cell_x;

        if (x > 0) {
            left_bound = x * w / 9 - w / 45;
            mid_cell_x = w / 18 + w / 45;
            if (x < 8) {
                right_bound = x * w / 9 + w / 9 - 1 + w / 45;
            } else {
                right_bound = x * w / 9 + w / 9 - 1;
            }
        } else {
            left_bound = x * w / 9;
            mid_cell_x = w / 18;
            right_bound = x * w / 9 + w / 9 - 1 + w / 45;
        }

        for (size_t y = 0; y < 9; y++) // second for
        {
            size_t mid_cell_y;

            if (y > 0) {
                top_bound = y * h / 9 - h / 45;
                mid_cell_y = h / 18 + h / 45;
                if (y < 8) {
                    bottom_bound = y * h / 9 + h / 9 - 1 + h / 45;
                } else {
                    bottom_bound = y * h / 9 + h / 9 - 1;
                }
            } else {
                top_bound = y * h / 9;
                mid_cell_y = h / 18;
                bottom_bound = y * h / 9 + h / 9 - 1 + h / 45;
            }

            SDL_Surface *cell = cut_image(image, top_bound, bottom_bound,
                                          left_bound, right_bound);
            SDL_Surface *cellsegmap = SDL_CreateRGBSurface(
                0, right_bound - left_bound + 1, bottom_bound - top_bound + 1,
                32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0);

            size_t limit = (h / 9 * w / 9) / 60;

            size_t X = 0;
            size_t size = 0;
            size_t segval = 0;
            do {
                segval++;
                size = propagate(cell, cellsegmap, segval, mid_cell_x + X,
                                 mid_cell_y);
                size_t size2 = propagate(cell, cellsegmap, segval,
                                         mid_cell_x - X, mid_cell_y);
                if (size2 > size) {
                    size = size2;
                }
                X++;
            } while (size < limit / 6 && X < w / 36);

            if (size > limit) {
                remove_grid(cell, cellsegmap, segval, 0);
                SDL_Surface *emptycell = SDL_CreateRGBSurface(
                    0, 28, 28, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0);
                size_t top, right, left, bottom;
                find_extremity_coordinates(cellsegmap, segval, &top, &right,
                                           &left, &bottom);
                SDL_Surface *croppedcell =
                    cut_image(cell, top, bottom, left, right);

                int max = croppedcell->w;
                if (croppedcell->h > max) {
                    max = croppedcell->h;
                }

                SDL_Surface *inter = SDL_CreateRGBSurface(
                    0, max, max, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0);
                copy_symbol(inter, croppedcell, (max - croppedcell->w) / 2,
                            (max - croppedcell->h) / 2);

                char name[255];
                sprintf(name, "%scell_%lu_%lu", PATH, x, y);

                if (SDL_SaveBMP(inter, name) != 0) {
                    printf("SDL_SaveBMP failed: %s\n", SDL_GetError());
                }

                GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(name, NULL);
                if (pixbuf == NULL) {
                    printf("Failed to resize image\n");
                }

                GdkPixbuf *new = gdk_pixbuf_scale_simple(pixbuf, 22, 22,
                                                         GDK_INTERP_BILINEAR);

                gdk_pixbuf_save(new, name, "jpeg", NULL, "quality", "100",
                                NULL);

                SDL_Surface *shrinkedcroppedcell = IMG_Load(name);

                copy_symbol(emptycell, shrinkedcroppedcell,
                            (28 - shrinkedcroppedcell->w) / 2,
                            (28 - shrinkedcroppedcell->h) / 2);
                SDL_FreeSurface(croppedcell);
                SDL_FreeSurface(shrinkedcroppedcell);

                /*
                for (size_t x = 0; x < 28; x++)
                {
                    for (size_t y = 0; y < 28; y++)
                    {
                        Uint32 pix = get_pixel(emptycell, x, y);
                        Uint8 color;
                        SDL_GetRGB(pix, emptycell->format, &color, &color,
                &color); if (color < 128)
                        {
                            put_pixel(emptycell, x, y,
                SDL_MapRGB(emptycell->format, 0, 0, 0));
                        }
                        else
                        {
                            put_pixel(emptycell, x, y,
                SDL_MapRGB(emptycell->format, 255, 255, 255));
                        }
                    }
                }
                */

                if (SDL_SaveBMP(emptycell, name) != 0) {
                    printf("SDL_SaveBMP failed: %s\n", SDL_GetError());
                }

                SDL_FreeSurface(emptycell);
            }
            SDL_FreeSurface(cell);
            SDL_FreeSurface(cellsegmap);
        }
    }
}

void copy_symbol(SDL_Surface *output, SDL_Surface *input, size_t left,
                 size_t top) {
    for (size_t x = 0; x < (size_t)input->w; x++) {
        for (size_t y = 0; y < (size_t)input->h; y++) {
            Uint32 pix = get_pixel(input, x, y);
            Uint8 color;
            SDL_GetRGB(pix, input->format, &color, &color, &color);
            put_pixel(output, x + left, y + top,
                      SDL_MapRGB(output->format, color, color, color));
        }
    }
}

int remove_dir(const char *folder_name) {
    DIR *folder = opendir(folder_name);
    int folder_name_len = strlen(folder_name);

    if (folder != NULL) {
        struct dirent *entity;
        entity = readdir(folder);
        while (entity != NULL) {
            char path[folder_name_len + strlen(entity->d_name) + 1];
            strcpy(path, folder_name);
            strcat(path, entity->d_name);

            remove(path);

            entity = readdir(folder);
        }
    }
    closedir(folder);
    return 0;
}