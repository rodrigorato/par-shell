#include "helper.h"

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


void errReadFromPipe(char* reads, int pipeDescriptor, int maxBufSize){
	int readChars = 0;
	if((readChars = read(pipeDescriptor, reads, maxBufSize)) == -1)
		defaultErrorBehavior(ERR_READFROMPIPE);
	reads[readChars] = '\0';
}

int errOpen(char* filename, int mode){
	int i;
	i = open(filename, mode);
	if(i == -1)
		defaultErrorBehavior(ERR_OPENFILE);
	return i;
}

FILE* errFOpen(char* filename, char* flags){
	FILE* fd;
	fd = fopen(filename, flags);
	if(fd == NULL)
		defaultErrorBehavior(ERR_OPENFILE);
	return fd;
}

int errOpenPerms(char* filename, int mode, int perms){
	int i;
	i = open(filename, mode, perms);
	if(i == -1)
		defaultErrorBehavior(ERR_OPENFILE);
	return i;
}

void errClose(int fd){
	if(close(fd))
		defaultErrorBehavior(ERR_CLOSEFILE);
}

void errFClose(FILE* fd){
	if(fclose(fd))
		defaultErrorBehavior(ERR_CLOSEFILE);
}

void errSignal(int sig, void* routine){
	if(signal(sig, routine) == SIG_ERR)
		defaultErrorBehavior(ERR_SIGNAL);
}

void errFflush(FILE* fd){
	if(fflush(fd))
		defaultErrorBehavior(ERR_FFLUSH);
}

void errUnlink(char* path){
	if(unlink(path))
		defaultErrorBehavior(ERR_UNLINK);
}

int errDup(int fd){
	int i;
	if((i = dup(fd)) == -1)
		defaultErrorBehavior(ERR_DUP);
	return i;
}