# -*- Makefile -*-
CC = gcc

all: sim

sim: simulator.o
	$(CC) simulator.o -o sim
simulator.o: simulator.c
	$(CC) -c simulator.c -o simulator.o
clean:
	rm *.o sim
