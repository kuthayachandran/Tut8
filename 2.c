#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#define processP "C:\Users\100566729\Desktop\100566729_Tutorial8/process" 
#define Memory 1024;
int avail_mem[1024]={0};
typedef struct {
	char name[256];
	int priority;
	int pid;
	int address;
	int memory;
	int runtime;
	bool suspended;
} proc;
typedef struct {
	proc process;
	struct fifo_queue * next;
}fifo_queue;
void add(fifo_queue ** head,proc process){
	
	if((*head)->process.runtime==0){
		(*head)->process=process;
	}
	else if((*head)->next!=NULL){
		add(&(*head)->next, process);
	}
	else{
		fifo_queue * next = malloc(sizeof(fifo_queue));
		next->process=process;
		(*head)->next= next;
	}
}
proc pop(fifo_queue ** head){
	
	proc ret_proc;
	proc null_proc;
	strcpy(null_proc.name, "");
	fifo_queue * next_proc = NULL;
	if(*head == NULL){
		printf("Pop could not be performed the list in empty");
		return null_proc;
	}
	next_proc = (*head)->next;
	ret_proc =(*head)->process;
	free(*head);
	*head = next_proc;
	return ret_proc;
}

void print_list(fifo_queue ** head) {
	fifo_queue * current_proc = *head;

	while (current_proc != NULL) {
		printf("%s\n",current_proc->process.name);
		current_proc = current_proc->next;
	}
}

void memAlloc(int memory, int pid, int start){
	int i;
	for(i=0; i<memory; i++){
		avail_mem[i+start] = pid;
	}
}

void memFree(int memory, int pid, int start){
	int i;
	for(i=0; i<memory; i++){
		avail_mem[i+start] = 0;
	}
}

int Cprocess(proc *process){
	int status, start = 0;
	int runtime = process->runtime;
	int pid2;
	pid_t pid;
	pid = fork();
	if(pid == 0){
		process->pid = (int) getpid();
		printf("\n%s, ", process->name);
		printf("%d, ", process->priority);
		printf("%d, ", process->pid);
		printf("%d\n", process->runtime);
		memAlloc(process->memory, process->pid, 0);
		execl(processP, "process", pid, process->name,NULL);
	}
	else if(pid<0){

		status = -1;

	}
	else{
		sleep(runtime);
		kill(pid, SIGINT);
		if (waitpid (pid, &status, 0) != pid){
			status = -1;
			memFree(process->memory, process->pid, 0);
		}
	}
	process->pid = pid2;
	return status;
}

void secondary_Cprocess(fifo_queue ** head){
	while(head==NULL){
		int status, start = 0;
		proc process = pop(head);
		if(start+process.memory < sizeof(avail_mem)/sizeof(int)){
			pid_t pid;
			pid = fork();
			if(pid == 0){
				process.pid = (int) getpid();
				printf("\n%s, ", process.name);
				printf("%d, ", process.priority);
				printf("%d, ", process.pid);
				printf("%d\n", process.runtime);
				process.address = start;
				memAlloc(process.memory, process.pid, start);
				start += process.memory;
				execl(processP, "process", pid, process.name,NULL);
			}
			else if(pid<0){

				status = -1;

			}
			else{
				if(process.pid!=0 && process.suspended)
					kill(pid, SIGCONT);
				sleep(1);
				kill(pid, SIGTSTP);
				process.suspended= true;
				if(process.runtime >= 1){
					if (waitpid (pid, &status, 0) != pid){
						status = -1;
						memFree(process.memory, process.pid, process.address);
					}
				}
				if(process.runtime <=1){
					process.runtime--;
					add(head, process);
				}
			}
		}
		else{
			add(head, process);
		}

	}
}
int main(void) {
	fifo_queue * prioirty = malloc(sizeof(fifo_queue));
	fifo_queue * secondary = malloc(sizeof(fifo_queue));
	proc current_proc;
	char buffer[256];
	char *buffer2;
	char name[256];
	int priority, runtime, memory;
	FILE *file = fopen("processes_q5.txt", "r");
	
	while(fgets(buffer, 256, file)){
		memcpy(name, buffer, 256);
		strtok_r(name, ",", &buffer2);
		strcpy(current_proc.name, name);

		strcpy(buffer, buffer2);
		strtok_r(buffer, ", ", &buffer2);
		priority = atoi(buffer);
		current_proc.priority = priority;

		strcpy(buffer, buffer2);
		strtok_r(buffer, ", ", &buffer2);
		memory = atoi(buffer);
		current_proc.memory = memory;

		strcpy(buffer, buffer2);
		strtok(buffer, "\n");
		runtime = atoi(buffer);
		current_proc.runtime = runtime;

		current_proc.suspended = false;
		current_proc.pid = 0;
		current_proc.address = 0;
		if(current_proc.priority == 0)
			add(&prioirty, current_proc);
		else
			add(&secondary,current_proc);
	}
	
	while(&prioirty->process!=NULL){
		proc poped;
		poped = pop(&prioirty);
		Cprocess(&poped);
	}
	
	secondary_Cprocess(&secondary);

	return 0;
}
