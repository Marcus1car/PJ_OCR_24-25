/* linked_list Implementation */
#include "linked_list.h"

// init function
linked_list *init_linked_list() {
    linked_list *list = (linked_list *)malloc(sizeof(linked_list));
    // setup attributes
    list->length = 0;
    list->value_size = sizeof(void *);
    list->head = NULL;
    // attach methods
    list->get_value_at = &linked_list_get_value_at;
    list->insert_value_at = &linked_list_insert_value_at;
    list->append_value = &linked_list_append_value;
    list->remove_value_at = &linked_list_remove_value_at;
    list->index_of = &linked_list_index_of;

    return list;
}

/* Free function */
void free_linked_list(linked_list *list) {
    _free_linked_list_node(list->head);
    free(list);
}

// free node
void _free_linked_list_node(ll_node *node) {
    if (node->next != NULL)
        _free_linked_list_node(node->next);
    free(node->value);
    free(node);
}

// error message
void _linked_list_exit_msg(const char *msg) {
    fprintf(stderr, "%s", msg);
    fprintf(stderr, "%s", "\n");
    exit(EXIT_FAILURE);
}

// is an index valid ?
bool _linked_list_valid_index(linked_list *list, int index) {
    return 0 <= index && index < list->length;
}

// assert that an index is valid
void _linked_list_assert_index(linked_list *list, int index) {
    if (_linked_list_valid_index(list, index))
        return;
    char msg[255];
    sprintf(msg, LL_INDEX_ERROR_MSG, index, list->length);
    _linked_list_exit_msg(msg);
}

// get the value at the index
ll_node *_linked_list_get_node_at(linked_list *list, int index) {
    ll_node *node = list->head;

    for (int i = 0; i < index; i++) {
        node = node->next;
        // verify after we get the next node: the head should have been verified
        // in the _assert_index call
        if (node == NULL) {
            char msg[255];
            sprintf(msg, LL_NULL_POINTER_NODE, i);
            _linked_list_exit_msg(msg);
        }
    }

    return node;
}

// create a new node
ll_node *_linked_list_new_node() {
    // allocate new memory for node
    size_t ll_node_size = sizeof(struct ll_node);
    struct ll_node *node = (struct ll_node *)malloc(ll_node_size);
    // if the pointer is NULL, the allocation faled
    if (node == NULL) {
        char msg[255];
        sprintf(msg, LL_MEM_ALLOC_ERROR_MSG, ll_node_size);
        _linked_list_exit_msg(msg);
    }
    node->next = NULL;
    return node;
}

/* 'Public' Methods */

// get value at index
void *linked_list_get_value_at(linked_list *list, int index) {
    _linked_list_assert_index(list, index);
    return _linked_list_get_node_at(list, index)->value;
}

// insert value at index
void linked_list_insert_value_at(linked_list *list, int index, void *value) {
    // init list ?
    if (index == 0) {
        // insert at head
        ll_node *new_head = _linked_list_new_node();
        new_head->value = value;
        new_head->next = list->head;
        list->head = new_head;
    }
    // append after head
    else {
        int new_index = index - 1;
        _linked_list_assert_index(list, new_index);
        // get nodes
        ll_node *prev_node = _linked_list_get_node_at(list, new_index);
        ll_node *after_node = prev_node->next;
        ll_node *new_node = _linked_list_new_node();
        new_node->value = value;
        // set connections
        prev_node->next = new_node;
        new_node->next = after_node;
    }

    // increment list length
    list->length++;
}

// append value
void linked_list_append_value(linked_list *list, void *value) {
    // add to empty list
    if (list->length == 0) {
        list->head = _linked_list_new_node();
        list->head->value = value;
    }
    // add to tail of the list
    else {
        ll_node *tail = _linked_list_get_node_at(list, list->length - 1);
        tail->next = _linked_list_new_node();
        tail->next->value = value;
    }

    // increment list length
    list->length++;
}

// remove value at index
void *linked_list_remove_value_at(linked_list *list, int index) {
    _linked_list_assert_index(list, index);
    void *removed_value = NULL;

    // remove the head
    if (index == 0) {
        // there is only the head
        if (list->length == 1) {
            removed_value = list->head->value;
            free(list->head);
            list->head = NULL;
        }
        // there are more
        else {
            ll_node *old_head = list->head;
            ll_node *new_head = old_head->next;
            removed_value = old_head->value;
            list->head = new_head;
            free(old_head);
            old_head = NULL;
        }
    }
    // remove somewhere else
    else {
        ll_node *previous_node = _linked_list_get_node_at(list, index - 1);
        ll_node *current_node = previous_node->next;
        removed_value = current_node->value;
        ll_node *next_node = current_node->next;
        previous_node->next = next_node;
        free(current_node);
        current_node = NULL;
    }

    // decrement the length
    list->length--;
    return removed_value;
}

// index of value
int linked_list_index_of(linked_list *list, void *value) {
    ll_node *node = list->head;
    int index = 0;
    do {
        if (node->value == value) {
            return index;
        }
        node = node->next;
        index++;
    } while (node != NULL);

    return -1;
}
