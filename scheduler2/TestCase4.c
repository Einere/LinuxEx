#include "TestCase3.h"
#define MAX_TEXT 512
struct mymsgbuf{
		long mytype;
		char mtext[MAX_TEXT];

};


int SendInTestCase4(int inputKey)
{
	key_t key;
	int msgid;
	struct mymsgbuf mesg;
	int count;
	key = inputKey;
	msgid = mymsgget(key,0);
	if(msgid==-1)
	{
		perror("msgget");
		exit(1);
	}

	mesg.mytype = 1;
	strcpy(mesg.mtext,"Message Q Test");
	if(mymsgsnd(msgid,(void*)&mesg,MAX_TEXT,0)==-1)
	{
		exit(1);
	}
}


int ReadInTestCase4(int inputKey)
{
	struct mymsgbuf inmsg;
	key_t key;
	int msgid,len;
	
	key=inputKey;

	if((msgid=mymsgget(key,0))<0) 
	{
		perror("msgget");
		exit(1);

	}
	len =mymsgrcv(msgid,&inmsg,MAX_TEXT,1,0);
	if(len==-1)
	{
		perror("msgrcv");
	}
	printf("Received Msg= %s,Len %d, Key : %d\n",inmsg.mtext,len,key);

}

void Sender(int* key){
	int count;
	int key1 = key[0];
	int key2 = key[1];
	for(count=0;count<2;count++){
		SendInTestCase4(key1);
		ReadInTestCase4(key2);
	}

}
void Receiver(int* key){
	int count;
	int key1 = key[0];
	int key2 = key[1];
	for(count=0;count<2;count++){
		ReadInTestCase4(key1);
		SendInTestCase4(key2);
	}
}


void RemoveMessageQueueTest4(int inputKey){
	int msgid,len;
	int count,key;
	key=inputKey;
	if((msgid=mymsgget(key,0))<0) 
	{
		perror("msgget");
		exit(1);

	}	
	while(1){
		if(mymsgctl(msgid,MY_IPC_RMID,0)!=-1){
			break;
		}
		else{
			continue;
		}
	}
}


void TestCase4(void)
{
	int key[2]={1234,1235};
	thread_t pid[2];

	thread_create(&pid[1],NULL,(void*)Receiver,(void*)key);
	thread_create(&pid[0],NULL,(void*)Sender,(void*)key);

	sleep(10);
	RemoveMessageQueueTest4(1234);
	RemoveMessageQueueTest4(1235);


}
