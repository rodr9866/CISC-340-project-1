#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void assembler(char *inputFile, char *outputFile){
	FILE *inFile;
	FILE *outFile;
	int  comment = 0;
	char* line = (char*)malloc(100*sizeof(char));
	inFile = fopen(inputFile,"r");
	if(outputFile != NULL){
		outFile = fopen(outputFile, "w");
	}
	while(fgets(line, 100, inFile) != NULL){
		char* token = strtok(line," 	");
        	while(token != NULL && comment == 0 ){
			if(*token == '#'){
				comment = 1;
			}else{
				 printf("%s\n", token);
				 token = strtok(NULL, "   ");

			}
                }
		comment = 0;
	}
}

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
	printf("outputfile is %s\n", outputFile);
	printf("Inputfile is %s\n", inputFile);
}


