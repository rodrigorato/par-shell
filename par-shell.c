#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "commandlinereader.h"
#include "list.h"

#define PATHNAME_MAX_ARGS 5

int main(int argc, char* argv[]){
	char* argvec[PATHNAME_MAX_ARGS+1]; argvec[0] = NULL;
	int read = 0, i;
	read = readLineArguments(argvec, PATHNAME_MAX_ARGS+1);

	printf("Number of read arguments: %d\n", read);

	for(i=0; i<read; i++)
			printf("%s\n", argvec[i]);

	return 0;
}