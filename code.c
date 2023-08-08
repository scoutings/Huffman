#include "code.h"

#include <stdio.h>
#include <string.h>

static void print_vector(uint8_t vector) {
    // Helper function for my print code mainly for debugging
    for (uint8_t i = 0; i < 8; i++) {
        printf(" %u", (((uint8_t) 0x01 << i) & vector) >> i);
    }
    printf("\n");
    for (int i = 0; i < 8; i++) {
        printf("%2d", i);
    }
    printf("\n");
    return;
}

Code code_init(void) {
    Code c;
    c.top = 0;
    for (int i = 0; i < MAX_CODE_SIZE; i++) {
        c.bits[i] = 0;
    }
    return c;
}

uint32_t code_size(Code *c) {
    return c->top;
}

bool code_empty(Code *c) {
    return !code_size(c);
}

bool code_full(Code *c) {
    return code_size(c) == MAX_CODE_SIZE;
}

bool code_push_bit(Code *c, uint8_t bit) {
    // Push if stack has room
    if (!code_full(c) && (bit == 0 || bit == 1)) {
        c->bits[c->top / 8] |= (bit << (c->top % 8));
        c->top += 1;
        return true;
    } else {
        return false;
    }
}

bool code_pop_bit(Code *c, uint8_t *bit) {
    // Can only pop if the stack is not empty
    if (!code_empty(c)) {
        c->top -= 1;
        *bit = (c->bits[c->top / 8] & ((uint8_t) 0x01 << (c->top % 8))) >> c->top % 8;
        c->bits[c->top / 8] &= ~((uint8_t) 0x01 << (c->top % 8));
        return true;
    } else {
        return false;
    }
}

void code_print(Code *c) {
    printf("Top of stack: %u\n", c->top);
    for (int i = 0; i < 1; i++) {
        printf("Vector %d:\n", i);
        print_vector(c->bits[i]);
    }
    return;
}
