#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <pthread.h>
#include "commandlinereader.h"
#include "list.h"
#include "time_helper.h"

#define PATHNAME_MAX_ARGS 5 /* Program can be ran with 5 arguments */
#define INPUTVECTOR_SIZE PATHNAME_MAX_ARGS+2 /* vector[0] = program name; vector[-1] = NULL */

void *gottaWatchEmAll(void *voidList){
	list_t* processList = (list_t*) voidList;
	int pid=0, status=0;
	while(1){
		lst_lock(processList);
		if(lst_numactive(processList) == 0){
			if(lst_isfinal(processList)){
				lst_unlock(processList);
				return NULL; // pthread_exit() ou isto?? pls halp
			}
			lst_unlock(processList);
			sleep(1);
		}
		else{
			lst_unlock(processList);
			pid = wait(&status);
			lst_lock(processList);
			update_terminated_process(processList, pid, GET_CURRENT_TIME(), status);
			lst_unlock(processList);
		}
	}
}


int main(int argc, char* argv[]){
	/* Saves fork()'s return value */
	int forkId;
	pthread_t watcherThread;	

	/** 
	 * Declares the vector we use to store inputs and sets all positions to NULL 
	 * 0th index is the program's name, followed by it's arguments (max 5)
	 * The index after the last argument is set to NULL.  
	 **/
	char* inputVector[INPUTVECTOR_SIZE] = {};

	/** 
	 * Initializes a list where we store the processes ran by the shell
	 * along with some information about each process. (check list.h)
	 **/
	list_t *processList = lst_new(); 

	if(!processList){
		fprintf(stderr, "Couldn't allocate enough memory to save a process list\n");
		exit(EXIT_FAILURE);
	}

	if(pthread_create(&watcherThread, 0, gottaWatchEmAll,(void *)processList)){
		fprintf(stderr, "Couldn't create a watcher thread\n");
		exit(EXIT_FAILURE);
	}

	/**
	 * Reads user input, and tries to start a process running
	 * the user specified program. If an error is detected
	 * it writes into the stderr stream. 
	 * Repeats until user input is the exit command.
	 **/
	readLineArguments(inputVector, INPUTVECTOR_SIZE);
	while(!inputVector[0] || strcmp(inputVector[0], "exit")){
		/* If the user presses enter we just stand-by to read his input again */
		if(inputVector[0] != NULL){
			/*Locking list because we need to ensure that child is inserted in list*/
			lst_lock(processList);
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
				fprintf(stderr, "%s: couldn't execv\n", inputVector[0]);
				exit(EXIT_FAILURE);
			}
		}
		/*We unlock because we ensured that the monitor thread couldnt write
		  on list, even if the kid was already zombie*/
		lst_unlock(processList);
		readLineArguments(inputVector, INPUTVECTOR_SIZE);
	}

	/* Frees the last user input - the exit command */
	lst_finalize(processList);
	free(inputVector[0]);
	pthread_join(watcherThread, NULL);

	/* Prints info about every child process to the user */
	lst_print(processList);

	/* Deallocates all memory for the process list. */
	lst_destroy(processList);

	return 0;
}
