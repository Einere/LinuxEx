#include "TestCase3.h"
 
void* Tc3ThreadProc(void* param)
{
	thread_t tid = 0;
    int *retVal; 
	tid = thread_self();
	while (1)
	{
		sleep(2);
		printf("Tc3ThreadProc: my thread id:(%d), arg : %d\n", (int)tid, *((int*)param));
	}
	retVal=(int*)param;
	thread_exit(retVal);
	return retVal;
}

/* 
 * - TestCase3 tests suspending, resuming and deleting Threads. 
 * - Testing API scopes: thread_suspend, thread_resume, thread_cancel,thread_self
 */
 
void
TestCase3(void)
{
	thread_t tid[TOTAL_THREAD_NUM];
	int i = 0;
	int j = 0;
	signal(SIGCHLD,SIG_IGN);
	
	thread_create(&tid[0], NULL, (void*)Tc3ThreadProc, (void*)1);	
	thread_create(&tid[1], NULL, (void*)Tc3ThreadProc, (void*)2);	
	thread_create(&tid[2], NULL, (void*)Tc3ThreadProc, (void*)3);	
    thread_create(&tid[3], NULL, (void*)Tc3ThreadProc, (void*)4);	
    thread_create(&tid[4], NULL, (void*)Tc3ThreadProc, (void*)5);	

	/* Suspend all thread */
	for (i = 0; i < TOTAL_THREAD_NUM;i++)
	{
		sleep(2);
		
		if (thread_suspend(tid[i]) == -1)
		{
			printf("TestCase3: Thread suspending Failed..\n");
			assert(0);
		}
		
		Thread *temp = WaitQHead;
		
		for(j=0;j<i;j++)
			temp=temp->pNext;
		
		if(temp->status != 2)
		{
			printf("TestCase3: Thread is not suspended");
			assert(0);
		}
	}

	/* Resume thread sequentially */
	for (i=0;i<TOTAL_THREAD_NUM;i++)
	{
		sleep(2);
		
		if(thread_resume(tid[i])==-1)
		{
			printf("Testcase3: Thread resume Failed");
			assert(0);
		}
	}

	return ;
}
