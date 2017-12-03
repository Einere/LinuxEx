#include "Init.h"
#include "Scheduler.h"
#include "Thread.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void* child(void* arg){
	while(1){
		printf("child thread is running\n");
		sleep(1);
	}
	return NULL;
}

void* exam(void* arg){
	thread_t tid;
	
	//thread_create(&tid, NULL, child, NULL);
	while(1) {
		printf("exam is running\n");
		sleep(1);
	}
}

int main(void)
{
	thread_t tid;
	int arg;

	RunQHead = NULL;
	thread_create(&tid, NULL, exam, NULL);
	
	printf("----- run scheduler ------\n");
	RunScheduler();

	return 0;
}


