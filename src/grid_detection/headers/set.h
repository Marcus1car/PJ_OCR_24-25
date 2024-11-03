#ifndef SET_H
#define SET_H

typedef struct set {
    struct set *parent;
    size_t value;
} set;

set *init_set(set *par, size_t val);

set *get_root(set *s);

#endif