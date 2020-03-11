#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define NUMMEMORY 10
#define NUMREGS 8

typedef struct state_struct {
	int pc;
	int mem[NUMMEMORY];
	int reg[NUMREGS];
	int num_memory;
} statetype;

//DONE
int convert_num(int num);
int getImmediate(int mcInstruction);
int getOpcode(int mcInstruction);
int getDestReg(int mcInstruction);
int getRegA(int mcInstruction);
int getRegB(int mcInstruction);
void print_state(statetype *stateptr);
void simulator(char *inputFile, char *outputFile);
void add(statetype *stateptr);
void nand(statetype *stateptr);
void lw(statetype *stateptr);
void sw(statetype *stateptr);
void beq(statetype *stateptr);

//NEED TO BE IMPLEMENTED

void jalr(statetype *stateptr);

//NOT SURE IF NEEDED
void halt();
void noop();


int main(int argc, char **argv) {
	char *inputFile = NULL;
	char *outputFile = NULL;
	int index;
	int c;
	opterr = 0;

	while ((c = getopt (argc, argv, "i:o:")) != -1){
		switch (c) {
			case 'o':
				outputFile = optarg;
				break;
			case 'i':
				inputFile  = optarg;
				break;
			case '?':
				if (optopt == 'i'|| optopt == 'o'){
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				}else if (isprint (optopt)){
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				}else{
					fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
				}
				return 1;
			default:
				abort ();
		}
	}
	if(inputFile == NULL){
		fprintf(stderr, "-i  must be included\n");
		return 1;
	}

	simulator(inputFile, outputFile);
	return 0;
}

void simulator(char *inputFile, char *outputFile)
{
	FILE *inFile;
	FILE *outFile;
	char *line =(char*)malloc(100*sizeof(char));
	inFile = fopen(inputFile,"r");
	int mcInstruction = 0;
	int lineNumber = 0;
	int instructionCount = 0;


	if(outputFile != NULL){
		outFile = fopen(outputFile, "w");
	}

	while (fgets(line, 100,inFile) != NULL){
		lineNumber++;
		//print_state(&state);
	}

	int mem[lineNumber];
	int reg[NUMREGS];
	statetype state = {
		.pc = 0,
		.mem = *mem,
		.reg = *reg,
		.num_memory = lineNumber
	};

	lineNumber = 0;
	fseek(inFile, 0, SEEK_SET);
	while (fgets(line, 100,inFile) != NULL){
		mcInstruction = atoi(line);
		(&state)->mem[lineNumber] = mcInstruction;
		lineNumber++;
	}
	fclose(inFile);

	(&state)->reg[0] = 0;
	int opcode = getOpcode((&state)->mem[(&state)->pc]);

	while(opcode != 6){
		//while not a halt command
		print_state(&state);
		opcode = getOpcode((&state)->mem[(&state)->pc]);
		//execute correct command based on opcode
		if(opcode == 0){
			add(&state);
		}else if(opcode == 1){
			nand(&state);
		}else if(opcode == 2){
			lw(&state);
		}else if (opcode == 3){
			sw(&state);
		}else if(opcode == 4){
			beq(&state);
		}
		//printf("Opcode: %i, RegA: %i, RegB: %i, Immediate: %i\n",getOpcode((&state)->mem[(&state)->pc]), getRegA((&state)->mem[(&state)->pc]),getRegB((&state)->mem[(&state)->pc]),getImmediate((&state)->mem[(&state)->pc]));


		(&state)->pc++;
		instructionCount++;
	}

	printf("Machine halted\nINSTRUCTIONS: %i", instructionCount);
}

void add(statetype *stateptr){
	int destReg = getDestReg(stateptr->mem[stateptr->pc]);
	int regA = getRegA(stateptr->mem[stateptr->pc]);
	int regB = getRegB(stateptr->mem[stateptr->pc]);
	stateptr->reg[destReg] = (stateptr->reg[regA]) + (stateptr->reg[regB]);
}

void nand(statetype *stateptr){
	int destReg = getDestReg(stateptr->mem[stateptr->pc]);
	int regA = getRegA(stateptr->mem[stateptr->pc]);
	int regB = getRegB(stateptr->mem[stateptr->pc]);
	stateptr->reg[destReg] = ~((stateptr->reg[regA]) & (stateptr->reg[regB]));
}

void lw(statetype *stateptr){
	int regA = getRegA(stateptr->mem[stateptr->pc]);
	int regB = getRegB(stateptr->mem[stateptr->pc]);
	int offset = getImmediate(stateptr->mem[stateptr->pc]);

	int address = stateptr->reg[regB] + offset;

	stateptr->reg[regA] = stateptr->mem[address];

}

void sw(statetype *stateptr){
	int regA = getRegA(stateptr->mem[stateptr->pc]);
	int regB = getRegB(stateptr->mem[stateptr->pc]);
	int offset = getImmediate(stateptr->mem[stateptr->pc]);
	int address = stateptr->reg[regB] + offset;

	stateptr->mem[address] = stateptr->reg[regA];
}

void beq(statetype *stateptr){
	int regA = getRegA(stateptr->mem[stateptr->pc]);
	int regB = getRegB(stateptr->mem[stateptr->pc]);
	int offset = getImmediate(stateptr->mem[stateptr->pc]);

	if((stateptr->reg[regA]) == (stateptr->reg[regB])){
		stateptr->pc = stateptr->pc + offset;
	}
}




















void print_state(statetype *stateptr){
	//prints the current state of memory, registers and the PC
	int i;
	printf("\n@@@\nstate:\n");
	printf("\tpc %d\n", stateptr->pc);

	printf("\tmemory:\n");
	for(i = 0; i < stateptr->num_memory; i++){
		printf("\t\tmem[%d]=%d\n", i, stateptr->mem[i]);
	}
	printf("\tregisters:\n");
	for(i = 0; i < NUMREGS; i++){
		printf("\t\treg[%d]=%d\n", i, stateptr->reg[i]);
	}
	printf("end state\n");
}

int convert_num(int num){
	// convert a 16-bit number into a signed 32-bit integer
	if (num & (1<<15)){
		num -= (1<<16);
	}
	return(num);
}
int getDestReg(int mcInstruction){
	int result;
	result = mcInstruction & 0x00000007;
	return result;
}
int getOpcode(int mcInstruction){
	//returns the opcode value of commands EXCEPT .fill returns the sign of the number
	int result;
	result = mcInstruction >> 22;
	return result;
}
int getRegA(int mcInstruction){
	int result;
	result = mcInstruction >> 19;
	result = result & 0x00000007;
	return result;
}
int getRegB(int mcInstruction){
	int result;
	result = mcInstruction >> 16;
	result = result & 0x00000007;
	return result;
}
int getImmediate(int mcInstruction){
	int result;
	result = mcInstruction & 0x0000FFFF;
	result = convert_num(result);
	return result;
}
