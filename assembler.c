#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <glib.h>

void assembler (char *inputFile, char *outputFile);
int isCommand(char* str);
int commandValue(char* str);

long int jType(int instructions[4]);
long int rType(int instructions[4]);
long int iType(int instructions[4], GHashTable* labels);
long int oType(int instructions[4]);

char commands[9][6] = {"add","nand","lw","sw","beq","jalr","halt","noop",".fill"};


int main(int argc, char **argv){
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

	assembler(inputFile, outputFile);
	//printf("outputfile is %s\n", outputFile);
	//printf("Inputfile is %s\n", inputFile);
}

void assembler(char *inputFile, char *outputFile)
{
	FILE        *inFile;
	FILE        *outFile;
	int         comment = 0;
	GHashTable* labels = g_hash_table_new(g_str_hash,g_int_equal);
	char*       line = (char*)malloc(100*sizeof(char));
	int         lineNumber = 0;

	//File setup
	inFile = fopen(inputFile,"r");
	if(outputFile != NULL)
	{
		outFile = fopen(outputFile, "w");
	}

	//First pass to get labels
	while(fgets(line, 100, inFile) != NULL)
	{
		char* token = strtok(line," \t\n\v\f\r");

		//token is first word
		if(!isCommand(token))
		{
			printf("LABEL: %s VALUE: %i\n",token,lineNumber);
			char* label = (char*)malloc(10*sizeof(char));
			strcpy(label, token);
			g_hash_table_insert(labels, token, (int*)lineNumber);
			free(label);
		}

		printf("%s %i\n",token, commandValue(token));
		lineNumber++;
	}
	fseek(inFile, 0, SEEK_SET);

	//     print contents of hash table ///
	printf("\n LABELS FOUND\n\n");
	GHashTableIter iter;
	gpointer key, value;

	g_hash_table_iter_init (&iter, labels);
	while (g_hash_table_iter_next (&iter, &key, &value))
  	{
    	printf("key: %s  value: %i\n", key,value);
  	}
	//     print contents of hash table ///

	printf("\n\nSecond pass\n\n");
	//Second pass to create instructions in machine language

	long int machineInstructions[lineNumber];
	int instructions[lineNumber][4];
	lineNumber = 0;

	while(fgets(line, 100, inFile) != NULL)
	{
		//ignore labels at start
		char* lineCopy = (char*)malloc(100*(sizeof(char)));
		strcpy(lineCopy,line);
		char* token = strtok(lineCopy," \t\n\v\f\r");
		int   opcode;
		if(!isCommand(token)){
			token = strtok(NULL," \t\n\v\f\r");
		}

		if(isCommand(token))
		{
			//get and store opcode
			opcode = commandValue(token);
			instructions[lineNumber][0] = opcode;
			int argument = 1;
			token = strtok(NULL, " \t\n\v\f\r");
			while(token != NULL && comment == 0)
			{
				if(*token == '#'){
					comment = 1;
				}else if (g_hash_table_lookup(labels, token) != NULL){
					instructions[lineNumber][argument] = *(int*)g_hash_table_lookup(labels, token);
				}else{
					instructions[lineNumber][argument] = atoi(token);
				}
				token = strtok(NULL, " \t\n\v\f\r");
				argument++;
			}
			comment = 0;





			if(opcode == 0 || opcode == 1){
				machineInstructions[lineNumber] = rType(instructions[lineNumber]);
			}else if(2 <= opcode && opcode <= 4){
				machineInstructions[lineNumber] = iType(instructions[lineNumber], labels);
			}else if(opcode == 6){
				machineInstructions[lineNumber] = jType(instructions[lineNumber]);
			}else if(7 <= opcode && opcode <= 8){
				machineInstructions[lineNumber] = oType(instructions[lineNumber]);
			}

		}
		else
		{
			printf("error finding opcode\n");
		}
		lineNumber++;

	}
	printf("\ninstructions array\n");
	for(int i = 0; i < lineNumber; i++){
		printf("Line %i :", i);
		for(int j = 0; j < 4; j++){
			printf(" %i ", instructions[i][j]);
		}
		printf("\n");
	}
	printf("\nmachineInstructions\n");

	for(int i = 0; i < lineNumber; i++){
		printf("Line %i : %li\n",i,machineInstructions[i]);
	}

}

int isCommand(char* str)
{
	int result = 0;
	for (int i=0; i < 9; i++)
	{
		if(!strcmp(str,commands[i]))
		{
			result = 1;
		}
	}
	return result;
}

int commandValue(char* str)
{
	int result = -1;
	for (int i=0; i < 9; i++)
	{
		if(!strcmp(str,commands[i]))
		{
			result = i;
		}
	}
	return result;
}

long int jType(int instructions[4])
{
	long int result = 0;
	long int opcode = instructions[0];
	long int regA   = instructions[1];
	long int regB   = instructions[2];

	opcode = opcode<<22;
	regA   = regA<<19;
	regB   = regB<<16;
	result = result | opcode | regA | regB;

	return result;
}
long int rType(int instructions[4])
{
	long int result  = 0;
	long int opcode  = instructions[0];
	long int destReg = instructions[1];
	long int regA    = instructions[2];
	long int regB    = instructions[3];

	opcode = opcode<<22;
	regA   = regA<<19;
	regB   = regB<<16;
	result = result | opcode | regA | regB | destReg;

	return result;
}
long int iType(int instructions[4], GHashTable* labels)
{
	long int result       = 0;
	long int opcode       = instructions[0];
	long int regA         = instructions[1];
	long int regB         = instructions[2];
	long int offset_field = instructions[3];
	opcode = opcode<<22;
	regA   = regA<<19;
	regB   = regB<<16;
	result = result | opcode | regA | regB | offset_field;

	return result;
}
long int oType(int instructions[4])
{
	long int result = 0;
	long int opcode = instructions[0];
	opcode = opcode<<22;
	result = result | opcode;

	return result;
}
