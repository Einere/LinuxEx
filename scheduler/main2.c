#include "MsgQueue.h"
#include "Init.h"
#include "Helper.h"
#include "Scheduler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void* test(void * para){
	int index1 = mymsgget(100,0);
	int index2 = mymsgget(200,0);
	
	Message* sndmsg = (Message*)malloc(sizeof(Message));
	sndmsg->type = 1;
	strcpy(sndmsg->data, "hello, world!");
	int nsnd = mymsgsnd(index1, (void*)sndmsg, strlen(sndmsg->data), 0);   
	printf("saved length = %d...\n", nsnd);

	Message* rcvmsg = (Message*)malloc(sizeof(Message));
	int nrcv = mymsgrcv(index1, rcvmsg, nsnd, 1, 0);
	printf("nrcv = %d, message = %s...\n", nrcv, rcvmsg->data);	
	
	sndmsg->type = 2;
	strcpy(sndmsg->data, "Bon nuit!");
	nsnd = mymsgsnd(index2, (void*)sndmsg, strlen(sndmsg->data), 0);   
	printf("saved length = %d...\n", nsnd);

	memset(rcvmsg->data, '\0', sizeof(rcvmsg->data));
	nrcv = mymsgrcv(index2, rcvmsg, nsnd, 1, 0);
	printf("nrcv = %d, message = %s...\n", nrcv, rcvmsg->data);	
	
	print_tq(qcbTblEntry[1].pQcb);

	while(1);
	return 0;
}

void* test2(void* para){
	while(1){
		fprintf(stderr, "no mean thread\n");
		sleep(1);
	}
}

int main(){
	pthread_t tid1, tid2;
	Init();
	thread_create(&tid1, NULL, (void*)test, 0);
	thread_create(&tid2, NULL, (void*)test2, 0);
	RunScheduler();
}
