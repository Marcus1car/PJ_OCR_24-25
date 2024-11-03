#include <stdio.h>
#include <stdlib.h>

typedef struct set {
    struct set *parent;
    size_t value;
} set;

set *init_set(set *par, size_t val) {
    set *s = malloc(sizeof(set));
    s->value = val;
    if (par == NULL) {
        s->parent = s;
    } else {
        s->parent = s;
    }
    return s;
}

set *get_root(set *s) {
    set *p = s;
    while (p->parent != p) {
        p = p->parent;
    }
    return p;
}