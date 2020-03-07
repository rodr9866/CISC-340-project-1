#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <glib.h>

void assembler (char *inputFile, char *outputFile);
int isCommand(char* str);
int digitsOnly(char *s);
int commandValue(char* str);
void checkRegRange(long int reg);
long int jType(int instructions[4]);
long int rType(int instructions[4]);
long int iType(int instructions[4], GHashTable* labels, int lineNumber);
long int oType(int instructions[4]);
long int fill(int instructions[4]);

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
			gchar *str = "string";
			str = g_strdup(token);
			if(('a' <= str[0] && str[0] <= 'z') || ('A' <= str[0] && str[0] <= 'Z'))
			{
				if(g_hash_table_contains(labels, str))
				{
					printf("ERROR: Duplicate labels detected: %s\n",str);
					exit(-1);
				}else{
					g_hash_table_insert(labels, str, (int*)lineNumber);
				}
			}else
			{
				printf("ERROR: Label must start with a character: %s\n", str);
				exit(-1);
			}
			if(strlen(str) > 7)
			{
				printf("ERROR: Labels must be 6 characters or fewer: %s\n", str);
				exit(-1);
			}


		}

		lineNumber++;
	}
	fseek(inFile, 0, SEEK_SET);


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
		if((line[0] != ' ' && line[0] != '\t') && isCommand(token))
		{
			//label is command
			printf("ERROR: Labels cannot be commands Line: %i Label: %s\n",lineNumber,token);
			printf("*Reminder* whitespace is required before opcodes\n");
			exit(-1);
		}
		if(!isCommand(token)){
			token = strtok(NULL," \t\n\v\f\r");
		}
		if(token != NULL)
		{

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
				}else if (g_hash_table_contains(labels, token)){
					if(instructions[lineNumber][0] == 4)
					{
						instructions[lineNumber][argument] = g_hash_table_lookup(labels, token) - (lineNumber + 1);
					}else{
						instructions[lineNumber][argument] = g_hash_table_lookup(labels, token);
					}
				}else if(!digitsOnly(token)){
					printf("ERROR: Line %i: Label not found: %s\n",lineNumber,token);
					exit(-1);
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
				machineInstructions[lineNumber] = iType(instructions[lineNumber], labels, lineNumber);
			}else if(opcode == 5){
				machineInstructions[lineNumber] = jType(instructions[lineNumber]);
			}else if(6 <= opcode && opcode <= 7){
				machineInstructions[lineNumber] = oType(instructions[lineNumber]);
			}else if(opcode == 8){
				machineInstructions[lineNumber] = fill(instructions[lineNumber]);
			}

		}
		else
		{
			printf("ERROR: opcode not valid: line %i command: %s\n", lineNumber, token);
			exit(-1);
		}
		lineNumber++;

	}

	for(int i = 0; i < lineNumber; i++){
		if (outFile != NULL){
			fprintf(outFile, "%li\n",machineInstructions[i]);
		}
		printf("%li\n",machineInstructions[i]);
	}

}

int isCommand(char* str)
{
	if(str == NULL)
	{
		printf("ERROR: extra whitespace\n");
		exit(-1);
	}
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
void checkRegRange(long int reg)
{
	if(reg > 7 || reg < 0){
		printf("ERROR: Register does not exist\n");
		exit(-1);
	}
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

int digitsOnly(char *s)
{
	int i = 0;
    while (s[i] != '\0') {
        if (isdigit(s[i]) == 0 && s[i] != '-')
		{
			return 0;
		}
		i = i + 1;
    }

    return 1;
}
long int jType(int instructions[4])
{
	long int result = 0;
	long int opcode = instructions[0];
	long int regA   = instructions[1];
	long int regB   = instructions[2];
	checkRegRange(regA);
	checkRegRange(regB);
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
	checkRegRange(regA);
	checkRegRange(regB);
	checkRegRange(destReg);

	opcode = opcode<<22;
	regA   = regA<<19;
	regB   = regB<<16;
	result = result | opcode | regA | regB | destReg;

	return result;
}
long int iType(int instructions[4], GHashTable* labels, int lineNumber)
//This needs work to handle beq, currently doesn't handle it
//We have to figure out someway to make the offset only 16 bits because otherwise if it is negative it will
//mess everything up
{
	long int result       = 0;
	long int opcode       = instructions[0];
	long int regA         = instructions[1];
	long int regB         = instructions[2];
	long int offset_field;
	checkRegRange(regA);
	checkRegRange(regB);
	offset_field = instructions[3];
	if (offset_field < -32768 || 32767 < offset_field)
	{
		printf("ERROR: Line %i: offset_field is out of range (−32768, 32767): %li\n",lineNumber, offset_field);
		exit(-1);
	}
	opcode = opcode<<22;
	regA   = regA<<19;
	regB   = regB<<16;
	if (offset_field >= 0){
		result = result | opcode | regA | regB | offset_field;
	}else{
		offset_field = offset_field & 0x0000FFFF;
		result = result | opcode | regA | regB | offset_field;
	}

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
long int fill(int instructions[4])
{
	long int result = 0;
	short int value = instructions[1];
	result = result | value;
	return result;
}
