#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <wait.h>
#include "commandlinereader.h"
#include "list.h"

#define PATHNAME_MAX_ARGS 5

int main(int argc, char* argv[]){
	/* Aux variable to wait for them child processes */
	int i = 0, pid = 0, status = 0;

	/* Assumes fork returns something */
	int forkId;

	/* Declares the vector we use to store inputs and sets all positions to NULL */
	char* inputVector[PATHNAME_MAX_ARGS+2] = {};

	/* Initializes a list where we store the processes ran by the shell */
	list_t *processList = lst_new(); 

	if(!processList){
		perror("WHY YOU NO MEMORY?!?\n");
		return 0;
	}

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
			/* time(NULL) returns the actual time in time_t struct, from time.h*/
			insert_new_process(processList, forkId, time(NULL)); // proteger isto
		}
		else{
			/* HOW CAN WE VERIFY IF PATHNAME IS VALID? - we dont*/
			/* Child process' code */
			execv(inputVector[0], inputVector);
			perror("oops\n");
			//exit(0);
			exit(EXIT_FAILURE);
		}

		

		readLineArguments(inputVector, PATHNAME_MAX_ARGS+2);
	}

	/*
	for(i=0; i<2; i++){
			printf("%s\n", inputVector[i]);
			free(inputVector[i]);
	}
	*/


	for(i = lst_sizeof(processList); i>0; i--){
		/* must verify if child was sucessful */
		pid = wait(&status);
		if (WIFEXITED(status)){
			if (WEXITSTATUS(status) == EXIT_SUCCESS){
				printf("pid:%d\texit-code:%d\n", pid, status);
				/* update_terminated_process(processList, pid, time(NULL));
				   they terminate all at the same time */
			}	
		}
	}
	/* to check start and end times 
	lst_print(processList); */
	lst_destroy(processList);

	return 0;
}