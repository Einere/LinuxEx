#include "MsgQueue.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
	_InitMsgQueue();
	int index1 = mymsgget(100,0);
	int index2 = mymsgget(200,0);
	
	Message* mymsg = (Message*)malloc(sizeof(Message));
	mymsg->type = 1;
	strcpy(mymsg->data, "hello, world!");
	int nsave = mymsgsnd(index1, (void*)mymsg, strlen(mymsg->data), 0);   
	printf("saved length = %d\n", nsave);
	return 0;
}
