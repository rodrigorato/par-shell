#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "commandlinereader.h"
#include "list.h"

#define PATHNAME_MAX_ARGS 5

int main(int argc, char* argv[]){
	/* Aux variable to wait for them child processes */
	int i = 0, pid = 0, status = 0;

	/* Assumes fork returns something */
	int forkId;

	/* Initializes a list where we store the processes ran by the shell */
	list_t *processList = lst_new(); 

	if(!processList){
		perror("WHY YOU NO MEMORY?!?\n");
		return 0;
	}

	/* Declares the vector we use to store inputs and sets the 0th position to NULL */
	char* inputVector[PATHNAME_MAX_ARGS+2]; inputVector[0] = NULL; 
	

	readLineArguments(inputVector, PATHNAME_MAX_ARGS+2);
	/* Cheeky OR ;) */
	while(!inputVector[0] || strcmp(inputVector[0], "exit")){
		forkId = fork();

		if(forkId < 0){
			/* Houston, we got a problem! */
			perror("fork() it all!\n");
		}
		else if (forkId > 0){
			/* Parent process' code */
			insert_new_process(processList, forkId, time(NULL)); // proteger isto
		}
		else{
			/* Child process' code */
			execv(inputVector[0], inputVector);
			perror("oops\n");
			exit(0);
		}


		readLineArguments(inputVector, PATHNAME_MAX_ARGS+2);
	}

	//does end program stuff
	
	for(i = lst_sizeof(inputVector); i>0; i--){
		pid = wait(&status);
		printf("pid:%d\texit-code:%d\n", pid, status);
	}

	lst_destroy(inputVector);

	return 0;
}