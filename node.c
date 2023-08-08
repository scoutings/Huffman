#include "node.h"

#include <stdio.h>
#include <stdlib.h>

Node *node_create(uint8_t symbol, uint64_t frequency) {
    Node *n = (Node *) malloc(sizeof(Node));
    if (n) {
        n->symbol = symbol;
        n->frequency = frequency;
        n->left = n->right = NULL;
    }
    return n;
}

void node_delete(Node **n) {
    if (*n) {
        free(*n);
        *n = NULL;
    }
    return;
}

Node *node_join(Node *left, Node *right) {
    Node *ret_n = node_create((uint8_t) '$', left->frequency + right->frequency);
    ret_n->left = left;
    ret_n->right = right;
    return ret_n;
}

void node_print(Node *n) {
    printf("(%u, %lu)", n->symbol, n->frequency);
}
