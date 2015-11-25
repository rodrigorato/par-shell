#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <fnctl.h>
#include "error_helper.h"
#include "pipes.h"

/* Writes a char array to a named pipe to be read later */
void writeToPipe(char** sendv, int pipeDescriptor){
	int i;
	for(i = 0; sendv[i] != NULL; i++)
		if(write(pipeDescriptor, sendv[i], sizeof(sendv[i])) == -1)
			defaultErrorBehavior(ERR_WRITETOPIPE);
	if(write(pipeDescriptor, NULL, sizeof(NULL)) == -1)
		defaultErrorBehavior(ERR_WRITETOPIPE);
}

/* Reads a char array from a named pipe, sent by the function above */
void readFromPipe(char** readv, int pipeDescriptor, int maxBufSize){
	int i;
	for(i = 0; sendv[i] != NULL; i++)
		if(read(pipeDescriptor, readv[i], maxBufSize) == -1)
			defaultErrorBehavior(ERR_READFROMPIPE);
	readv[i] = NULL;
}