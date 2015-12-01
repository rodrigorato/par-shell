#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commandlinereader.h"
#include "error_helper.h"

#define MAX_BUF 1024 /* The communication's max buffer size */
#define PATHNAME_MAX_ARGS 5 /* Program can be ran with 5 arguments */
#define INPUTVECTOR_SIZE PATHNAME_MAX_ARGS+2 /* vector[0] = program name; vector[-1] = NULL */

int main(int argc, char** argv){
	int writePipeDescriptor;
	char* writePipe; /* The name of the pipe used to send commands to the par-shell "server" */
	char inputVector[MAX_BUF];
	if(argc != 2){ /* If the number of specified arguments is incorrect, the program won't run.  */
		defaultErrorBehavior(ERR_WRONGARGUMENTS);
		exit(EXIT_FAILURE);
	}
	writePipe = argv[1];
	//apanhar erros
	writePipeDescriptor = open(writePipe, O_WRONLY);

	printf("par-shell@%s $ ", writePipe);
	while(fgets(inputVector, MAX_BUF, stdin)){
		errWriteToPipe(inputVector, writePipeDescriptor);
		printf("par-shell@%s $ ", writePipe);
	}

	close(writePipeDescriptor);
	/* Handle different exit commands */

	return 0;
}
