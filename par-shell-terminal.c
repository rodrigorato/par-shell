#include <stdio.h>
#include "commandlinereader.h"
#include "error_helper.h"

#define PATHNAME_MAX_ARGS 5 /* Program can be ran with 5 arguments */
#define INPUTVECTOR_SIZE PATHNAME_MAX_ARGS+2 /* vector[0] = program name; vector[-1] = NULL */
#define STANDARD_EXIT 1
#define GLOBAL_EXIT 2
#define NORMAL_COMMAND 3

int commandType(char* command){
	if(!strcmp(command, "exit"))
		return STANDARD_EXIT;
	if(!strcmp(command, "exit-global"))
		return GLOBAL_EXIT;
	return NORMAL_COMMAND;
}

int main(int argc, char** argv){
	char* writePipe; /* The name of the pipe used to send commands to the par-shell "server" */
	char* inputVector[INPUTVECTOR_SIZE] = {};
	if(argc != 2){ /* If the number of specified arguments is incorrect, the program won't run.  */
		defaultErrorBehavior(ERR_WRONGARGUMENTS);
		exit(EXIT_FAILURE);
	}
	writePipe = argv[1]; 

	printf("par-shell@%s $ ", writePipe);
	readLineArguments(inputVector, INPUTVECTOR_SIZE);
	while(!inputVector[0] || (commandType(inputVector[0]) == NORMAL_COMMAND)){
		printf("Command: %s\n", inputVector[0]);
		printf("par-shell@%s $ ", writePipe);
		readLineArguments(inputVector, INPUTVECTOR_SIZE);
	}

	/* Handle different exit commands */

	return 0;
}
