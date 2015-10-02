#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "list.h"
#include "time_helper.h"
		

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
	struct lst_iitem *item, *nextitem;
	item = list->first;
	while (item != NULL){
		nextitem = item->next;
		free(item->cmd);
		free(item);
		item = nextitem;
  	}
	free(list);
}


int insert_new_process(list_t *list, int pid, time_t starttime, char* cmd)
{
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
		strcpy(item->cmd, cmd);
	}
	else{
		/* We have to allocate this like so if we want to free it later. */
		item->cmd = (char*) malloc(sizeof(char)*7);
		strcpy(item->cmd, "(null)");
	}

	item->next = list->first;
	(list->lst_size)++;
	list->first = item;
	return 1;
}


void update_terminated_process(list_t *list, int pid, time_t endtime,int status)
{
	lst_iitem_t *temp = list->first;
	for(; temp!=NULL; temp=temp->next){
		if((temp->pid) == pid){
			temp->endtime = endtime;
			temp->status = status;
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

	printf("\nPROCESS LIST:\n"
		   "%-6s\t%-4s\t%-10s\t%-10s\t%-s\n", 
		   "PID", "STATUS", "START TIME", "END TIME", "COMMAND");
	
	item = list->first;
	while (item != NULL){
		printf("%-6d\t", item->pid);
		printf("%-4d\t", item->status);
		PRINT_TIME_T_AS_HMS(item->starttime); printf("\t");
		PRINT_TIME_T_AS_HMS(item->endtime);	printf("\t");
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
		free(temp->cmd);
		free(temp);
		list->lst_size--;
		return 1;
	}
	return 0;
}
