#ifndef __HELPER_H__
#define __HELPER_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

/* Sets the max array size of arguments we will read */
/* [prog name, arg1, arg2, arg3, arg4, arg5, NULL] */
#define PATHNAME_MAX_ARGS 	5
#define INPUTVECTOR_SIZE 	PATHNAME_MAX_ARGS+2 

/* Limits the number of programs running at a time */
/* Should be set to the number of cores in your machine. */
#define MAXPAR 				4 

/* Sets the maximum buffer sizes for the buffers used in the program */
#define MAX_BUF 			1024
#define MAXLOGLINESIZE 		256
#define MAXFILENAMELENGTH 	28

/* The file names used */
#define INPUTPIPENAME 		"par-shell-in" 
#define STATSFILE 			"par-shell-stats.txt"

/* Commands sent between the terminals and the par-shell program */
#define NEWTERMINALID 		"new_terminal_pid"
#define CLOSINGTERMINAL 	"closing_terminal_pid"
#define NORMAL_COMMAND 		0
#define EXIT_COMMAND 		1
#define EXIT_GLOBAL_COMMAND 2 
#define STATS_COMMAND 		3


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

/* Reads a "string" from a named pipe, sent by the function above */
void errReadFromPipe(char* reads, int pipeDescriptor, int maxBufSize);

/* Tries to open filename returning its filedescriptor */
int errOpen(char* filename, int mode);

/* Tries to open a fopen() */
FILE* errFOpen(char* filename, char* flags);

/* Tries to open filename, with permissions returning its filedescriptor */
int errOpenPerms(char* filename, int mode, int perms);

/* Tries to close the file with the fd it receives */
void errClose(int fd);

/* Tries to fclose() */
void errFClose(FILE* fd);

/* Associates a signal with a routine */
void errSignal(int sig, void* routine);

/* Tries to flush a file */
void errFflush(FILE* fd);

/* Tries to unlink a file */
void errUnlink(char* path);

/* Tries to dup */
int errDup(int fd);

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
#define ERR_OPENFILE			"ERROR: Couldn't open a file!"
#define ERR_CLOSEFILE			"ERROR: Couldn't close a file!"
#define ERR_WRITETOFILE			"ERROR: Couldn't write to a file!"
#define ERR_FFLUSH				"ERROR: Couldn't flush a file!"
#define ERR_SIGNAL 				"ERROR: Couldn't associate a routine to a signal!"
#define ERR_UNLINK				"ERROR: Couldn't unlink a file!"
#define ERR_THREADJOIN			"ERROR: Couldn't join a thread!"
#define ERR_ALLOCLIST			"ERROR: Couldn't create an instance of a list!"
#define ERR_WATCHERTHREADCREATE "ERROR: Couldn't start a watcher thread!"
#define ERR_MKFIFO				"ERROR: There was a problem making the par-shell pipe!"
#define ERR_DUP					"ERROR: Couldn't dup!"
#define ERR_LISTINSERT			"ERROR: Couldn't insert a new element to the list!"
#define ERR_LISTREMOVE			"ERROR: Couldn't remove an element from the list!"
#define ERR_FORK				"ERROR: Couldn't fork a new process!"
#define ERR_EXECV				"ERROR: Couldn't execv a program!"

#endif /* __ERROR_HELPER_H__ */
