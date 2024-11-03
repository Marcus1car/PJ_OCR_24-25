#include "../headers/segmentation.h"

CCTuple *twopassSegmentation(SDL_Surface *image, SDL_Surface *segmap,
                             size_t *segval) {
    set *s = init_set(NULL, 0);
    size_t linked_size = 5000;
    set **linked = malloc(sizeof(set) * linked_size);
    Uint32 p;
    Uint32 p1, p2;
    for (size_t x = 0; x < (size_t)image->w; x++) {
        for (size_t y = 0; y < (size_t)image->h; y++) {
            Uint8 color;
            p = get_pixel(image, x, y);
            SDL_GetRGB(p, image->format, &color, &color, &color);
            size_t length = 0;
            if (color == 255) {
                int b = 0;
                if (x > 0) {
                    p1 = get_pixel(segmap, x - 1, y);
                    if (p1 != 0) {
                        b = 1;
                        length += 1;
                    }
                }
                if (y > 0) {
                    p2 = get_pixel(segmap, x, y - 1);
                    if (p2 != 0) {
                        length += 1;
                    }
                }
                if (length == 0) {
                    if (*(segval) + 1 == linked_size) {
                        linked_size = linked_size + 5000;
                        linked = realloc(linked, sizeof(set) * linked_size);
                    }
                    s = init_set(NULL, *segval);
                    linked[(int)*segval] = s;
                    put_pixel(segmap, x, y, *segval);
                    *segval += 1;
                } else if (length == 1) {
                    if (b) {
                        put_pixel(segmap, x, y, (size_t)p1);
                    } else {
                        put_pixel(segmap, x, y, (size_t)p2);
                    }
                } else {
                    p = p1;
                    if (p1 != p2) {
                        if (p2 < p1) {
                            p = p2;
                        }
                        set *s1 = linked[p1];
                        set *s2 = linked[p2];
                        s1 = get_root(s1);
                        s2 = get_root(s2);
                        if (s1->value != s2->value) {
                            if (s1->value < s2->value) {
                                s2->parent = s1;
                            } else {
                                s1->parent = s2;
                            }
                        }
                    }
                    put_pixel(segmap, x, y, p);
                }
            }
        }
    }

    CCTuple *histo = malloc(sizeof(CCTuple) * (*segval));
    for (size_t i = 0; i < *segval; i++) {
        CCTuple t = {.nb_pixels = 0, .label = i};
        histo[i] = t;
    }

    for (size_t x = 0; x < (size_t)image->w; x++) {
        for (size_t y = 0; y < (size_t)image->h; y++) {
            p = get_pixel(segmap, x, y);
            if (p != 0) {
                s = linked[p];
                s = get_root(s);
                histo[s->value].nb_pixels += 1;
                put_pixel(segmap, x, y, s->value);
            }
        }
    }

    heapSort(histo, *segval);

    free(linked);
    return histo;
}

size_t propagate(SDL_Surface *image, SDL_Surface *segmap, int segval, size_t x,
                 size_t y) {
    size_t n = 0;
    Uint8 color;
    Uint32 pix = get_pixel(image, x, y);
    SDL_GetRGB(pix, image->format, &color, &color, &color);
    Uint32 p = get_pixel(segmap, x, y);
    if (p != 0 || color == 0) {
        return 0;
    }
    put_pixel(segmap, x, y, segval);
    if (y > 0) {
        n += propagate(image, segmap, segval, x, y - 1);
    }
    if (x > 0) {
        n += propagate(image, segmap, segval, x - 1, y);
    }
    if (x < (size_t)image->w - 1) {
        n += propagate(image, segmap, segval, x + 1, y);
    }
    if (y < (size_t)image->h - 1) {
        n += propagate(image, segmap, segval, x, y + 1);
    }
    return 1 + n;
}
