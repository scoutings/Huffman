#include "code.h"
#include "defines.h"
#include "header.h"
#include "huffman.h"
#include "io.h"
#include "node.h"
#include "pq.h"
#include "stack.h"

#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define OPTIONS "hi:o:v"

// Ascii for the char L
static uint8_t L = 76;
// Ascii for the char I
static uint8_t I = 73;

static void print_helper() {
    printf("SYNOPSIS:\n  A Huffman encoder.\n  Compresses a file using the Huffman coding "
           "algorithm.\n\nUSAGE\n  ./encode [-h] [-i infile] [-o outfile]\n\nOPTIONS\n  -h         "
           "    Program usage and help.\n  -v             Print compression statistics.\n  -i "
           "infile      Input file to compress.\n  -o outfile     Output of compressed data.\n");
    return;
}

static void write_tree(Node *curr_node, int outfile) {
    if (!curr_node->left && !curr_node->right) {
        // printf("\n++Symbol (%d) ++\n", curr_node->symbol);
        write_bytes(outfile, &L, 1);
        write_bytes(outfile, &curr_node->symbol, 1);
    } else {
        write_tree(curr_node->left, outfile);
        write_tree(curr_node->right, outfile);
        write_bytes(outfile, &I, 1);
    }
    return;
}

static void print_verbose(uint64_t uncompressed_file_size, uint64_t compressed_file_size) {
    printf("Uncompressed file size: %lu bytes\nCompressed file size: %lu bytes\nSpace saving: "
           "%.2f%%\n",
        uncompressed_file_size, compressed_file_size,
        100 * (1 - ((float) compressed_file_size / (float) uncompressed_file_size)));
}

int main(int argc, char **argv) {
    int opt;
    enum { INPUT, OUTPUT, VERBOSE };
    bool options[3] = { false, false, false };
    int infile = 0;
    int outfile = 1;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': print_helper(); exit(0);
        case 'i':
            options[INPUT] = true;
            infile = open(optarg, O_RDONLY);
            break;
        case 'o':
            options[OUTPUT] = true;
            outfile = open(optarg, O_WRONLY);
            break;
        case 'v': options[VERBOSE] = true; break;
        }
    }

    // These will be printed if the verbose flag is specified
    uint64_t uncompressed_file_size;
    uint64_t compressed_file_size;

    // We will use these to build the histogram
    uint8_t buffer[BLOCK] = { 0 };
    uint64_t hist[ALPHABET] = { 0 };
    hist[0] += 1;
    hist[255] += 1;
    int32_t bytes_numof;

    // Building the histogram
    do {
        bytes_numof = read_bytes(infile, buffer, BLOCK);
        for (int32_t i = 0; i < bytes_numof; i++) {
            hist[buffer[i]] += 1;
        }
    } while (bytes_numof == BLOCK);

    uncompressed_file_size = bytes_read;

    Node *root_node = build_tree(hist);

    Code table[ALPHABET] = { 0 };
    build_codes(root_node, table);

    // Get the stats of the uncompressed file
    struct stat statbuf;
    fstat(infile, &statbuf);

    // get uinique symbols
    uint8_t unique_symbols = 0;
    for (int i = 0; i < ALPHABET; i++) {
        if (hist[i]) {
            unique_symbols += 1;
        }
    }

    Header header;
    header.magic = MAGIC;
    header.permissions = statbuf.st_mode;
    header.tree_size = (unique_symbols * 3) - 1;
    header.file_size = statbuf.st_size;

    if (options[OUTPUT]) {
        fchmod(outfile, header.permissions);
    }

    write_bytes(outfile, (uint8_t *) &header, sizeof(Header));

    write_tree(root_node, outfile);

    // Reread the infile and generate the compressed file
    lseek(infile, 0, SEEK_SET);
    do {
        bytes_numof = read_bytes(infile, buffer, BLOCK);
        if (bytes_numof == -1) {
            printf("ERROR READING FILE (FIX ME LATER)\n");
        }
        for (int32_t i = 0; i < bytes_numof; i++) {
            write_code(outfile, &table[buffer[i]]);
        }
    } while (bytes_numof == BLOCK);

    flush_codes(outfile);

    if (!options[OUTPUT]) {
        printf("\n");
    }

    compressed_file_size = bytes_written;

    if (options[VERBOSE]) {
        print_verbose(uncompressed_file_size, compressed_file_size);
    }

    delete_tree(&root_node);

    if (options[INPUT]) {
        close(infile);
    }
    if (options[OUTPUT]) {
        close(outfile);
    }
    return 0;
}
