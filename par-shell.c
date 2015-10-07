#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include "commandlinereader.h"
#include "list.h"
#include "time_helper.h"

#define PATHNAME_MAX_ARGS 5

int main(int argc, char* argv[]){
	/* Aux variables used along the program */
	int i = 0, pid = 0, status = 0;

	/* Saves fork()'s return value */
	int forkId;

	/** 
	 * Declares the vector we use to store inputs and sets all positions to NULL 
	 * 0th index is the program's name, followed by it's arguments (max 5)
	 * The index after the last argument is set to NULL.
	 **/
	char* inputVector[PATHNAME_MAX_ARGS+2] = {};

	/** 
	 * Initializes a list where we store the processes ran by the shell
	 * along with some information about each process. (check list.h)
	 **/
	list_t *processList = lst_new(); 

	if(!processList){
		fprintf(stderr, "Couldn't allocate enough memory to save a process list\n");
		exit(EXIT_FAILURE);
	}

	/**
	 * Reads user input, and tries to start a process running
	 * the user specified program. If an error is detected
	 * it writes into the stderr stream. 
	 * Repeats until user input is the exit command.
	 **/
	readLineArguments(inputVector, PATHNAME_MAX_ARGS+2);
	while(!inputVector[0] || strcmp(inputVector[0], "exit")){
		if(inputVector[0] != NULL){
			forkId = fork();

			if(forkId < 0){
				/* Couldn't fork - maybe not enough memory? */
				fprintf(stderr, "Couldn't fork\n");
			}
			else if (forkId > 0){
				/**
				 * Runs the parent process' code:
				 * Saves the child process info in a list and frees
				 * the allocated memory used to save the user input.
				 * 
				 * WARNING: If there isn't enough memory to keep track of the child process,
				 *			meaning we won't be able to save it on our list, it still executes.
				 **/
				if(!insert_new_process(processList, forkId, GET_CURRENT_TIME(), inputVector[0])) 
					fprintf(stderr, "Child with PID:%d was lost because "
									"you didn't have enough memory to save it, "
									"it's still running.\n", forkId);
				free(inputVector[0]);
			}
			else{
				/**
				 * Runs the child process' code:
				 * Tries to execute the program the user specified with respective arguments
				 * 
				 * WARNING: If the program wasn't found,
				 *		    the process exits with EXIT_FAILURE.
				 **/
				execv(inputVector[0], inputVector);
				fprintf(stderr, "%s: program not found\n", inputVector[0]);
				exit(EXIT_FAILURE);
			}
		}
		readLineArguments(inputVector, PATHNAME_MAX_ARGS+2);
	}

	/* Frees the last user input - the exit command */
	free(inputVector[0]);

	/**
	 * Waits for all the kids to complete and gets all their statuses, 
	 * including zombies, and updates their data in the process list.
	 **/
	for(i = lst_sizeof(processList); i>0; i--){
		pid = wait(&status);
		update_terminated_process(processList, pid, GET_CURRENT_TIME(), WEXITSTATUS(status));
	}

	/* Prints info about every chil process to the user */
	lst_print(processList);

	/* Deallocates all memory for the process list. */
	lst_destroy(processList);

	return 0;
}