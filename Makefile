CC = gcc

all: assembler.o test.o
	$(CC) assembler.o -o assembler
	$(CC) test.o -o test
assembler: assembler.c
	$(CC) -c assembler.c

test: test.c
	$(CC) -c test.c

clean:
	rm *.o test  assembler
