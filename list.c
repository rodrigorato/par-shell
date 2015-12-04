#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "helper.h"
#include "list.h"
#include "time_helper.h"


list_t* lst_new(){
	list_t *list;
	list = (list_t*) malloc(sizeof(list_t));
   	if(list){
   		list->first = NULL;
   		list->lst_size = 0;
   		list->lst_active = 0;
   		list->final = 0;
   		list->lst_mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
   		if((!(list->lst_mutex)) || pthread_mutex_init(list->lst_mutex,NULL)){ // if mutex allocation or initialization failed
				free(list->lst_mutex); free(list); list = NULL; // we undo all work previously done
			}
   	}
   	return list;
}


void lst_destroy(list_t *list){
	struct lst_iitem *item, *nextitem;
	item = list->first;
	while (item != NULL){
		nextitem = item->next;
		free(item->cmd);
		free(item);
		item = nextitem;
  	}

  	/* As soon as the mutex is unlocked the list is destroyed */
  	if(list->lst_mutex){
  		while(pthread_mutex_trylock(list->lst_mutex));
  		errMutexUnlock(list->lst_mutex, ERR_UNLOCKMUTEX);
  		errMutexDestroy(list->lst_mutex, ERR_DESTROYMUTEX);
  		free(list->lst_mutex);
		free(list);
	}
}


int insert_new_process(list_t *list, int pid, time_t starttime, char* cmd){
	if(lst_isfinal(list))
		return 0;
	lst_iitem_t *item;
	item = (lst_iitem_t *) malloc (sizeof(lst_iitem_t));
	if (!item)
		return 0;
	item->pid = pid;
	item->starttime = starttime;
	item->endtime = 0;
	item->status = 0;

	if(cmd){
		item->cmd = (char*) malloc(sizeof(char)*(strlen(cmd)+1));
		if(item->cmd)
			strcpy(item->cmd, cmd);
		else{
			free(item); return 0;
		}
	}
	else{
		/* We have to allocate it like so if we want to free it later. */
		item->cmd = (char*) malloc(sizeof(char)*7);
		if(item->cmd)
			strcpy(item->cmd, "(null)");
		else{
			free(item); return 0;
		}
	}

	item->next = list->first;
	(list->lst_size)++;
	(list->lst_active)++;
	list->first = item;
	return 1;
}


lst_iitem_t* update_terminated_process(list_t *list, int pid, time_t endtime,int status){
	lst_iitem_t *temp = list->first;
	for(; temp!=NULL; temp=temp->next){
		if((temp->pid) == pid){
			temp->endtime = endtime;
			temp->status = status;
			(list->lst_active)--;
			return temp;
		}
	}
	if(!temp)
		printf("no process with pid: %d\n", pid);
	return NULL;
}

int lst_sizeof(list_t *list){
	return list->lst_size;
}

int lst_numactive(list_t *list){
	return list->lst_active;
}

void lst_lock(list_t *list){
	errMutexLock(list->lst_mutex, ERR_LOCKMUTEX);
}

void lst_unlock(list_t *list){
	errMutexUnlock(list->lst_mutex, ERR_UNLOCKMUTEX);
}

void lst_finalize(list_t *list){
	list->final = 1;
}

int lst_isfinal(list_t *list){
	return list->final;
}

void lst_print(list_t *list){
	lst_iitem_t *item;

	printf("\nPROCESS LIST:\n"
		   "%-6s\t%-4s\t%-10s\t%-10s\t%-9s\t%-s\n",
		   "PID", "STATUS", "START TIME", "END TIME", "DURATION", "COMMAND");

	item = list->first;
	while (item != NULL){
		printf("%-6d\t", item->pid);

		if(WIFEXITED(item->status))
			printf("%-4d\t", WEXITSTATUS(item->status));
		else
			printf("%s\t", "NoExit");

		PRINT_TIME_T_AS_HMS(item->starttime); printf("\t");
		PRINT_TIME_T_AS_HMS(item->endtime);	printf("\t");

		printf("%-9g\t", GET_DURATION_TIME(item->endtime,item->starttime));
		printf("%-s\n", item->cmd);

		item = item->next;
	}

}

int lst_remove(list_t *list, int pid){
	lst_iitem_t *temp = list->first, *aux=NULL;
	for(; temp && temp->pid != pid; aux=temp, temp=temp->next);
	if(temp){
		if(!aux)
			list->first = (list->first)->next;
		else
			aux->next=temp->next;
		free(temp);
		(list->lst_size)--;
		return 1;
	}
	return 0;
}


int lst_push(list_t *list, int newPid){
	lst_iitem_t *item;
	item = (lst_iitem_t *) malloc (sizeof(lst_iitem_t));
	if (!item)
		return 0;
	item->pid = newPid;
	item->starttime = (time_t)NULL;
	item->endtime = (time_t)NULL;
	item->status = 0;
	item->cmd = (char*)NULL;
	item->next = list->first;
	(list->lst_size)++;
	list->first = item;
	return 1;
}

int lst_pop(list_t *list){
	int pid;
	lst_iitem_t *item;
	if(!list) 
		return 0;
	
	item = list->first;
	if(!item) 
		return 0;

	pid = item->pid;
	list->first = item->next;
	free(item);
	return pid;
}

