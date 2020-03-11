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
int getRegA(int mcInstruction);
int getRegB(int mcInstruction);
void print_state(statetype *stateptr);
void simulator(char *inputFile, char *outputFile);

//NEED TO BE IMPLEMENTED
void add(int destReg, int regA, int regB);
void nand(int destReg, int regA, int regB);
void lw(int destReg, int offset, int base);
void sw(int destReg, int offset, int base);
void beq(int regA, int regB, int offset);
void jalr(int regA, int regB);
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
	int mem[NUMMEMORY];
	int reg[NUMREGS];
	struct state_struct state = {0,*mem,*reg,0};


	if(outputFile != NULL){
		outFile = fopen(outputFile, "w");
	}

	while (fgets(line, 100,inFile) != NULL){
		mcInstruction = atoi(line);
		printf("Opcode: %i, RegA: %i, RegB: %i, Immediate: %i\n",getOpcode(mcInstruction), getRegA(mcInstruction),getRegB(mcInstruction),getImmediate(mcInstruction));





		//print_state(&state);
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
