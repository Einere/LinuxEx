#ifndef __THREAD_H__
#define __THREAD_H__

#include <pthread.h>
#include <stdio.h>

#define TIMESLICE	(2)


typedef int BOOL;
typedef pthread_t	thread_t;
typedef void thread_attr_t;


typedef enum{
	THREAD_STATUS_RUN = 0,
	THREAD_STATUS_READY = 1,
	THREAD_STATUS_BLOCKED = 2,
	THREAD_STATUS_ZOMBIE = 3 
} ThreadStatus;

typedef struct _Thread Thread;
typedef struct _Thread {
	ThreadStatus		status;
	void*               pEixtCode;
    pthread_t			tid;
    pthread_cond_t     	readyCond;
   	BOOL				bRunnable;
   	pthread_mutex_t   	readyMutex;
	pthread_t			parentTid;
	Thread*				pPrev;
	Thread*				pNext;
} Thread;	

/* wrapper */
void* __wrapperFunc(void* arg);
typedef struct __wrapperArg {
   void* (*funcPtr)(void*);
   void* funcArg;
} WrapperArg;


/* head and tail pointers for ready queue */ 
Thread* 	ReadyQHead;
Thread*		ReadyQTail;

/* head and tail pointers for waiting queue */
Thread*		WaitQHead;
Thread*		WaitQTail;

/* user define functions */
int 		thread_create(thread_t *thread, thread_attr_t *attr, void *(*start_routine) (void *), void *arg);
int 		thread_join(thread_t thread, void **retval);
int 		thread_suspend(thread_t tid);
int			thread_resume(thread_t tid);
thread_t 	thread_self();

void __thread_wait_handler(int signo);
Thread* __getThread(thread_t tid);
void __thread_wakeup(Thread* pTCB);

void rq_push(Thread *in_TCB);
Thread* rq_search(pthread_t s_tid);
Thread* rq_remove(pthread_t r_tid);
Thread* rq_pop();
void print_rq();

void wq_push(Thread *in_TCB);
Thread* wq_search(pthread_t s_tid);
Thread* wq_remove(pthread_t r_tid);
Thread* wq_pop();
void print_wq();

#endif /* __THREAD_H__ */


