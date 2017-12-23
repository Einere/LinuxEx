#include "TestCase2.h"
#define MAX_TEXT 512
struct mymsgbuf{
		long mytype;
		char mtext[MAX_TEXT];

};

struct my_msg_st {
    long int my_msg_type;
    char my_text[MAX_TEXT];
};

int SendInTestCase2(int type)
{
	key_t key;
	int msgid, count;
	struct mymsgbuf mesg;
	key = 2555;
	msgid = mymsgget(key,0);
	if(msgid==-1)
	{
		perror("msgget");
		exit(1);
	}
	mesg.mytype = type;
	for(count=0; count<2; count++){
		strcpy(mesg.mtext,"Message Q Test");
		if(mymsgsnd(msgid,(void*)&mesg,MAX_TEXT,0)==-1){
			printf("msgsnd in : %d",type);
			exit(1);
		}
	}
}


int ReadInTestCase2(int type)
{
	struct mymsgbuf inmsg;
	key_t key;
	int msgid,len,count;
	key=2555;
	if((msgid=mymsgget(key,0))<0)
	{
		perror("msgget");
		exit(1);

	}
	for(count=0;count<2;count++){
		len =mymsgrcv(msgid,&inmsg,MAX_TEXT,type,0);
		if(len==-1)
		{
			perror("msgrcv");
		}
		printf("Received Msg= %s,Len %d, Type : %d\n",inmsg.mtext,len,(int)inmsg.mytype);
	}
}

void RemoveMessageQueue(){
	int msgid,len;
	int count,key;
	key=2555;
	if((msgid=mymsgget(key,0))<0) 
	{
		perror("msgget");
		exit(1);

	}
	mymsgctl(msgid,MY_IPC_RMID,0);
}


void TestCase2(void)
{
	thread_t pid[6];

	thread_create(&pid[3],NULL,(void*)ReadInTestCase2,(void*)1);
	thread_create(&pid[4],NULL,(void*)ReadInTestCase2,(void*)2);
	thread_create(&pid[5],NULL,(void*)ReadInTestCase2,(void*)3);
	
	thread_create(&pid[0],NULL,(void*)SendInTestCase2,(void*)1);
	thread_create(&pid[1],NULL,(void*)SendInTestCase2,(void*)2);
	thread_create(&pid[2],NULL,(void*)SendInTestCase2,(void*)3);

	sleep(10);

	RemoveMessageQueue();
}
