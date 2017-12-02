#include "Thread.h"
#include "Init.h"
#include "Scheduler.h"

void* __wrapperFunc(void* arg){
	//!!!!!!!!!! need to confirm !!!!!!!!!!
	
	void* ret;
	WrapperArg* pArg = (WrapperArg*)arg;
	//arg is wrapperArg, need to casting WrapperArg type  
	
	sigset_t set;
	int retSig;
	//for handler
	
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
	sigwait(&set, &retSig);
	//wait until TCB is pushed in ready queue
	//if TCB is pushed in ready queue, parent thread send SIGUSR1 to child thread to execute handler
	
	__thread_wait_handler(0);
	//block until thread is ready to run
	
	void* funcPtr = pArg.funcPtr;
	void* funcArg = pArg.funcArg;

	ret = (*funcPtr)(funcArg);
	
	return ret;
}



int thread_create(thread_t *thread, thread_attr_t *attr, void *(*start_routine) (void *), void *arg){
	//!!!!!!!!!! need to confirm !!!!!!!!!!
	
	ptread_t tmp_tid;
	//for child thread's tid
	WrapperArg wrapperArg;
	//for wrapper
	wrapperArg.funcPtr = child;
	wrapperArg.funcArg = 10;
	//set wrapper function, argument
	
	ptread_create(&tmp_tid, NULL, __wrapperFunc, &wrapperArg);
	//create thread and make to execute __wrapperFunc
	
	Thread tmp;
	//make TCB
	tmp.tid = tmp_tid;
	tmp.status = THREAD_STATUS_READY;
	tmp.parentTid = thread_self(); //pthread_self() or thread_self()
	pPrev = pNext = NULL;
	//set information
	
	push(&tmp);
	//push tmp thread in ready queue
	
	pthread_kill(tmp_tid, SIGUSR1);
	//wake up child thread by sending SIGUSR1
	return 0;
}

void* child(void* arg){
	while(1){
		printf("child thread is running\n");
		sleep(1);
	}
	return NULL:
}

int 	thread_join(thread_t thread, void **retval)
{

}


int 	thread_suspend(thread_t tid)
{

}


int	thread_resume(thread_t tid)
{

}




thread_t	thread_self()
{
	return pthread_self();	
}	

void __thread_wait_handler(int signo){
	//call when send thread at ready queue
	Thread* tmp;

	tmp = __getThread(pthread_self());
	//get caller's TCB pointer
	pthread_mutex_lock(&(tmp->readyMutex));
	//lock mutex
	while(tmp->bRunnable == false) pthread_cond_wait(&(tmp->readyCond), &(tmp->readyMutex));
	//wait until bRunnable be true
	pthread_mutex_unlock(&(tmp->readyMutex));
	//unlock mutex
}

Thread* __getThread(thread_t tid){
	//serach TCB with tid at ready queue and wait queue
	Thread* tmp;

	if((tmp = rq_search(tid)) != NULL) return tmp;
	//if TCB is in ready queue
	if((tmp = wq_search(tid)) != NULL) return tmp;
	//if TCB is in wait queue

	return NULL;
	//not exist TCB in ready queue and wait queue
}

void __thread_wakeup(Thread* pTCB){
	//wake up thread with 1st TCB only when it need to run
	
	pthread_mutex_lock(&(pTCB->readyMutex));
	//lock mutex
	pTCB->bRunnable = true;
	pthread_cond_signal(&(pTCB->readyCond));
	//change bRunnable to true, so thread corresponding TCB will be wake up
	pthread_mutex_unlock(&(pTCB->readyMutex));
	//unlock mutex
}

void __ContextSwitch(Thread* pCurTCB,Thread* pNewTCB){
	//send running thread to ready queue, pop head thread and run
	//schduler call rq_pop, pass Thread pointer to __ContextSwitch()
	
	pthread_kill(pCurTCB->tid, SIGUSR1);
	//stop running thread
	pCurTCB->status = THREAD_STATUS_READY;
	//set stoped thread's status to ready
	rq_push(pCurTCB);
	//push stoped thread at ready queue
	__thread_wakeup(pNewTCB);
	//run new thread by waking up
	
	return NULL;
}


void rq_push(Thread *in_TCB){
	//insert in_TCB at ready queue
	if(ReadyQHead == NULL){
		//if ready queue is empty
		ReadyQHead = in_TCB;
		ReadyQTail = in_TCB;
	}
	else{
		//if ready queue is not empty
		in_TCB->pPrev = ReadyQTail;
		//in_TCB link to previous TCB
		ReadyQTail->pNext = in_TCB;
		//previous TCB link to in_TCB
		ReadyQTail = ReadyQTail->pNext;
		//advance ReadyQTail
	}
}

Thread* rq_search(pthread_t s_tid){
	//search TCB by tid in ready queue
	Thread* tmp;
	tmp = ReadyQHead;
	
	while(tmp != NULL){ //or tmp != readyQTail
		//search until last TCB
		if(tmp->tid == s_tid){
			return tmp;
		}
		tmp = tmp->pNext;
		//advance tmp
	}
	
	return NULL;
	//if not founded
}

Thread* rq_remove(pthread_t r_tid){
	//remove corresponding TCB from ready queue
	Thread* tmp;
	
	if((tmp = rq_search(r_tid)) == NULL){
		perror("not exist searching TCB : ");
		return NULL;
	}
	printf("exist searching TCB (tid = %lu)\n",r_tid);
	
	if(tmp->pPrev != NULL) tmp->pPrev->pNext = tmp->pNext;
	//previous TCB link to next TCB
	if(tmp->pNext != NULL) tmp->pNext->pPrev = tmp->pPrev;
	//next TCB link to previous TCB
	printf("link prev and next complete\n");

	tmp->pPrev = NULL;
	tmp->pNext = NULL;
	//init tmp
	
	return tmp;
}

Thread* rq_pop(){
	//pop head TCB from ready queue
	Thread* tmp;
	tmp = ReadyQHead;

	if(tmp == NULL){
		perror("not exist TCB in ready queue : ");
		return NULL;
	}

	tmp->pNext->pPrev = NULL;
	//next TCB unlink to tmp
	ReadyQHead = tmp->pNext;
	//advance readyQHead
	tmp->pNext = NULL;
	//tmp unlink to next TCB
	
	return tmp;
}

void print_rq(){
	Thread* iter = ReadyQHead;
	while(iter != NULL){
		printf("tid = %lu\n", iter->tid);
		iter = iter->pNext;

	}
	printf("print ready queue end\n");
}

void wq_push(Thread *in_TCB){
	//insert in_TCB at wait queue
	if(WaitQHead == NULL){
		//if wait queue is empty
		WaitQHead = in_TCB;
		WaitQTail = in_TCB;
	}
	else{
		//if wait queue is not empty
		in_TCB->pPrev = WaitQTail;
		//in_TCB link to previous TCB
		WaitQTail->pNext = in_TCB;
		//previous TCB link to in_TCB
		WaitQTail = WaitQTail->pNext;
		//advance WaitQTail
	}
}

Thread* wq_search(pthread_t s_tid){
	//search TCB by tid in wait queue
	Thread* tmp;
	tmp = WaitQHead;
	
	while(tmp != NULL){ //or tmp != WaitQTail
		//search until last TCB
		if(tmp->tid == s_tid){
			return tmp;
		}
		tmp = tmp->pNext;
		//advance tmp
	}
	
	return NULL;
	//if not founded
}

Thread* wq_remove(pthread_t r_tid){
	//remove corresponding TCB from wait queue
	Thread* tmp;
	
	if((tmp = wq_search(r_tid)) == NULL){
		perror("not exist searching TCB : ");
		return NULL;
	}
	printf("exist searching TCB (tid = %lu)\n",r_tid);
	
	if(tmp->pPrev != NULL) tmp->pPrev->pNext = tmp->pNext;
	//previous TCB link to next TCB
	if(tmp->pNext != NULL) tmp->pNext->pPrev = tmp->pPrev;
	//next TCB link to previous TCB
	printf("link prev and next complete\n");

	tmp->pPrev = NULL;
	tmp->pNext = NULL;
	//init tmp
	
	return tmp;
}

Thread* wq_pop(){
	//pop head TCB from wait queue
	Thread* tmp;
	tmp = WaitQHead;

	if(tmp == NULL){
		perror("not exist TCB in wait queue : ");
		return NULL;
	}

	tmp->pNext->pPrev = NULL;
	//next TCB unlink to tmp
	WaitQHead = tmp->pNext;
	//advance WaitQHead
	tmp->pNext = NULL;
	//tmp unlink to next TCB
	
	return tmp;
}

void print_wq(){
	Thread* iter = WaitQHead;
	while(iter != NULL){
		printf("tid = %lu\n", iter->tid);
		iter = iter->pNext;

	}
	printf("print wait queue end\n");
}

