#include "Init.h"
#include "Thread.h"
#include "Scheduler.h"


int RunScheduler( void )
{
	
}


void __ContextSwitch(Thread* pCurThread, Thread* pNewThread){
	//send running thread to ready queue, pop head thread and run
	//schduler call rq_pop, pass Thread pointer to __ContextSwitch()
	
	pthread_kill(pCurThread->tid, SIGUSR1);
	//stop running thread
	pCurThread->status = THREAD_STATUS_READY;
	//set stoped thread's status to ready
	rq_push(pCurThread);
	//push stoped thread at ready queue
	__thread_wakeup(pNewThread);
	//run new thread by waking up
	
	return NULL;
}

