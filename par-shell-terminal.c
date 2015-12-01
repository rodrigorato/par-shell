#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commandlinereader.h"
#include "error_helper.h"

#define MAX_BUF 1024 /* The communication's max buffer size */
#define PATHNAME_MAX_ARGS 5 /* Program can be ran with 5 arguments */
#define INPUTVECTOR_SIZE PATHNAME_MAX_ARGS+2 /* vector[0] = program name; vector[-1] = NULL */
#define NORMAL_COMMAND 0
#define EXIT_COMMAND 1
#define EXIT_GLOBAL_COMMAND 2

int specialCommand(char* command){
	if(!strcmp(command, "exit"))
		return EXIT_COMMAND;
	if(!strcmp(command, "exit-global"))
		return EXIT_GLOBAL_COMMAND;
	return NORMAL_COMMAND;
}

int main(int argc, char** argv){
	int writePipeDescriptor;
	char* writePipe; /* The name of the pipe used to send commands to the par-shell "server" */
	char inputString[MAX_BUF];
	if(argc != 2){ /* If the number of specified arguments is incorrect, the program won't run.  */
		defaultErrorBehavior(ERR_WRONGARGUMENTS);
		exit(EXIT_FAILURE);
	}
	writePipe = argv[1];
	//apanhar erros
	writePipeDescriptor = open(writePipe, O_WRONLY);

	printf("par-shell@%s $ ", writePipe); 	
	while(!specialCommand(inputString) && fgets(inputString, MAX_BUF, stdin)){
		switch(specialCommand(inputString)){
			case NORMAL_COMMAND: 
				errWriteToPipe(inputString, writePipeDescriptor);
				printf("par-shell@%s $ ", writePipe);
				break;
			
			case EXIT_COMMAND:
				break;
			
			case EXIT_GLOBAL_COMMAND:
				//par-shell deve mandar matar todos os terms (Ctrl c signal?)
				errWriteToPipe("exit", writePipeDescriptor);
				break;
		}
	}

	close(writePipeDescriptor);
	/* Handle different exit commands */

	return 0;
}
