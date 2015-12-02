#ifndef __ERROR_HELPER_H__
#define __ERROR_HELPER_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>

#define PATHNAME_MAX_ARGS 5 /* Program can be ran with 5 arguments */
#define INPUTVECTOR_SIZE PATHNAME_MAX_ARGS+2 /* vector[0] = program name; vector[-1] = NULL */
#define MAXPAR 4 /* Set it to the number of cores in your machine. */
#define MAX_BUF 1024 /* The communication's max buffer size */
#define MAXLOGLINESIZE 256
#define MAXFILENAMELENGTH 28 /* The max filename length */
#define INPUTPIPENAME "/tmp/par-shell-in" /* The input pipe's name */
#define NEWTERMINALID "new_terminal_pid"
#define CLOSINGTERMINAL "closing_terminal_pid"
#define NORMAL_COMMAND 0
#define EXIT_COMMAND 1
#define EXIT_GLOBAL_COMMAND 2 


/**
 *	This file and corresponding .c are used
 *  to call every function that may cause an error.
 *  As we aren't checking the errno variable for each
 *  error and the way we manage the errors is pretty simple
 *  this definitions come in handy so we don't get repetitive
 *	and messy (random strings everywhere) code.
 *
 *	This also allows for a more flexible error managing as all
 *  the strings we print out are in one and only one place.
 *
 *  We also keep all the macros that are common to all the programs
 *  in this project here, guarantees they are always the same and don't repeat.
 *
 *	ALL THESE FUNCTIONS, UNLESS SPECIFIED OTHERWISE,
 *	RUN defaultErrorBehavior() TO HANDLE ERRORS, IF THERE ARE ANY.
 */

/**
 * 	This is called to handle an error. It prints the message to stderr.
 * 	For more details check the function's definition,
 * 	changing it will change the way EVERY error is handled.
 **/
void defaultErrorBehavior(char* message);

/* Initializes a mutex with NULL attr */
void errMutexInit(pthread_mutex_t* mutex, char* message);

/* Locks a mutex */
void errMutexLock(pthread_mutex_t* mutex, char* message);

/* Unlocks a mutex */
void errMutexUnlock(pthread_mutex_t* mutex, char* message);

/* Destroys a mutex */
void errMutexDestroy(pthread_mutex_t* mutex, char* message);

/* Initializes a Condition Variable with NULL attr */
void errCondVarInit(pthread_cond_t* condvar, char* message);

/* Waits on a Condition Variable with associated mutex */
void errCondVarWait(pthread_cond_t* condvar, pthread_mutex_t* mutex, char* message);

/* Signals a Condition Variable */
void errCondVarSignal(pthread_cond_t* condvar, char* message);

/* Destroys a Condition Variable */
void errCondVarDestroy(pthread_cond_t* condvar, char* message);

/* Writes a "string" to a named pipe to be read later */
void errWriteToPipe(char* sends, int pipeDescriptor);

/* Reads a "string" from a named pipe, sent by the function above
void errReadFromPipe(char* reads, int pipeDescriptor, int maxBufSize);
*/

/* DEFAULT ERROR MESSAGES DEFINITIONS: */
#define ERR_DESTROYMUTEX 		"ERROR: Couldn't destroy a Mutex!"
#define ERR_LOCKMUTEX 			"ERROR: Couldn't lock a Mutex!"
#define ERR_UNLOCKMUTEX 		"ERROR: Couldn't unlock a Mutex!"
#define ERR_INITCONDVARMUTEX	"ERROR: Couldn't initialize a Condition Variable associated Mutex!"
#define ERR_LOCKCONDVARMUTEX 	"ERROR: Couldn't lock a Condition Variable associated Mutex!"
#define ERR_UNLOCKCONDVARMUTEX 	"ERROR: Couldn't unlock a Condition Variable associated Mutex!"
#define ERR_DESTROYCONDVARMUTEX	"ERROR: Couldn't destroy a Condition Variable associated Mutex!"
#define ERR_INITCONDVAR 		"ERROR: Couldn't initialize a Condition Variable!"
#define ERR_WAITCONDVAR 		"ERROR: Couldn't wait on a Condition Variable!"
#define ERR_SIGNALCONDVAR 		"ERROR: Couldn't signal a Condition Variable!"
#define ERR_DESTROYCONDVAR 		"ERROR: Couldn't destroy a Condition Variable!"
#define ERR_WRONGARGUMENTS		"ERROR: Wrong number of arguments!"
#define ERR_WRITETOPIPE			"ERROR: Couldn't write to a pipe!"
#define ERR_READFROMPIPE		"ERROR: Couldn't read from a pipe!"


#endif /* __ERROR_HELPER_H__ */
