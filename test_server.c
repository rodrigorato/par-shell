#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "error_helper.h"

#define MAX_BUF 1024 /* The communication's max buffer size */
#define PATHNAME_MAX_ARGS 5 /* Program can be ran with 5 arguments */
#define INPUTVECTOR_SIZE PATHNAME_MAX_ARGS+2 /* vector[0] = program name; vector[-1] = NULL */
#define INPUTPIPENAME "par-shell-in" /* The input pipe's name */

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

int main(){
	int i;
    int inputPipeDescriptor;
    char *inputPipe = INPUTPIPENAME;

    /* create the FIFO (named pipe) */
    mkfifo(inputPipe, 0666);

    char* buf[INPUTVECTOR_SIZE];
    for(i = 0; i < INPUTVECTOR_SIZE; i++) buf[i] = (char*) malloc(sizeof(char)*MAX_BUF);

    /* open, read, and display the message from the FIFO */
    inputPipeDescriptor = open(inputPipe, O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR);
    do{
	    errReadFromPipe(buf, inputPipeDescriptor, MAX_BUF);
	    // TEST

	    for(i = 0; strcmp(buf[i], "\0") && i < INPUTVECTOR_SIZE; i++)
	    	printf("no server: %s\n", buf[i]);
            
    	// TEST
	}while(commandType(buf[0]) == NORMAL_COMMAND);

    for(i = 0; i < INPUTVECTOR_SIZE; i++) free(buf[i]);
    close(inputPipeDescriptor);
    unlink(inputPipe);

    return 0;
}
