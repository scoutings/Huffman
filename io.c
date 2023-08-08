#include "io.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static uint8_t buffer_read[BLOCK] = { 0 };
// This is a bit index
static int32_t buffer_read_idx = 0;
static int32_t last_bit = -1;

static uint8_t buffer_write[BLOCK];
static int32_t buffer_write_idx = 0;

uint64_t bytes_read = 0;
uint64_t bytes_written = 0;

int read_bytes(int infile, uint8_t *buf, int nbytes) {
    int bytes_left = nbytes;
    // While there are still some bytes to read, we keep reading
    while (bytes_left) {
        // We store the bytes read in the index that we have not written to yet
        int temp_read = read(infile, &buf[nbytes - bytes_left], bytes_left);
        // If the infile has read some bytes
        if (temp_read > 0) {
            bytes_read += temp_read;
            bytes_left -= temp_read;
        } else if (temp_read == -1) {
            // If there was an error in the read we return -1
            return -1;
        } else {
            break;
        }
    }
    return nbytes - bytes_left;
}

int write_bytes(int outfile, uint8_t *buf, int nbytes) {
    int bytes_left = nbytes;
    // While there are still some bytes to write, we keep writing
    while (bytes_left) {
        // We store the bytes written in the index that we have not read from yet
        int temp_write = write(outfile, &buf[nbytes - bytes_left], bytes_left);
        // If the outfile has written some bytes
        if (temp_write > 0) {
            bytes_written += temp_write;
            bytes_left -= temp_write;
        } else if (temp_write == -1) {
            // If there was an error in the write we return -1
            printf("Error with code: %s\n", strerror(errno));
            return -1;
        } else {
            break;
        }
    }
    return nbytes - bytes_left;
}

bool read_bit(int infile, uint8_t *bit) {
    bool ret_val;
    if (!buffer_read_idx) {
        // If the bufffer index is 0 we need to read mroe into the buffer
        memset(buffer_read, 0, BLOCK);
        // Did this while debugging ^ no real need to take it out
        uint32_t bytes_read = read_bytes(infile, buffer_read, BLOCK);
        if (bytes_read < BLOCK) {
            last_bit = 8 * bytes_read;
        }
    }
    // get the next bit in the buffer
    *bit = (buffer_read[buffer_read_idx / 8] & ((uint8_t) 0x01 << buffer_read_idx % 8))
           >> buffer_read_idx % 8;
    // If it is the last bit we return false
    if (buffer_read_idx == last_bit) {
        ret_val = false;
    } else {
        ret_val = true;
    }
    buffer_read_idx += 1;
    if (buffer_read_idx == 8 * BLOCK) {
        // reset buffer idx if we have reached the end of the buffer
        buffer_read_idx = 0;
    }
    return ret_val;
}

void write_code(int outfile, Code *c) {
    // iterate through the code
    for (uint8_t i = 0; i < c->top; i++) {
        // Fetch the bit in the code
        uint8_t bit = (c->bits[i / 8] & ((uint8_t) 0x01 << i % 8)) >> i % 8;
        // write the bits value in the buffer
        if (bit) {
            buffer_write[buffer_write_idx / 8] |= (uint8_t) 0x01 << (buffer_write_idx % 8);
        } else {
            buffer_write[buffer_write_idx / 8] &= ~((uint8_t) 0x01 << (buffer_write_idx % 8));
        }
        buffer_write_idx += 1;
        if (buffer_write_idx == BLOCK * 8) {
            // If we have reached the end of the buffer we need to write
            write_bytes(outfile, buffer_write, BLOCK);
            buffer_write_idx = 0;
        }
    }
    return;
}

void flush_codes(int outfile) {
    // How many bytes we muxt write into the outfile
    uint32_t bytes_idx;
    if (buffer_write_idx % 8 == 0) {
        bytes_idx = (buffer_write_idx / 8);
    } else {
        bytes_idx = (buffer_write_idx / 8) + 1;
    }
    // Zero out any bits left in the last byte
    for (uint32_t i = buffer_write_idx; i < 8 * bytes_idx; i++) {
        buffer_write[buffer_write_idx / 8] &= ~((uint8_t) 0x01 << (buffer_write_idx % 8));
    }
    write_bytes(outfile, buffer_write, bytes_idx);
    return;
}
