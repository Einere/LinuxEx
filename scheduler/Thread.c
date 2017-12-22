#include "Thread.h"
#include "Init.h"
#include "Scheduler.h"
#include "Helper.h"
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void* __wrapperFunc(void* arg){
	
	WrapperArg* pArg = (WrapperArg*)arg;
	//arg is wrapperArg, need to casting WrapperArg type  
	
	Thread* tmp = (Thread*)malloc(sizeof(Thread));
	JoinStr* tmp_j = (JoinStr*)malloc(sizeof(JoinStr));
	//make TCB and Join

	tmp->tid = thread_self();
	tmp->status = THREAD_STATUS_READY;
	tmp->parentTid = 0; 
	tmp->bRunnable = false;
	pthread_mutex_init(&(tmp->readyMutex), NULL);
	pthread_cond_init(&(tmp->readyCond), NULL);
	tmp->pPrev = tmp->pNext = NULL;
	tmp_j->tid = thread_self();
	tmp_j->parentTid = 0;
	tmp_j->pPrev = tmp_j->pNext = NULL;
	//set information
	
	rq_push(tmp);
	jq_push(tmp_j);
	is_pushed = true;
	//push tmp thread in ready queue
	
	pthread_mutex_lock(&static_mutex);	
	pthread_cond_signal(&static_cond);
	//send signal to parent therad
	pthread_mutex_unlock(&static_mutex);
	
	__thread_wait_handler(0);
	//block until thread is ready to run

	void* (*funcPtr)(void*) = pArg->funcPtr;
	void* funcArg = pArg->funcArg;

	//fprintf(stderr, "arg = %p (__wrapperFunc)\n", arg);	
	//fprintf(stderr, "pArg->funcPtr = %p (__wrapperFunc)\n", pArg->funcPtr);	
	
	void* ret = funcPtr(funcArg);
	
	//fprintf(stderr, "will run func?\n");	
	return ret;
}



int thread_create(thread_t *thread, thread_attr_t *attr, void* (*start_routine)(void *), void *arg){
	//make child thread and push in ready queue
	
	pthread_t tmp_tid;
	//for child thread's tid
	
	WrapperArg *wrapperArg = (WrapperArg*)malloc(sizeof(WrapperArg));
	//for wrapper
	
	wrapperArg->funcPtr = start_routine;
	wrapperArg->funcArg = arg;
	//set wrapper function, argument
	
	//fprintf(stderr, "parent create thread right now (thread_create)\n");	
	//fprintf(stderr, "start_routine address = %p (thread_create)\n", start_routine);	
	//fprintf(stderr, "wrapperArg->funcPtr address = %p (thread_create)\n", wrapperArg->funcPtr);	
	signal(SIGUSR1, __thread_wait_handler); 
	
	pthread_create(&tmp_tid, NULL, __wrapperFunc, wrapperArg);
	//create thread and make to execute __wrapperFunc
	//fprintf(stderr, "pthread_create called\n");
	
	pthread_mutex_lock(&static_mutex);
	while(is_pushed == false) pthread_cond_wait(&static_cond, &static_mutex);
	is_pushed = false;
	__getThread(tmp_tid)->parentTid = thread_self();
	pthread_mutex_unlock(&static_mutex);

	//fprintf(stderr, "parent received signal from child thread (thread_create())\n");
	*thread = tmp_tid;
	//call back created thread's tid
	return 0;
}

int thread_join(thread_t thread, void **retval){
	//parent thread block until child thread exit

	//fprintf(stderr, "thread_join called\n");
	Thread* p_TCB = __getThread(thread_self());
	//get parent's TCB
	Thread* c_TCB = __getThread(thread);
	//get child's TCB
	RunQHead = NULL;
	wq_push(p_TCB);
	if(p_TCB == NULL) RunQHead = NULL;
	//because parent must be running
	
	JoinStr* JCB;
	if((JCB = jq_search(thread)) == NULL) fprintf(stderr, "child's JoinStr isn't exist\n");
	
	p_TCB->bRunnable = false;
	
	if(JCB->parentTid == thread_self()){
		//if child exit befor join, don't call handler
	}
	else if(JCB->parentTid == '\0'){
		//if child exit after join
		JCB->parentTid = thread_self();
		__thread_wait_handler(0);
	}

	*retval = c_TCB->pExitCode;
	
	if(c_TCB != NULL){
		rq_remove(thread);
		free(c_TCB);
	}
	if(JCB != NULL){
		jq_remove(thread);
		free(JCB);
	}
	//remove TCB and JCB, free memory
	
	return 0;
}

int thread_exit(void* retval){
	Thread* c_TCB;
	if((c_TCB = __getThread(thread_self())) == NULL) c_TCB = RunQHead;
	//if caller is running thread
	
	Thread* p_TCB = __getThread(c_TCB->parentTid);
	//get parent's TCB
	
	JoinStr* JCB = jq_search(thread_self());
	//get child's JCB

	if(p_TCB != NULL){
		//parent's TCB is exist
		//fprintf(stderr, "parent TCB isn't NULL (thread_exit)\n");
		if(JCB->parentTid == c_TCB->parentTid){
			//child is joined
			wq_remove(c_TCB->parentTid);
			rq_push(p_TCB);
			//fprintf(stderr, "pass ready queue joined parent (thread_exit)\n");
			//wake up parent
		}
		else{
			//child isn't joined yet
			JCB->parentTid = c_TCB->parentTid;
			//fprintf(stderr, "no parent joined me (thread_exit)\n");
		}
	}
	else{
		//fprintf(stderr, "parent TCB is NULL (thread_exit)\n");
	}
	c_TCB->pExitCode = retval;
	//fprintf(stderr, "i will die soon (thread_exit)\n");
	return 0;
}

int thread_suspend(thread_t tid){
	Thread* tmp = __getThread(tid);

	tmp = rq_remove(tid);
	wq_push(tmp);
	tmp->status = THREAD_STATUS_BLOCKED;

	return 0;
}

int	thread_resume(thread_t tid){
	Thread* tmp = __getThread(tid);

	tmp = wq_remove(tid);
	rq_push(tmp);
	tmp->status = THREAD_STATUS_READY;

	return 0;
}

thread_t thread_self(){
	return pthread_self();	
}	

void __thread_wait_handler(int signo){
	//call when send thread to ready queue
	
	Thread* tmp;
	//fprintf(stderr, "[%ld]wait_handler is called (__thread_wait_handler())\n", thread_self());
	
	tmp = __getThread(thread_self());
	//get caller's TCB pointer
	
	//fprintf(stderr, "hello, hello, hello, hello (__thread_wait_handler)\n");	
	pthread_mutex_lock(&(tmp->readyMutex));
	//lock mutex
	
	//fprintf(stderr, "[%ld]i lock mutex (__thread_wait_handler())\n", thread_self());
	while(tmp->bRunnable == false){
		//fprintf(stderr, "[%ld]caller's bRunnable is false (__thread_wait_handler())\n", thread_self());
		pthread_cond_wait(&(tmp->readyCond), &(tmp->readyMutex));
		//fprintf(stderr, "[%ld]receive signal to wake up (__thread_wait_handler())\n", thread_self());
	}
	//wait until bRunnable be true
	pthread_mutex_unlock(&(tmp->readyMutex));
	//unlock mutex
	//fprintf(stderr, "[%ld]i unlock mutex (__thread_wait_handler())\n", thread_self());
}

Thread* __getThread(thread_t tid){
	//serach TCB with tid at ready queue and wait queue
	Thread* tmp;

	if((tmp = rq_search(tid)) != NULL){
		//fprintf(stderr, "TCB is in ready queue (__getThread())\n");
		return tmp;
	}
	//if TCB is in ready queue
	if((tmp = wq_search(tid)) != NULL){
		//fprintf(stderr, "TCB is in wait queue (__getThread())\n");	
		return tmp;
	}
	//if TCB is in wait queue
	if(RunQHead != NULL){
		if(RunQHead->tid == tid) return RunQHead;
	}
	//if TCB is in Run queue
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
		//fprintf(stderr, "insert empty ready queue\n");
		ReadyQHead = in_TCB;
		ReadyQTail = in_TCB;
	}
	else{
		//if ready queue is not empty
		//fprintf(stderr, "insert after ReadyQTail\n");
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
	//fprintf(stderr, "rq_search - start\n");	
	while(tmp != NULL){ //or tmp != readyQTail
		//search until last TCB
		if(tmp->tid == s_tid){
			//fprintf(stderr, "rq_search - find\n");
			return tmp;
		}
		tmp = tmp->pNext;
		//advance tmp
	}
	//fprintf(stderr, "rq_search - not founded\n");
	return tmp;
	//if not founded
}

Thread* rq_remove(pthread_t r_tid){
	//remove corresponding TCB from ready queue
	Thread* tmp;
	
	if((tmp = rq_search(r_tid)) == NULL){
		perror("not exist searching TCB : ");
		return NULL;
	}
	//printf("exist searching TCB (tid = %lu) (rq_remove)\n",r_tid);
	
	if(tmp->pPrev != NULL){
		//previous TCB link to next TCB
		tmp->pPrev->pNext = tmp->pNext;
		
		if(tmp->pNext != NULL) ReadyQTail = tmp->pNext;
		else ReadyQTail = tmp->pPrev;
	}
	if(tmp->pNext != NULL){
		//next TCB link to previous TCB
		tmp->pNext->pPrev = tmp->pPrev;

		if(tmp->pPrev != NULL) ReadyQHead = tmp->pPrev;
		else ReadyQHead = tmp->pNext;
	}
	if(tmp->pPrev == NULL && tmp->pNext == NULL) ReadyQHead = ReadyQTail = NULL;

	//printf("link prev and next complete (rq_remove)\n");

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
		printf("tid = %d\n", (int)iter->tid);
		iter = iter->pNext;

	}
	printf("print ready queue end\n");
}

void wq_push(Thread *in_TCB){
	if(WaitQHead == NULL){
		//if wait queue is empty
		//fprintf(stderr, "insert empty wait queue\n");
		WaitQHead = in_TCB;
		WaitQTail = in_TCB;
	}
	else{
		//if wait queue is not empty
		//fprintf(stderr, "insert after WaitQTail\n");
		in_TCB->pPrev = WaitQTail;
		//in_TCB link to previous TCB
		WaitQTail->pNext = in_TCB;
		//previous TCB link to in_TCB
		WaitQTail = WaitQTail->pNext;
		//advance ReadyQTail
	}
}

Thread* wq_search(pthread_t s_tid){
	Thread* tmp;
	tmp = WaitQHead;
	//fprintf(stderr, "wq_search - start\n");	
	while(tmp != NULL){ 
		//search until last TCB
		if(tmp->tid == s_tid){
			//fprintf(stderr, "wq_search - find\n");
			return tmp;
		}
		tmp = tmp->pNext;
		//advance tmp
	}
	//fprintf(stderr, "wq_search - not founded\n");
	return tmp;
	//if not founded
}

Thread* wq_remove(pthread_t r_tid){
	//remove corresponding TCB from wait queue
	Thread* tmp;
	
	if((tmp = wq_search(r_tid)) == NULL){
		perror("not exist searching TCB : ");
		return NULL;
	}
	//printf("exist searching TCB (tid = %lu)\n",r_tid);

	if(tmp->pPrev != NULL){
		//previous TCB link to next TCB
		tmp->pPrev->pNext = tmp->pNext;
		
		if(tmp->pNext != NULL) WaitQTail = tmp->pNext;
		else WaitQTail = tmp->pPrev;
	}
	if(tmp->pNext != NULL){
		//next TCB link to previous TCB
		tmp->pNext->pPrev = tmp->pPrev;

		if(tmp->pPrev != NULL) WaitQHead = tmp->pPrev;
		else WaitQHead = tmp->pNext;
	}
	if(tmp->pPrev == NULL && tmp->pNext == NULL) WaitQHead = WaitQTail = NULL;

	//printf("link prev and next complete\n");

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
		printf("tid = %d\n", (int)iter->tid);
		iter = iter->pNext;

	}
	printf("print wait queue end\n");
}

void jq_push(JoinStr* in_JCB){
	if(JoinQHead == NULL){
		//if wait queue is empty
		//fprintf(stderr, "insert empty wait queue\n");
		JoinQHead = in_JCB;
		JoinQTail = in_JCB;
	}
	else{
		//if wait queue is not empty
		//fprintf(stderr, "insert after WaitQTail\n");
		in_JCB->pPrev = JoinQTail;
		//in_TCB link to previous TCB
		JoinQTail->pNext = in_JCB;
		//previous TCB link to in_TCB
		JoinQTail = JoinQTail->pNext;
		//advance ReadyQTail
	}
}

JoinStr* jq_search(pthread_t s_tid){
	JoinStr* tmp;
	tmp = JoinQHead;
	//fprintf(stderr, "wq_search - start\n");	
	while(tmp != NULL){ 
		//search until last TCB
		if(tmp->tid == s_tid){
			//fprintf(stderr, "wq_search - find\n");
			return tmp;
		}
		tmp = tmp->pNext;
		//advance tmp
	}
	//fprintf(stderr, "wq_search - not founded\n");
	return tmp;
	//if not founded
}

JoinStr* jq_remove(pthread_t r_tid){
	//remove corresponding TCB from wait queue
	JoinStr* tmp;
	
	if((tmp = jq_search(r_tid)) == NULL){
		perror("not exist searching TCB : ");
		return NULL;
	}
	//printf("exist searching TCB (tid = %lu)\n",r_tid);

	if(tmp->pPrev != NULL){
		//previous TCB link to next TCB
		tmp->pPrev->pNext = tmp->pNext;
		
		if(tmp->pNext != NULL) JoinQTail = tmp->pNext;
		else JoinQTail = tmp->pPrev;
	}
	if(tmp->pNext != NULL){
		//next TCB link to previous TCB
		tmp->pNext->pPrev = tmp->pPrev;

		if(tmp->pPrev != NULL) JoinQHead = tmp->pPrev;
		else JoinQHead = tmp->pNext;
	}
	if(tmp->pPrev == NULL && tmp->pNext == NULL) JoinQHead = JoinQTail = NULL;

	//printf("link prev and next complete\n");

	tmp->pPrev = NULL;
	tmp->pNext = NULL;
	//init tmp
	
	return tmp;
}

JoinStr* jq_pop(){
	//pop head TCB from wait queue
	JoinStr* tmp;
	tmp = JoinQHead;

	if(tmp == NULL){
		perror("not exist TCB in wait queue : ");
		return NULL;
	}

	tmp->pNext->pPrev = NULL;
	//next TCB unlink to tmp
	JoinQHead = tmp->pNext;
	//advance WaitQHead
	tmp->pNext = NULL;
	//tmp unlink to next TCB
	
	return tmp;
}

void print_jq(){
	JoinStr* iter = JoinQHead;
	while(iter != NULL){
		printf("tid = %d\n", (int)iter->tid);
		iter = iter->pNext;
	}
	printf("print join queue end\n");
}

