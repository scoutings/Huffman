# Assignment 6

This is a program that will compress and decompress given files

code.c		Implementation of the code ADT

decode.c	This program will decompress a given file

encode.c	This program will compress a given file

huffman.c	This comatains many functions manipulationg trees to build, rebuild and delete trees

io.c		Implementation of  I/O ADT, uses low level sys calls to read/write

node.c 		Implementation of the node ADT

stack.c		Implementation of a stack of nodes

The command line arguments when running encode:

  -h             Program usage and help.

  -v             Print compression statistics.

  -i infile      Input file to compress.

  -o outfile     Output of compressed data.

The command line arguments when running decode:

  -h             Program usage and help.

  -v             Print compression statistics.

  -i infile      Input file to decompress.

  -o outfile     Output of decompressed data.

 ## Build

	$ make all
	$ make encode
	$ make decode

## Running

	$ ./encode -hv -i <infile> -o <outfile>
	$ ./decode -hv -i <infile> -o <outfile>

## Cleaning

	$ make clean
