#include "Init.h"
#include "Scheduler.h"
#include "Thread.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void* exam(void* arg){
	while(1) {
		fprintf(stderr, "\n!!!!!!!!!!!!!!!!!!!!!1exam is running\n");
		sleep(1);
	}
}

void* child(void* arg){
	while(1){
		fprintf(stderr, "\n@@@@@@@@@@@@@@@@@@@@@@@@@@child is running\n");
		sleep(1);
	}
}
int main(void){
	thread_t tid1, tid2;
	int arg;
	
	pthread_mutex_init(&static_mutex, NULL);
	pthread_cond_init(&static_cond, NULL);
	
	RunQHead = NULL;
	thread_create(&tid1, NULL, exam, 0);
	fprintf(stderr, "exam's address = %p\n", exam);	
	thread_create(&tid2, NULL, child, 0);
	printf("----- run scheduler ------\n");
	RunScheduler();

	return 0;
}


