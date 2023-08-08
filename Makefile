CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic
LFLAGS = -lm

all: encode decode entropy

encode: encode.o node.o pq.o code.o io.o stack.o huffman.o
	$(CC) $(CFLAGS) -o encode encode.o node.o pq.o code.o io.o stack.o huffman.o

decode: decode.o node.o pq.o code.o io.o stack.o huffman.o
	$(CC) $(CFLAGS) -o decode decode.o node.o pq.o code.o io.o stack.o huffman.o

entropy: entropy.o
	$(CC) $(CFLAGS) -o entropy entropy.o $(LFLAGS)

encode.o: encode.c
	$(CC) $(CFLAGS) -c encode.c

decode.o: decode.c
	$(CC) $(CFLAGS) -c decode.c

node.o: node.c
	$(CC) $(CFLAGS) -c node.c

pq.o: pq.c
	$(CC) $(CFLAGS) -c pq.c

code.o: code.c
	$(CC) $(CFLAGS) -c code.c

io.o: io.c
	$(CC) $(CFLAGS) -c io.c

stack.o: stack.c
	$(CC) $(CFLAGS) -c stack.c

huffman.o: huffman.c
	$(CC) $(CFLAGS) -c huffman.c

entropy.o: entropy.c
	$(CC) $(CFLAGS) -c entropy.c

clean:
	rm -f *.o encode decode entropy

format:
	clang-format -i -style=file *.h *.c
