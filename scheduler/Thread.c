#include "Thread.h"
#include "Init.h"
#include "Scheduler.h"
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void* __wrapperFunc(void* arg){
	//!!!!!!!!!! need to confirm !!!!!!!!!!
	fprintf(stderr, "__wrapperFunc() execute\n");	
	
	void* ret;
	WrapperArg* pArg = (WrapperArg*)arg;
	//arg is wrapperArg, need to casting WrapperArg type  
	
	Thread* tmp = (Thread*)malloc(sizeof(Thread));
	//make TCB

	tmp->tid = thread_self();
	tmp->status = THREAD_STATUS_READY;
	tmp->parentTid = 0; 
	tmp->bRunnable = false;
	pthread_mutex_init(&(tmp->readyMutex), NULL);
	pthread_cond_init(&(tmp->readyCond), NULL);
	tmp->pPrev = tmp->pNext = NULL;
	//set information
	
	rq_push(tmp);
	is_pushed = true;
	//push tmp thread in ready queue
	
	pthread_mutex_lock(&static_mutex);	
	pthread_cond_signal(&static_cond);
	//send signal to parent therad
	pthread_mutex_unlock(&static_mutex);
/*
	sigset_t set;
	int retSig;
	//for handler
	
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
	sigwait(&set, &retSig);
	fprintf(stderr, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
	//wait until TCB is pushed in ready queue
	//if TCB is pushed in ready queue, parent thread send SIGUSR1 to child thread to execute handler
*/	
	__thread_wait_handler(0);
	//block until thread is ready to run

	void* (*funcPtr)(void*) = pArg->funcPtr;
	void* funcArg = pArg->funcArg;
	fprintf(stderr, "pArg->funcPtr = %p, pArg->funcArg = %p\n", pArg->funcPtr, pArg->funcArg);
	
	fprintf(stderr, "funcPtr(funcArg) = %d",funcPtr(funcArg));
	ret = funcPtr(funcArg);
	//위에서 세그폴트가 뜨는것 같음. 수정 필요.
	
	fprintf(stderr, "will run func?\n");	
	return ret;
}



int thread_create(thread_t *thread, thread_attr_t *attr, void* (*start_routine)(void *), void *arg){
	//make child thread and push in ready queue
	//!!!!!!!!!! need to confirm !!!!!!!!!!	
	
	pthread_t tmp_tid;
	//for child thread's tid
	
	WrapperArg wrapperArg;
	//for wrapper
	
	wrapperArg.funcPtr = start_routine;
	wrapperArg.funcArg = arg;
	//set wrapper function, argument
	
	fprintf(stderr, "parent create thread right now\n");	
	signal(SIGUSR1, __thread_wait_handler); 
	
	pthread_create(&tmp_tid, NULL, __wrapperFunc, &wrapperArg);
	//create thread and make to execute __wrapperFunc
	
	fprintf(stderr, "pthread_create called\n");
	
	pthread_mutex_lock(&static_mutex);
	while(is_pushed == false) pthread_cond_wait(&static_cond, &static_mutex);
	is_pushed = false;
	pthread_mutex_unlock(&static_mutex);

	fprintf(stderr, "parent received signal from child thread (thread_create())\n");
	*thread = tmp_tid;
	//call back created thread's tid
	//printf("parent send signal right now\n");
	//pthread_kill(tmp_tid, SIGUSR1);
	//make child thread call handler by sending SIGUSR1
	return 0;
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
	fprintf(stderr, "wait_handler is called (__thread_wait_handler())\n");
	
	tmp = __getThread(pthread_self());
	//get caller's TCB pointer
	
	pthread_mutex_lock(&(tmp->readyMutex));
	//lock mutex
	
	fprintf(stderr, "i lock mutex (__thread_wait_handler())\n");
	while(tmp->bRunnable == false){
		fprintf(stderr, "caller's bRunnable is false (__thread_wait_handler())\n");
		pthread_cond_wait(&(tmp->readyCond), &(tmp->readyMutex));
		fprintf(stderr, "receive signal to wake up (__thread_wait_handler())\n");
	}
	//wait until bRunnable be true
	pthread_mutex_unlock(&(tmp->readyMutex));
	//unlock mutex
	fprintf(stderr, "i unlock mutex (__thread_wait_handler())\n");
}

Thread* __getThread(thread_t tid){
	//serach TCB with tid at ready queue and wait queue
	Thread* tmp;

	if((tmp = rq_search(tid)) != NULL){
		fprintf(stderr, "TCB is in ready queue (__getThread())\n");
		return tmp;
	}
	//if TCB is in ready queue
	if((tmp = wq_search(tid)) != NULL){
		fprintf(stderr, "TCB is in wait queue (__getThread())\n");	
		return tmp;
	}
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
	pTCB->status = THREAD_STATUS_RUN;
	//set status to run
	pthread_mutex_unlock(&(pTCB->readyMutex));
	//unlock mutex
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
	Thread* tmp = ReadyQHead;

	if(tmp == NULL){
		perror("not exist TCB in ready queue : ");
		return NULL;
	}
	if(tmp->pNext != NULL) tmp->pNext->pPrev = NULL;
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

