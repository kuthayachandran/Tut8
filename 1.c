#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct {
	char parent[256];
	char name[256];
	int priority;
	int memory;
} proc;
typedef struct {
	proc process;
	struct proc_tree * nextl;
	struct proc_tree * nextr;
}proc_tree;

void add(proc_tree ** head,proc process){
	
	if(strcmp(process.parent,"NULL")==0)
		(*head)->process=process;
	
	if((*head)->nextl!=NULL&&(*head)->nextr!=NULL){
		add(&(*head)->nextl,process);
		add(&(*head)->nextr,process);
	}
	
	if((*head)->nextl==NULL){
		if(strcmp((*head)->process.name,process.parent)==0){
			proc_tree * left = malloc(sizeof(proc_tree));
			left->process = process;
			(*head)->nextl = left;
		}
	}
	else if((*head)->nextr==NULL){
		if(strcmp((*head)->process.name,process.parent)==0){
			proc_tree * right = malloc(sizeof(proc_tree));
			right->process = process;
			(*head)->nextr = right;
		}
	}
}

void print_list(proc_tree ** head) {
	proc_tree * current_proc = *head;
	printf("%s\n",current_proc->process.name);
	if((*head)->nextl!=NULL){
		print_list(&(*head)->nextl);
	}
	if((*head)->nextr!=NULL){
		print_list(&(*head)->nextr);
	}
}
int main(void) {
	proc_tree * tree = malloc(sizeof(proc_tree));
	proc current_proc;
	char buffer[256];
	char *buffer2;
	char name[256];
	int priority, memory;
	
	FILE *file = fopen("process.txt", "r");
	while(fgets(buffer, 256, file)){
		memcpy(name, buffer, 256);
		strtok_r(name, ", ", &buffer2);
		strcpy(current_proc.parent, name);

		strcpy(name, buffer2);
		strtok_r(name, ", ", &buffer2);
		memmove(&name[0], &name[1], strlen(name) - 0);
		strcpy(current_proc.name, name);

		strcpy(buffer, buffer2);
		strtok_r(buffer, " ", &buffer2);
		strcpy(buffer, buffer2);
		strtok_r(buffer, ", ", &buffer2);
		priority = atoi(buffer);
		current_proc.priority = priority;

		strcpy(buffer, buffer2);
		strtok(buffer, "\n");
		memory = atoi(buffer);
		current_proc.memory = memory;
		add(&tree, current_proc);
	}
	print_list(&tree);
}
