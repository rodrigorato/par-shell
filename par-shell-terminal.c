#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> 
#include <signal.h>
#include "commandlinereader.h"
#include "error_helper.h"

/**
 *  All the macros (and error preventing functions) we use
 *  in this program are defined in error_helper.h,
 *  please do check that file.
 **/

int writePipeDescriptor, statsFileDescriptor;

void interruptionExit(int s){
	char buffer[MAX_BUF];
	sprintf(buffer, "%s %d\n", CLOSINGTERMINAL, getpid());
	errWriteToPipe(buffer, writePipeDescriptor);
	close(writePipeDescriptor);
	exit(EXIT_SUCCESS);
}

int specialCommand(char* command){
	/* We need to add the '\n' chars because fgets puts them there */
	if(!strcmp(command, "exit\n"))
		return EXIT_COMMAND;
	if(!strcmp(command, "exit-global\n"))
		return EXIT_GLOBAL_COMMAND;
	if(!strcmp(command, "stats\n"))
		return STATS_COMMAND;
	return NORMAL_COMMAND;
}

int main(int argc, char** argv){
	char* writePipe; /* The name of the pipe used to send commands to the par-shell "server" */
	char inputString[MAX_BUF], buffer[MAX_BUF], message[MAX_BUF];
	if(argc != 2){ /* If the number of specified arguments is incorrect, the program won't run.  */
		defaultErrorBehavior(ERR_WRONGARGUMENTS);
		exit(EXIT_FAILURE);
	}
	writePipe = argv[1];
	//apanhar erros
	writePipeDescriptor = open(writePipe, O_WRONLY);
	signal(SIGINT, interruptionExit);

	/* Sends a message with this terminal's PID to the main program */
	sprintf(buffer, "%s %d\n", NEWTERMINALID, getpid());
	errWriteToPipe(buffer, writePipeDescriptor);
	printf("par-shell@%s $ ", writePipe);
	while((!specialCommand(inputString) || specialCommand(inputString) == STATS_COMMAND) && fgets(inputString, MAX_BUF, stdin)){
		switch(specialCommand(inputString)){
			case NORMAL_COMMAND:
				/* Perform a normal command - send it to par-shell so it execs it */
			 	errWriteToPipe(inputString, writePipeDescriptor);
				printf("par-shell@%s $ ", writePipe);
				break;

			case STATS_COMMAND:
				sprintf(buffer, "%s %d\n", TERMINALSTATS, getpid());
				errWriteToPipe(buffer, writePipeDescriptor);
				sprintf(buffer, "%s%s%d", STATSDIR, TERMINALSTATS, getpid());
				mkfifo(buffer, 0666);
				statsFileDescriptor = open(buffer, O_RDONLY);
				errReadFromPipe(message, statsFileDescriptor, MAX_BUF);
				printf("--PAR-SHELL STATS--\n");
				printf("%s\n", message);
				close(statsFileDescriptor);
				unlink(buffer);			
				printf("par-shell@%s $ ", writePipe);	
				break;

			case EXIT_COMMAND:
				/* Perform an exit command - close this terminal program only */
				break;

			case EXIT_GLOBAL_COMMAND:
				/* Perform a exit-global command - tell par-shell to kill all terminals via signal */
				errWriteToPipe("exit\n", writePipeDescriptor);
				break;
		}
	}

	interruptionExit(0);
}
