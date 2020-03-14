# Notes

	03/12/2020 - part 2, simulator has been completed.

# How to run
	With one input file to print to screen
	./sim -i {input.as}
	With one input file to write to an output file
	./sim -i {input.as} > {output.mc}

# Dependent files
	simulator.c - code that converts assembly to machine code
	Makefile - for compiling simulator.c and making an executable

# Test files
**Each file has a respective *.mc file of assembled machine code**
	1. jumps.asm - tests commands that shift the PC
	2. nand_sw.asm - tests sw and nand
