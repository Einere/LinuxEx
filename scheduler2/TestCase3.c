#include "TestCase3.h"
#define MAX_TEXT 512
struct mymsgbuf{
		long mytype;
		char mtext[MAX_TEXT];

};


int SendInTestCase3(int inputKey)
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

	for(count=0;count<2;count++)
	{
		mesg.mytype = 1;
		strcpy(mesg.mtext,"Message Q Test");
		if(mymsgsnd(msgid,(void*)&mesg,MAX_TEXT,0)==-1){
			exit(1);
		}
	}
}


int ReadInTestCase3(int inputKey)
{
	struct mymsgbuf inmsg;
	key_t key;
	int msgid,len,count;
	
	key=inputKey;

	if((msgid=mymsgget(key,0))<0) 
	{
		perror("msgget");
		exit(1);

	}
	for(count =0; count<2;count++){
		len =mymsgrcv(msgid,&inmsg,MAX_TEXT,1,0);
		if(len==-1)
		{
			perror("msgrcv");
		}
		printf("Received Msg= %s,Len %d, Type : %d\n",inmsg.mtext,len,(int)inmsg.mytype);
	}
	mymsgctl(msgid,MY_IPC_RMID,0);
}




void TestCase3(void)
{
	thread_t pid[7];
	thread_create(&pid[3],NULL,(void*)ReadInTestCase3,(void*)1234);
	thread_create(&pid[4],NULL,(void*)ReadInTestCase3,(void*)1235);
	thread_create(&pid[5],NULL,(void*)ReadInTestCase3,(void*)1236);
	
	thread_create(&pid[0],NULL,(void*)SendInTestCase3,(void*)1234);
	thread_create(&pid[1],NULL,(void*)SendInTestCase3,(void*)1235);
	thread_create(&pid[2],NULL,(void*)SendInTestCase3,(void*)1236);




}