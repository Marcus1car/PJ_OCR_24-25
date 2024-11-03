// linked_list.h

#ifndef MEM_LINKED_LIST_H
#define MEM_LINKED_LIST_H

/* linked_list Declaration */
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* Constants */
#define LL_INDEX_ERROR_MSG                                                     \
    "linked_list:IndexError: %d out of bounds with linked list of length %d"
#define LL_MEM_ALLOC_ERROR_MSG                                                 \
    "linked_list:AllocationError: Could not allocate memory of size: %lu"
#define LL_ACESS_EMPTY_LIST "linked_list:Tried to access an empty linked list"
#define LL_NULL_POINTER_NODE "linked_list:node at index: %d the list is NULL"

/* Structures */

// linked list node structure
typedef struct ll_node ll_node;
struct ll_node {
    void *value;
    struct ll_node *next;
};

// Linked List structure
typedef struct linked_list linked_list;
// (https://stackoverflow.com/questions/17052443/c-function-inside-struct)

struct linked_list {
    // attributes
    int length;
    size_t value_size;
    ll_node *head;

    // methods
    void *(*get_value_at)(linked_list *, int index);
    void (*insert_value_at)(linked_list *, int index, void *value);
    void (*append_value)(linked_list *, void *);
    void *(*remove_value_at)(linked_list *, int index);
    int (*index_of)(linked_list *, void *value);
};

/* Init function */
linked_list *init_linked_list();

/* Free function */
void free_linked_list(linked_list *list);

/* 'Private' Methods */

// free node
void _free_linked_list_node(ll_node *node);

// error message
void _linked_list_exit_msg(const char *msg);

// is an index valid ?
bool _linked_list_valid_index(linked_list *list, int index);

// assert that an index is valid
void _linked_list_assert_index(linked_list *list, int index);

// get the node at the index
ll_node *_linked_list_get_node_at(linked_list *list, int index);

// create a new node
ll_node *_linked_list_new_node();

/* 'Public' Methods */

// get value at index
void *linked_list_get_value_at(linked_list *list, int index);

// insert value at index
void linked_list_insert_value_at(linked_list *list, int index, void *value);

// append value
void linked_list_append_value(linked_list *list, void *value);

// remove value at index
void *linked_list_remove_value_at(linked_list *list, int index);

// remove value
int linked_list_index_of(linked_list *list, void *value);

#endif
