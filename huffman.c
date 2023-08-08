#include "huffman.h"

#include "pq.h"
#include "stack.h"

#include <stdio.h>

// Ascii of the letter L
static uint8_t L = 76;
// Ascii of the letter I
static uint8_t I = 73;

static void post_order(Node *curr_node, Code table[static ALPHABET], Code *c) {
    // If this is a leaf node we update the table
    if (!curr_node->left && !curr_node->right) {
        table[curr_node->symbol] = *c;
    } else {
        // Else we continue with the traversal recursively
        uint8_t b = 0;
        code_push_bit(c, b);
        post_order(curr_node->left, table, c);
        code_pop_bit(c, &b);

        b = 1;
        code_push_bit(c, b);
        post_order(curr_node->right, table, c);
        code_pop_bit(c, &b);
    }
    return;
}

Node *build_tree(uint64_t hist[static ALPHABET]) {
    // Create a queue and populate it with all symbols
    PriorityQueue *q = pq_create(ALPHABET);
    for (int i = 255; i >= 0; i--) {
        if (hist[i]) {
            Node *n = node_create(i, hist[i]);
            enqueue(q, n);
        }
    }
    while (pq_size(q) > 1) {
        // While there are still nodes to join, pop, join, and enqueue them
        Node *n1;
        dequeue(q, &n1);
        Node *n2;
        dequeue(q, &n2);
        Node *nj = node_join(n1, n2);
        enqueue(q, nj);
    }
    Node *ret_node;
    dequeue(q, &ret_node);
    pq_delete(&q);
    // return the root node
    return ret_node;
}

void build_codes(Node *root, Code table[static ALPHABET]) {
    Code c = code_init();
    post_order(root, table, &c);
    return;
}

Node *rebuild_tree(uint16_t nbytes, uint8_t tree_dump[static nbytes]) {
    // Create a stack
    Stack *s = stack_create(nbytes);
    for (int i = 0; i < nbytes; i++) {
        // for every byte in the tree
        if (tree_dump[i] == L) {
            // If the element is an L we create a node
            Node *n = node_create(tree_dump[i + 1], 0);
            i += 1;
            stack_push(s, n);
        } else if (tree_dump[i] == I) {
            // if it is an I we join the previous two nodes
            Node *r;
            stack_pop(s, &r);
            Node *l;
            stack_pop(s, &l);
            Node *j = node_join(l, r);
            stack_push(s, j);
        }
    }
    Node *root_node;
    stack_pop(s, &root_node);
    stack_delete(&s);
    // return the root node
    return root_node;
}

void delete_tree(Node **root) {
    if (!(*root)->left && !(*root)->right) {
        node_delete(root);
        *root = NULL;
    } else {
        delete_tree(&(*root)->left);
        (*root)->left = NULL;

        delete_tree(&(*root)->right);
        (*root)->right = NULL;

        node_delete(root);
        *root = NULL;
    }
    return;
}
