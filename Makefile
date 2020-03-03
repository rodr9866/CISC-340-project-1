# -*- Makefile -*-
CC = gcc

all: assembler

assembler: assembler.o
	$(CC) assembler.o `pkg-config --cflags --libs glib-2.0` -o assembler

assembler.o: assembler.c
	$(CC) -c assembler.c `pkg-config --cflags --libs glib-2.0` -o assembler.o

clean:
	rm *.o assembler
