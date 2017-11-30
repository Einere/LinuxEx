#ifndef __THREAD_H__
#define __THREAD_H__

#include <pthread.h>



#define TIMESLICE	(2)


typedef int BOOL;
typedef pthread_t	thread_t;
typedef void thread_attr_t;


typedef enum{
	THREAD_STATUS_RUN = 0,
	THREAD_STATUS_READY = 1,
	THREAD_STATUS_BLOCKED = 2,
	THREAD_STATUS_ZOMBIE 3, 
}ThreadStatus;

typedef struct _Thread Thread;
typedef struct _Thread {
	ThreadStatus			status;
    	pthread_t			tid;
    	pthread_cond_t     		readyCond;
   	BOOL				bRunnable;
   	pthread_mutex_t   		readyMutex;
	pthread_t			parentTid;
	Thread*				pPrev;
	Thread*				pNext;
} Thread;	

typedef struct __wrapperArg {
   void*  (*funcPtr)(void*);
   void* funcArg;
} WrapperArg;


/* head and tail pointers for ready queue */ 
Thread* 	ReadyQHead;
Thread*		ReadyQTail;

/* head and tail pointers for waiting queue */
Thread*		WaitQHead;
Thread*		WaitQTai;


int 		thread_create(thread_t *thread, thread_attr_t *attr, void *(*start_routine) (void *), void *arg);
int 		thread_join(thread_t thread, void **retval);
int 		thread_suspend(thread_t tid);
int		thread_resume(thread_t tid);
thread_t 	thread_self();




#endif /* __THREAD_H__ */


