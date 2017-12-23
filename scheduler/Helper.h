#include <stdbool.h>
#include <pthread.h>
#include "MsgQueue.h"
pthread_cond_t static_cond; 
pthread_mutex_t static_mutex;
bool is_pushed;

/* join structur */
typedef struct _JoinStr JoinStr;
typedef struct _JoinStr{
	pthread_t tid;
	pthread_t parentTid;
	JoinStr* pPrev;
	JoinStr* pNext;
} JoinStr;

/* head for running queue */
Thread*     RunQHead;

/* head and tail for join queue*/
JoinStr* JoinQHead;
JoinStr* JoinQTail;

void* __wrapperFunc(void* arg);

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

void jq_push(JoinStr *in_JCB);
JoinStr* jq_search(pthread_t s_tid);
JoinStr* jq_remove(pthread_t r_tid);
JoinStr* jq_pop();
void print_jq();


void mq_push(Qcb *qcb, Message* in_msg);
Message* mq_search(Qcb* qcb, long s_type);
void mq_remove(Qcb* qcb, long r_type);
Message* mq_pop(Qcb* qcb);
void print_mq(Qcb* qcb);

void tq_push(Qcb* qcb, Thread *tcb);
Thread* tq_all_search(thread_t s_tid);
Thread* tq_search(Qcb* qcb, long s_type);
Thread* tq_remove(Qcb* qcb, long r_type);
Thread* tq_pop(Qcb* qcb);
void print_tq(Qcb* qcb);


