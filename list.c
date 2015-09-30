/*
 * list.c - implementation of the integer list functions 
 */


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "list.h"



list_t* lst_new(){
	list_t *list;
	list = (list_t*) malloc(sizeof(list_t));
   	if(list){
   		list->first = NULL;
   		list->lst_size = 0;
   	}
   	return list;
}


void lst_destroy(list_t *list){
/* lst_size will get destroyed , since it works w/o */
	struct lst_iitem *item, *nextitem;
	item = list->first;
	while (item != NULL){
		nextitem = item->next;
		free(item);
		item = nextitem;
  	}
	free(list);
}


int insert_new_process(list_t *list, int pid, time_t starttime)
{
	lst_iitem_t *item;
	item = (lst_iitem_t *) malloc (sizeof(lst_iitem_t));
	if (!item)
		return 0;
	item->pid = pid;
	item->starttime = starttime;
	item->endtime = 0;
	item->next = list->first;
	(list->lst_size)++;
	list->first = item;
	/* printf("started process with pid: %d\n", pid); */
	return 1;
}


void update_terminated_process(list_t *list, int pid, time_t endtime)
{
	lst_iitem_t *temp = list->first;
	for(; temp!=NULL; temp=temp->next){
		if((temp->pid) == pid){
			temp->endtime = endtime;
			printf("teminated process with pid: %d\n", pid);	
			break;
		}
	}
	if(!temp)
		printf("no process with pid: %d\n", pid);
}

int lst_sizeof(list_t *list){
	return list->lst_size;
}


void lst_print(list_t *list)
{
	lst_iitem_t *item;

	printf("Process list with start and end time:\n");
	item = list->first;
	while (item != NULL){
		printf("%d\t%s", item->pid, ctime(&(item->starttime)));
		printf("\t%s", ctime(&(item->endtime)));
		item = item->next;
	}
	printf("-- end of list.\n");
}

int lst_remove(list_t *list, int pid){
	lst_iitem_t *temp = list->first, *aux=NULL;
	for(; temp && temp->pid != pid; aux=temp, temp=temp->next);
	if(temp){
		if(!aux)
			// temos que tirar o primeiro
			list->first = (list->first)->next;
		else
			aux->next=temp->next;
		free(temp);
		list->lst_size--;
		return 1;
	}
	return 0;
}
