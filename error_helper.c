#include "error_helper.h"

void defaultErrorBehavior(char* message){
	fprintf(stderr, "%s\n", message);
	exit(EXIT_FAILURE);
}

void errMutexInit(pthread_mutex_t* mutex, char* message){
	if(pthread_mutex_init(mutex, NULL))
		defaultErrorBehavior(message);
}

void errMutexLock(pthread_mutex_t* mutex, char* message){
	if(pthread_mutex_lock(mutex))
		defaultErrorBehavior(message);
}

void errMutexUnlock(pthread_mutex_t* mutex, char* message){
	if(pthread_mutex_unlock(mutex))
		defaultErrorBehavior(message);
}

void errMutexDestroy(pthread_mutex_t* mutex, char* message){
	if(pthread_mutex_destroy(mutex))
		defaultErrorBehavior(message);
}

void errCondVarInit(pthread_cond_t* condvar, char* message){
	if(pthread_cond_init(condvar, NULL))
		defaultErrorBehavior(message);
}

void errCondVarWait(pthread_cond_t* condvar, pthread_mutex_t* mutex, char* message){
	if(pthread_cond_wait(condvar, mutex))
		defaultErrorBehavior(message);
}

void errCondVarSignal(pthread_cond_t* condvar, char* message){
	if(pthread_cond_signal(condvar))
		defaultErrorBehavior(message);
}

void errCondVarDestroy(pthread_cond_t* condvar, char* message){
	if(pthread_cond_destroy(condvar))
		defaultErrorBehavior(message);
}

/* Writes a char array to a named pipe to be read later */
void errWriteToPipe(char** sendv, int pipeDescriptor){
	int i;
	for(i = 0; sendv[i] != NULL; i++)
		if(write(pipeDescriptor, sendv[i], sizeof(sendv[i])) == -1)
			defaultErrorBehavior(ERR_WRITETOPIPE);
	if(write(pipeDescriptor, NULL, sizeof(NULL)) == -1)
		defaultErrorBehavior(ERR_WRITETOPIPE);
}

/* Reads a char array from a named pipe, sent by the function above */
void errReadFromPipe(char** readv, int pipeDescriptor, int maxBufSize){
	int i;
	for(i = 0; readv[i] != NULL; i++)
		if(read(pipeDescriptor, readv[i], maxBufSize) == -1)
			defaultErrorBehavior(ERR_READFROMPIPE);
}