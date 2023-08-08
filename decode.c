#include "code.h"
#include "defines.h"
#include "header.h"
#include "huffman.h"
#include "io.h"
#include "node.h"
#include "pq.h"
#include "stack.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define OPTIONS "hi:o:v"

static void print_helper() {
    printf(
        "SYNOPSIS\n  A Huffman decoder.\n  Decompresses a file using the Huffman coding "
        "algorithm.\n\nUSAGE\n  ./decode [-h] [-i infile] [-o outfile]\n\nOPTIONS\n  -h            "
        " Program usage and help.\n  -v             Print compression statistics.\n  -i infile     "
        " Input file to decompress.\n  -o outfile     Output of decompressed data.\n");
    return;
}

static void print_verbose(uint64_t uncompressed_file_size, uint64_t compressed_file_size) {
    printf("Compressed file size: %lu bytes\nDecompressed file size: %lu bytes\nSpace saving: "
           "%.2f%%\n",
        compressed_file_size, uncompressed_file_size,
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

    uint64_t uncompressed_file_size;
    uint64_t compressed_file_size;

    // Construct the header
    Header header;

    // Read the magic number (4 bytes) from the infile and verify
    uint32_t magic_number = 0;
    read_bytes(infile, (uint8_t *) &magic_number, 4);
    if (magic_number != MAGIC) {
        printf("Invalid magic number.\n");
        if (options[INPUT]) {
            close(infile);
        }
        if (options[outfile]) {
            close(outfile);
        }
        exit(1);
    }
    header.magic = magic_number;

    // Read the permission (2 bytes) and set the outfile to the samer perms
    uint16_t permissions = 0;
    read_bytes(infile, (uint8_t *) &permissions, 2);
    if (options[OUTPUT]) {
        fchmod(outfile, permissions);
    }
    header.permissions = permissions;

    // Read tree size (2 bytes)
    uint16_t tree_size = 0;
    read_bytes(infile, (uint8_t *) &tree_size, 2);
    header.tree_size = tree_size;

    // Read the file size (8 bytes) and print if verbose
    uint64_t file_size = 0;
    read_bytes(infile, (uint8_t *) &file_size, 8);
    header.file_size = file_size;

    // Read the tree dump
    uint8_t tree_dump[MAX_TREE_SIZE] = { 0 };
    read_bytes(infile, tree_dump, tree_size);

    Node *root_node = rebuild_tree(tree_size, tree_dump);

    uint8_t bit;
    Node *curr_node = root_node;

    // While the output file is still not the size of the original compressed
    // file we keep reading reading
    while (bytes_written < header.file_size) {
        read_bit(infile, &bit);
        // Read the next bit
        // Traverse down the tree
        if (bit) {
            curr_node = curr_node->right;
        } else {
            curr_node = curr_node->left;
        }
        // Check if the new node is a leaf node
        if (!curr_node->left && !curr_node->right) {
            write_bytes(outfile, &curr_node->symbol, 1);
            curr_node = root_node;
        }
    }

    compressed_file_size = bytes_read;
    uncompressed_file_size = bytes_written;

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
