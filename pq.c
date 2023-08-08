#include "pq.h"

#include "node.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct PriorityQueue {
    uint32_t head;
    uint32_t capacity;
    Node **nodes;
};

PriorityQueue *pq_create(uint32_t capacity) {
    PriorityQueue *q = (PriorityQueue *) malloc(sizeof(PriorityQueue));
    if (q) {
        q->head = 0;
        q->capacity = capacity;
        q->nodes = (Node **) calloc(capacity, sizeof(Node *));
        if (!q->nodes) {
            free(q);
            q = NULL;
        }
    }
    return q;
}

void pq_delete(PriorityQueue **q) {
    if (*q && (*q)->nodes) {
        free((*q)->nodes);
        free(*q);
        *q = NULL;
    }
    return;
}

bool pq_empty(PriorityQueue *q) {
    return !q->head;
}

bool pq_full(PriorityQueue *q) {
    return q->head == q->capacity;
}

uint32_t pq_size(PriorityQueue *q) {
    return q->head;
}

bool enqueue(PriorityQueue *q, Node *n) {
    // very simple insertion sort algorithm
    bool ret_val = false;
    if (pq_empty(q)) {
        q->nodes[q->head] = n;
        q->head += 1;
        ret_val = true;
    } else if (!pq_full(q)) {
        uint32_t backwards_idx = q->head;
        // We start form the rear and shift each element if its priority does not fit accordingly
        while (backwards_idx && q->nodes[backwards_idx - 1]->frequency < n->frequency) {
            q->nodes[backwards_idx] = q->nodes[backwards_idx - 1];
            backwards_idx -= 1;
        }
        q->nodes[backwards_idx] = n;
        q->head += 1;
        ret_val = true;
    } else {
        ret_val = false;
    }
    return ret_val;
}

bool dequeue(PriorityQueue *q, Node **n) {
    if (!pq_empty(q)) {
        q->head -= 1;
        *n = q->nodes[q->head];
        return true;
    } else {
        return false;
    }
}

void pq_print(PriorityQueue *q) {
    printf("[");
    for (uint32_t i = 0; i < pq_size(q); i++) {
        node_print(q->nodes[i]);
        if (i != pq_size(q) - 1) {
            printf(", ");
        }
    }
    printf("]\n");
    return;
}
