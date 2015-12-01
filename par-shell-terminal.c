#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commandlinereader.h"
#include "error_helper.h"

/**
 *  All the macros (and error preventing functions) we use
 *  in this program are defined in error_helper.h,
 *  please do check that file.
 **/

int specialCommand(char* command){
	/* We need to add the '\n' chars because fgets puts them there */
	if(!strcmp(command, "exit\n"))
		return EXIT_COMMAND;
	if(!strcmp(command, "exit-global\n"))
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
				/* Perform a normal command - send it to par-shell so it execs it */
			 	errWriteToPipe(inputString, writePipeDescriptor);
				printf("par-shell@%s $ ", writePipe);
				break;

			case EXIT_COMMAND:
				/* Perform an exit command - close this terminal program only */
				break;

			case EXIT_GLOBAL_COMMAND:
				/* Perform a exit-global command - tell par-shell to kill all terminals via signal */
				errWriteToPipe("exit", writePipeDescriptor);
				break;
		}
	}

	close(writePipeDescriptor);
	/* Handle different exit commands */

	return 0;
}
