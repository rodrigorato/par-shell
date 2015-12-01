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

void errWriteToPipe(char* sends, int pipeDescriptor){
	if(write(pipeDescriptor, sends, strlen(sends)) == -1)
		defaultErrorBehavior(ERR_WRITETOPIPE);
}

/*
void errReadFromPipe(char* reads, int pipeDescriptor, int maxBufSize){
	printf("ERRHELPER: Trying to read\n");
	if(read(pipeDescriptor, reads, maxBufSize) == -1)
		defaultErrorBehavior(ERR_READFROMPIPE);
	printf("ERRHELPER: Read %s\n", reads);
}
*/

