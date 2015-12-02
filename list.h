#ifndef __LIST_H__
#define __LIST_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "time_helper.h"

/* We use this struct to save processes on the list */
typedef struct lst_iitem{
   int pid;
   time_t starttime;
   time_t endtime;
   int status;
   char* cmd;
   struct lst_iitem *next;
} lst_iitem_t;

/* List instance */
typedef struct{
   lst_iitem_t * first;
   int lst_size;
   int lst_active;
   pthread_mutex_t * lst_mutex;
   int final; /* if list is final we wont let you add new elements */
} list_t;



/* Allocates memory for list_t and initializes it, returning a pointer to it */
list_t* lst_new();

/* Deallocates all the memory needed for the list. */
void lst_destroy(list_t *list);

/* Inserts a new process into the list, if there was a problem allocating memory
 * it returns 0. The return value is 1 if there was no problem. */
int insert_new_process(list_t *list, int pid, time_t starttime, char* cmd);

/* Updates the process info with it's endtime and exit status. */
lst_iitem_t* update_terminated_process(list_t *list, int pid, time_t endtime, int status);

/* Returns the number of processes stored on the list, running or not. */
int lst_sizeof(list_t *list);

/* Returns the number of active processes stored on the list. */
int lst_numactive(list_t *list);

/* Locks mutex, consider it the mutex itself */
void lst_lock(list_t *list);

/* Unlocks mutex, consider it the mutex itself */
void lst_unlock(list_t *list);

/* Finalizes list */
void lst_finalize(list_t *list);

/* Returns final status, 1 if final, 0 otherwise */
int lst_isfinal(list_t *list);

/* Prints the list into the stdout stream */
void lst_print(list_t *list);

/* Removes the process with that pid from the list
 * Return value is 0 if the process wasn't found
 * else, the return value is 1	*/
int lst_remove(list_t *list, int pid);


/* Pushes a new element with said PID value to the end of the list */
/* Returns 0 on error, 1 otherwise */
int lst_push(list_t *list, int newPid);

/* Pops the first element from the list, returning its PID value */
/* Returns 0 on error, a pid otherwise */
int lst_pop(list_t *list);

#endif /* __LIST_H__ */
