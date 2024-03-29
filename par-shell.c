#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <pthread.h>
#include <fcntl.h> /* open flags */
#include <sys/stat.h> /* open mode_t */
#include "commandlinereader.h"
#include "list.h"
#include "time_helper.h"
#include "helper.h"

/**
 *  All the macros (and error preventing functions) we use
 *  in this program are defined in error_helper.h,
 *  please do check that file.
 **/

pthread_mutex_t g_condMutex;
pthread_cond_t g_canWaitProcess, g_canRunProcess;
int g_runningProcesses = 0, totalExecutionTime = 0;
int currentIteration = -1; // we start at iteration0;
FILE* logfile;
FILE* statsfile;
list_t *terminalList;
list_t *processList;
pthread_t watcherThread;
pthread_t statsThread;

/**
 * Declares the vector we use to store inputs and sets all positions to NULL
 * 0th index is the program's name, followed by it's arguments (max 5)
 * The index after the last argument is set to NULL.
 **/
char* inputVector[INPUTVECTOR_SIZE] = {};

void writeStats(){
	/* Will write the stats needed for the terminals into the file specified by STATSFILE */
	int runningProcesses, execTime;
	statsfile = errFOpen(STATSFILE, "w");
	errMutexLock(&g_condMutex, ERR_LOCKMUTEX);
	runningProcesses = g_runningProcesses;
	execTime = totalExecutionTime;
	errMutexUnlock(&g_condMutex,ERR_UNLOCKMUTEX);
	if(!fprintf(statsfile, "Active processes count: \t%d\nTotal execution time: \t\t%d\n", runningProcesses, execTime))
		defaultErrorBehavior(ERR_WRITETOFILE);
 	errFflush(statsfile);
 	errFClose(statsfile);
}

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
	int i = 0;
	lst_iitem_t* waitedProcess;

	while(1){
		/* Waits until theres is a process to wait() on. */
		errMutexLock(&g_condMutex, ERR_LOCKCONDVARMUTEX);
		while(g_runningProcesses == 0)
			errCondVarWait(&g_canWaitProcess, &g_condMutex, ERR_WAITCONDVAR);
		errMutexUnlock(&g_condMutex, ERR_UNLOCKCONDVARMUTEX);

		/* Checks if the user has prompted to exit the program */
		lst_lock(processList);
		if((lst_numactive(processList) == 0) && lst_isfinal(processList)){
			lst_unlock(processList);
			pthread_exit(NULL);
		}
		lst_unlock(processList);

		pid = wait(&status);


		/* Signals the main thread as it can now run one more process. */
		errMutexLock(&g_condMutex, ERR_LOCKCONDVARMUTEX);
		g_runningProcesses--;
		errCondVarSignal(&g_canRunProcess, ERR_SIGNALCONDVAR);
		errMutexUnlock(&g_condMutex, ERR_UNLOCKCONDVARMUTEX);

		/* Saves the process it got through the wait() in a list. Updating Execution time. */
		lst_lock(processList);
		waitedProcess = update_terminated_process(processList, pid, GET_CURRENT_TIME(), status);

		i = GET_DURATION_TIME(waitedProcess->endtime, waitedProcess->starttime);
		totalExecutionTime+=i;
		lst_unlock(processList);

		/* Writting the information into the log, if any couldnt write , the program ends. */
		if(!fprintf(logfile, "iteracao %d\n", currentIteration))
				defaultErrorBehavior(ERR_WRITETOFILE);
		if(!fprintf(logfile, "pid: %d execution time %d s\n", pid, i))
				defaultErrorBehavior(ERR_WRITETOFILE);
		if(!fprintf(logfile, "total execution time: %d s\n", totalExecutionTime))
			defaultErrorBehavior(ERR_WRITETOFILE);

		/* Guarantees the info is on the file as soon as the process ends */
 		errFflush(logfile);

		currentIteration++;
		writeStats();
	}
}

void killAllTerminals(int s){
	/** 
	 *	Kills all running terminals and deallocates
	 * 	all the memory for the list. 
	 *  Program exiting routine.
	 **/
	int pid;

	/* This loop will go through all the known terminals sending them kill signals */
	while((pid = lst_pop(terminalList)))
		kill(pid, SIGKILL);

	lst_destroy(terminalList);

	free(inputVector[0]);
	
	/* Unlinks the named pipe used for reading inputs */
	errUnlink(INPUTPIPENAME);

	/* Commands to finalize list*/
	lst_lock(processList);
	lst_finalize(processList);
	lst_unlock(processList);

	/* We post one last time so the thread can get to pthread_exit*/
	errMutexLock(&g_condMutex, ERR_LOCKCONDVARMUTEX);
	g_runningProcesses++;
	errCondVarSignal(&g_canWaitProcess, ERR_SIGNALCONDVAR);
	errMutexUnlock(&g_condMutex, ERR_UNLOCKCONDVARMUTEX);

	/* Waiting on watcherThread to end */
	if(pthread_join(watcherThread, NULL))
		defaultErrorBehavior(ERR_THREADJOIN);

	/* Destroying mutex and Condition Variables */
	while(pthread_mutex_trylock(&g_condMutex));
  	errMutexUnlock(&g_condMutex, ERR_UNLOCKCONDVARMUTEX);
  	errMutexDestroy(&g_condMutex, ERR_DESTROYCONDVARMUTEX);
  	errCondVarDestroy(&g_canWaitProcess, ERR_DESTROYCONDVAR);
 	errCondVarDestroy(&g_canRunProcess, ERR_DESTROYCONDVAR);

 	/* Closing the log file */
 	errFClose(logfile);
	
	/* Unlinks the stats file */
	errUnlink(STATSFILE);

	/* Prints info about every child process to the user */
	lst_print(processList);

	/* Deallocates all memory for the process list. */
	lst_destroy(processList);
	exit(EXIT_SUCCESS);
}


int main(int argc, char* argv[]){
	int inputPipeDescriptor, outputFileDescriptor, stdinRedirect, i, forkId, procTime = 0;

	/**
	 * Initializes a list where we store the processes ran by the shell
	 * along with some information about each process. (check list.h)
	 * And a list for every terminal associated with this instance 
	 * of the par-shell program.
	 **/
	processList = lst_new();
	terminalList = lst_new();

	/* At this moment, if a SIGINT is received, this will proceed to exit normally */
	errSignal(SIGINT, killAllTerminals);

	/* Verifies if the lists were sucessfully initiated */
	if(!processList)
		defaultErrorBehavior(ERR_ALLOCLIST);
	if(!terminalList)
		defaultErrorBehavior(ERR_ALLOCLIST);

	/* Here we declare 3 log lines so we can read the log file. */
	char logLine1[MAXLOGLINESIZE], logLine2[MAXLOGLINESIZE], logLine3[MAXLOGLINESIZE];

	/* Open the log file for reading and appending */
	logfile = errFOpen("log.txt", "a+");

	/*gets current iteration and total execution time from the file and changes in our variables*/
	while(fgets(logLine1, MAXLOGLINESIZE, logfile) &&
		  fgets(logLine2, MAXLOGLINESIZE, logfile) &&
		  fgets(logLine3, MAXLOGLINESIZE, logfile)){
		sscanf(logLine1, "%s %d", logLine2, &currentIteration); // we don't really need the info on logLine2
		sscanf(logLine3, "%[^:]: %d %s", logLine2, &procTime, logLine2); // so we just use logLine2 as a buffer
	}
	totalExecutionTime+=procTime;
	currentIteration++;

	/* Initializes the mutex associated with the condition variables. */
	errMutexInit(&g_condMutex, ERR_INITCONDVARMUTEX);

	/* Initializes the condition variables used to fork()/wait() on processes. */
	errCondVarInit(&g_canRunProcess, ERR_INITCONDVAR);
	errCondVarInit(&g_canWaitProcess, ERR_INITCONDVAR);


	/* Tries to create the thread that will keep track of childs */
	if(pthread_create(&watcherThread, 0, gottaWatchEmAll,(void *)processList))
		defaultErrorBehavior(ERR_WATCHERTHREADCREATE);

	/**
	 * Initializes a named pipe, opens it (eventually locking on open)
	 * and dups it to stdin so we can read from it instead of stdin directly.
	 **/
	if(mkfifo(INPUTPIPENAME, 0666))
		defaultErrorBehavior(ERR_MKFIFO);

	/* Tries to open the communication pipe */
	inputPipeDescriptor = errOpen(INPUTPIPENAME, O_RDONLY);

	/* Redirection of stdin to the pipe, by closing stdin and duplicating the open pipe */
	errClose(fileno(stdin));
	stdinRedirect = errDup(inputPipeDescriptor);
	errClose(inputPipeDescriptor);

	

	/**
	 * Reads input from the communication pipe, and tries to start a process running
	 * the specified program.
	 * Repeats until one of the terminals sends a exit-global.
	 **/
	readLineArguments(inputVector, INPUTVECTOR_SIZE);
	while(!inputVector[0] || strcmp(inputVector[0], "exit")){

		/* If a new terminal id started we insert him in the list */
		if(inputVector[0] && !strcmp(inputVector[0], NEWTERMINALID)){
			if(!lst_push(terminalList, atoi(inputVector[1])))
				defaultErrorBehavior(ERR_LISTINSERT);
			printf("New terminal launched - pid: %d\n", atoi(inputVector[1]));
			free(inputVector[0]);
			inputVector[0] = NULL; /* Prevents it from trying to exec this command */
		}

		/* If an existing terminal was closed we remove him from the list */
		if(inputVector[0] && !strcmp(inputVector[0], CLOSINGTERMINAL)){
			if(!lst_remove(terminalList, atoi(inputVector[1])))
				defaultErrorBehavior(ERR_LISTREMOVE);
			printf("Terminal terminated connection - pid: %d\n", atoi(inputVector[1]));
			if(lst_sizeof(terminalList) == 0){
				/* If there are no more open terminals we redirect again */
				inputPipeDescriptor = errOpen(INPUTPIPENAME, O_RDONLY);
				errClose(stdinRedirect);
				errDup(inputPipeDescriptor);
				errClose(inputPipeDescriptor);			
			}
			free(inputVector[0]);
			inputVector[0] = NULL; /* Prevents it from trying to exec this command */
		}

		/* If any of the terminals presses enter we just stand-by to read input again */
		if(inputVector[0] != NULL){

			/* Prints what was read and his arguments */
			printf("Executing program: %s", inputVector[0]);
			for(i = 1; inputVector[i] != NULL; i++)
				printf(" %s", inputVector[i]);
			printf("\n");

			/* Waits while we can't run any more processes */
			errMutexLock(&g_condMutex, ERR_LOCKCONDVARMUTEX);
			while(g_runningProcesses >= MAXPAR)
				errCondVarWait(&g_canRunProcess, &g_condMutex, ERR_WAITCONDVAR);
			errMutexUnlock(&g_condMutex, ERR_UNLOCKCONDVARMUTEX);

			/* Locking list because we need to ensure that child is inserted in list */
			lst_lock(processList);

			forkId = fork();

			if(forkId < 0)
				defaultErrorBehavior(ERR_FORK);

			else if (forkId > 0){
				/**
				 * Runs the parent process' code:
				 * Saves the child process info in a list and frees
				 * the allocated memory used to save the user input.
				 **/
				if(!insert_new_process(processList, forkId, GET_CURRENT_TIME(), inputVector[0]))
					defaultErrorBehavior(ERR_LISTINSERT);

				/* Signals so that the monitoring thread can wait() on the ran processes. */
				errMutexLock(&g_condMutex, ERR_LOCKCONDVARMUTEX);
				g_runningProcesses++;
				errCondVarSignal(&g_canWaitProcess, ERR_SIGNALCONDVAR);
				errMutexUnlock(&g_condMutex, ERR_UNLOCKCONDVARMUTEX);

				free(inputVector[0]);
			}
			else{
				/**
				 * Runs the child process' code:
				 * Tries to execute the program the user specified with respective arguments
				 *
				 * WARNING: If the program wasn't found,
				 *		    the process exits with EXIT_FAILURE.
				 *
				 * We will now redirect the child process's output to a file named:
				 * par-shell-out-PID.txt : PID = the child processes pid
				 **/
				char name_string[MAXFILENAMELENGTH];
				sprintf(name_string, "par-shell-out-%d.txt", getpid());
				outputFileDescriptor = errOpenPerms(name_string, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);

				/* Used to make the child processes ignore the SIGINT signal */
				errSignal(SIGINT, SIG_IGN);

	 			close(fileno(stdout));
	 			dup(outputFileDescriptor);
	 			close(outputFileDescriptor);

				execv(inputVector[0], inputVector);
				defaultErrorBehavior(ERR_EXECV);
			}
			/* We unlock because we ensured that the monitor thread couldn't write
		   	on list, even if the child was already a zombie */
			lst_unlock(processList);
		}
		writeStats();
		readLineArguments(inputVector, INPUTVECTOR_SIZE);
	}

	/* Normal exit */
	killAllTerminals(0);
	return 0;
}
