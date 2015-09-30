/*
 * list.h - definitions and declarations of the integer list 
 */


#include <stdlib.h>
#include <stdio.h>
#include <time.h>




/* lst_iitem - each element of the list points to the next element */
typedef struct lst_iitem {
   int pid;
   time_t starttime;
   time_t endtime;
   struct lst_iitem *next;
} lst_iitem_t;

/* list_t */
typedef struct {
   lst_iitem_t * first;
   int lst_size;
} list_t;



/* lst_new - allocates memory for list_t and initializes it */
list_t* lst_new();

/* lst_destroy - free memory of list_t and all its items */
void lst_destroy(list_t *list);

/* 	insert_new_process - insert a new item with process id and its start time in list 'list' 
	0 se deu bosta, outra coisa se funcionou												*/
int insert_new_process(list_t *list, int pid, time_t starttime);

/* update_terminated_process - inserts the end time in the item with pid recieved */
void update_terminated_process(list_t *list, int pid, time_t endtime);

/* lst_sizeof - return the number of items in the list*/
int lst_sizeof(list_t *list);

/* lst_print - print the content of list 'list' to standard output */
void lst_print(list_t *list);

/* lst_remove - removes the process with that pid  */
int lst_remove(list_t *list, int pid);