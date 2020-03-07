# -*- Makefile -*-
CC = gcc

all: asm

asm: assembler.o
	$(CC) assembler.o `pkg-config --cflags --libs glib-2.0` -o asm

assembler.o: assembler.c
	$(CC) -c assembler.c `pkg-config --cflags --libs glib-2.0` -o assembler.o

clean:
	rm *.o asm
