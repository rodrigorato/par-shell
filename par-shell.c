#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <pthread.h>
#include <semaphore.h>
#include "commandlinereader.h"
#include "list.h"
#include "time_helper.h"

#define PATHNAME_MAX_ARGS 5 /* Program can be ran with 5 arguments */
#define INPUTVECTOR_SIZE PATHNAME_MAX_ARGS+2 /* vector[0] = program name; vector[-1] = NULL */
#define MAXPAR 4 /* Set it to the number of cores in your machine. */

pthread_mutex_t g_condMutex;
pthread_cond_t g_canWaitProcess, g_canRunProcess;
int g_runningProcesses = 0;

void *gottaWatchEmAll(void *voidList){
	/**
	 *	Watcher thread:
	 *	Runs in parallel with the main thread,
	 *  waiting out on all child processes
	 *	and keeping track of their end times,
	 * 	while it updates the list. 
	 *
	 *	We use the list's (un)lock functions 
	 *  (inner list mutex implementations)
	 *  to read and write to it.
	 **/
	list_t* processList = (list_t*) voidList;
	int pid=0, status=0;

	while(1){
		if(pthread_mutex_lock(&g_condMutex))
			fprintf(stderr, "Couldn't lock a condition variable associated mutex!\n");
		while(g_runningProcesses == 0)
			if(pthread_cond_wait(&g_canWaitProcess, &g_condMutex))
				fprintf(stderr, "Couldn't wait on a condition variable!\n");
		if(pthread_mutex_unlock(&g_condMutex))
			fprintf(stderr, "Couldn't unlock a condition variable related mutex!\n");

		lst_lock(processList);
		if((lst_numactive(processList) == 0) && lst_isfinal(processList)){
			lst_unlock(processList);
			pthread_exit(NULL);
		}
		lst_unlock(processList);
		
		pid = wait(&status);

		if(pthread_mutex_lock(&g_condMutex))
			fprintf(stderr, "Couldn't lock a condition variable related mutex!\n");
		g_runningProcesses--;
		if(pthread_cond_signal(&g_canRunProcess))
			fprintf(stderr, "Couldn't signal a condition variable!\n");
		if(pthread_mutex_unlock(&g_condMutex))
			fprintf(stderr, "Couldn't unlock a condition variable related mutex!\n");


		lst_lock(processList);
		update_terminated_process(processList, pid, GET_CURRENT_TIME(), status);
		lst_unlock(processList);
	}
}


int main(int argc, char* argv[]){
	int forkId; // Saves fork()'s return value 
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

	/* Initializes the mutex associated with the condition variables. */
	if(pthread_mutex_init(&g_condMutex, NULL))
		fprintf(stderr, "Couldn't create a mutex to keep track of the condition variables!\n");

	/* Initializes the condition variables used to fork()/wait() on processes. */
	if(pthread_cond_init(&g_canRunProcess, NULL))
		fprintf(stderr, "Couldn't initialize a condition variable!\n");
	if(pthread_cond_init(&g_canWaitProcess, NULL))
		fprintf(stderr, "Couldn't initialize a condition variable!\n");
	

	/* Verifies if the list was sucessfully initiated */
	if(!processList){
		fprintf(stderr, "Couldn't allocate enough memory to save a process list\n");
		exit(EXIT_FAILURE);
	}

	/* Tries to create the thread that will keep track of childs */
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
			
			/* Waits while we can't run any more processes */			
			if(pthread_mutex_lock(&g_condMutex))
				fprintf(stderr, "Couldn't lock a condition variable related mutex!\n");

			while(g_runningProcesses >= MAXPAR)
				if(pthread_cond_wait(&g_canRunProcess, &g_condMutex))
					fprintf(stderr, "Couldn't wait on a condition variable!\n");

			if(pthread_mutex_unlock(&g_condMutex))
				fprintf(stderr, "Couldn't unlock a condition variable associated mutex!\n");

			/* Locking list because we need to ensure that child is inserted in list */
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
				
				/* Signals so that the monitoring thread can wait() on the ran processes. */
				if(pthread_mutex_lock(&g_condMutex))
					fprintf(stderr, "Couldn't lock a condition variable related mutex!\n");
				
				g_runningProcesses++;

				if(pthread_cond_signal(&g_canWaitProcess))
					fprintf(stderr, "Couldn't signal a condition variable!\n");

				if(pthread_mutex_unlock(&g_condMutex))
					fprintf(stderr, "Couldn't unlock a condition variable related mutex!\n");

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
			/* We unlock because we ensured that the monitor thread couldn't write
		   	on list, even if the child was already a zombie */
			lst_unlock(processList);
		}

		readLineArguments(inputVector, INPUTVECTOR_SIZE);
	}

	lst_lock(processList);

	/* Commands to finalize list*/
	lst_finalize(processList);

	lst_unlock(processList);

	/* We post one last time so the thread can get to pthread_exit*/
	if(pthread_mutex_lock(&g_condMutex))
		fprintf(stderr, "Couldn't lock a condition variable related mutex!\n");
				
	g_runningProcesses++;

	if(pthread_cond_signal(&g_canWaitProcess))
		fprintf(stderr, "Couldn't signal a condition variable!\n");

	if(pthread_mutex_unlock(&g_condMutex))
		fprintf(stderr, "Couldn't unlock a condition variable related mutex!\n");


	/* Frees the last user input - the exit command */
	free(inputVector[0]);

	if(pthread_join(watcherThread, NULL))
		fprintf(stderr, "Error on pthread_join\n");

	while(pthread_mutex_trylock(&g_condMutex)); //will lock on trylock failure
  	if(pthread_mutex_unlock(&g_condMutex))
  		fprintf(stderr, "Couldn't unlock a condition variable related mutex!\n");
  	if(pthread_mutex_destroy(&g_condMutex))
  		fprintf(stderr, "Couldn't destroy a condition variable associated mutex!\n");

 	if(pthread_cond_destroy(&g_canWaitProcess))
 		fprintf(stderr, "Couldn't destroy a condition variable!\n");
 	
 	if(pthread_cond_destroy(&g_canRunProcess))
 		fprintf(stderr, "Couldn't destroy a condition variable!\n");

	/* Prints info about every child process to the user */
	lst_print(processList);

	/* Deallocates all memory for the process list. */
	lst_destroy(processList);

	return 0;
}
