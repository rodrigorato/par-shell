#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "commandlinereader.h"
#include "list.h"

int main(){
	char* argv[6]; argv[0] = NULL;
	int read = 0, i;
	read = readLineArguments(argv, 5);

	printf("Number of read arguments: %d\n", read);

	for(i=0; i<read; i++)
		printf("%s\n", argv[i]);

	return 0;
}