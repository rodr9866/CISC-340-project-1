#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <glib.h>

void assembler (char *inputFile, char *outputFile);
int isCommand(char* str);
int commandValue(char* str);
char commands[9][6] = {"add","nand","lw","sw","beq","jalr","halt","noop",".fill"};
int main (int argc, char **argv){
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
			g_hash_table_insert(labels, token, (int*)lineNumber);
		}

		printf("%s %i\n",token, commandValue(token));
		lineNumber++;
	}
	fseek(inFile, 0, SEEK_SET);
	printf("\n\nSecond pass\n\n");
	//Second pass to create instructions

	int instructions[lineNumber][5];

	lineNumber = 0;
	while(fgets(line, 100, inFile) != NULL)
	{
		char* token = strtok(line," \t\n\v\f\r");
		if(!isCommand(token)){
			token = strtok(NULL," \t\n\v\f\r");
		}
		instructions[lineNumber][0] = commandValue(token);
		printf("%i\n", instructions[lineNumber][0]);

		lineNumber++;
	//	while(token != NULL && comment == 0){
	//		if(*token == '#'){
	//			comment = 1;
	//		}else
	//		{
	//			printf("%s ", token);
	//			token = strtok(NULL, " \t");
	//		}
	//	}
	//	printf("\n");
	//	comment = 0;
	}

}

int isCommand(char* str)
{
	int result = 0;
	for (int i=0; i < 8; i++)
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
