#include "Init.h"
#include "Scheduler.h"
#include "Thread.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void* exam(void* arg){
	while(1) {
		printf("exam is running\n");
		sleep(1);
	}
}

int main(void){
	thread_t tid;
	int arg;
	
	pthread_mutex_init(&static_mutex, NULL);
	pthread_cond_init(&static_cond, NULL);

	RunQHead = NULL;
	thread_create(&tid, NULL, exam, NULL);
	
	printf("----- run scheduler ------\n");
	RunScheduler();

	return 0;
}


