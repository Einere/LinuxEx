#include "TestCase2.h"

void* Tc2ThreadProc(void* param)
{
		thread_t tid = 0;
		int count = 0;
		int i;
		int* retVal;
		tid = thread_self();
	    for(int i=0;i<10;i++){
	    	         sleep(2);
                     printf("Tc1ThreadProc: my thread id (%d), arg is (%d)\n", (int)tid, *((int*)param));
                     count++;
          
	    }             /* sleep for 1 seconds */
        retVal = (int*)param;
        thread_exit(retVal);
        return NULL;
}


/* 
 * - Testcase2 tests thread_join works successfully.
 * - 
 */
void
TestCase2(void)
{
	thread_t tid[TOTAL_THREAD_NUM];
	int result[TOTAL_THREAD_NUM];

	int i = 0, i1 = 1, i2 = 2, i3 = 3, i4 = 4, i5 = 5;

	thread_create(&tid[0], NULL, (void*)Tc2ThreadProc,(void*) &i1);	
	thread_create(&tid[1], NULL, (void*)Tc2ThreadProc,(void*) &i2);	
	thread_create(&tid[2], NULL, (void*)Tc2ThreadProc,(void*) &i3);	
	thread_create(&tid[3], NULL, (void*)Tc2ThreadProc,(void*) &i4);	
	thread_create(&tid[4], NULL, (void*)Tc2ThreadProc,(void*) &i5);
	
	for(i=0;i<TOTAL_THREAD_NUM;i++)
	{
		int* retVal;
		thread_join(tid[i],(void **)&retVal);

		printf("Thread [ %d ] is finish Return : [ %d ] ",(int)tid[i], *retVal);
	}

	return ;
}
