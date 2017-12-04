#include "Init.h"
#include "Thread.h"
#include "Scheduler.h"
#include <unistd.h>
#include <signal.h>

int RunScheduler( void ){
	//run scheduler
	Thread* pCurThread = NULL;
	Thread* pNewThread = NULL;
	
	while(1){
		if(ReadyQHead != NULL){
			//if ready queue is not empty	
			fprintf(stderr, "ready queue is not empyt (RunScheduler())\n");
			
			//pCurThread = __getThread(  );
			pCurThread = RunQHead;
			//get running thread's TCB pointer
			
			if(pCurThread == NULL) fprintf(stderr, "running thread's TCB isn't exist (RunScheduler())\n");
			
			pNewThread = rq_pop();
			//pop 1st TCB in ready queue, get it's pointer

			if(pNewThread == NULL) {
				fprintf(stderr, "new thread's TCB isn't exist (RunScheduler())\n");
				continue;
				//if ready queue is empty, don't call context switch
			}

			__ContextSwitch(pCurThread, pNewThread);
			//context switch
		
			fprintf(stderr, "----- sleep 2 -----\n");
			sleep(TIMESLICE);
			fprintf(stderr, "----- wake up -----\n");
		}
		else{
			fprintf(stderr, "ReadyQHead is NULL (RunScheduler())\n");
		}
	}
}


void __ContextSwitch(Thread* pCurThread, Thread* pNewThread){
	//send running thread to ready queue, pop head thread and run
	//schduler call rq_pop, pass Thread pointer to __ContextSwitch()
	fprintf(stderr, "----- context swich start -----\n");

	if(pCurThread != NULL){
		//if current thread is exist
		fprintf(stderr, "current thread is exist (__ContextSwitch())\n");
		pthread_kill(pCurThread->tid, SIGUSR1);
		//stop running thread
		pCurThread->status = THREAD_STATUS_READY;
		//set stoped thread's status to ready
		rq_push(pCurThread);
		//push stoped thread at ready queue
	}

	if(pNewThread != NULL){
		//if new thread is exist
		fprintf(stderr, "next thread is exist (__ContextSwitch())\n");
		__thread_wakeup(pNewThread);
		//run new thread by waking up
		//wakeup function change thread's bRunnable and status
		RunQHead = pNewThread;
		//for track running thread's TCB
	}

	fprintf(stderr, "----- context switch end -----\n");
}

