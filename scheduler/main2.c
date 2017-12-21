#include "MsgQueue.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
	_InitMsgQueue();
	int index1 = mymsgget(100,0);
	int index2 = mymsgget(200,0);
	
	Message* sndmsg = (Message*)malloc(sizeof(Message));
	sndmsg->type = 1;
	strcpy(sndmsg->data, "hello, world!");
	int nsnd = mymsgsnd(index1, (void*)sndmsg, strlen(sndmsg->data), 0);   
	printf("saved length = %d\n", nsnd);

	Message* rcvmsg = (Message*)malloc(sizeof(Message));
	int nrcv = mymsgrcv(index1, rcvmsg, nsnd, 1, 0);
	
	
	return 0;
}
