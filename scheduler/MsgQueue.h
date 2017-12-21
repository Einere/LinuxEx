#ifndef __MSGQ_H__

#define __MSGQ_H__

#include "Thread.h"

#define MAX_MSG_SIZE	(1024)
#define MAX_QCB_SIZE	(32)


typedef struct _Message Message;
typedef struct _Message {
	long		type;
	char		data[MAX_MSG_SIZE];
	int	    	size;
	Message*	pPrev;
	Message*	pNext;
} Message;


typedef struct _Qcb {
	int 		msgCount;
   	Message*	pMsgHead;
    	Message*	pMsgTail;
	int 		waitThreadCount;
	Thread*		pThreadHead;
    Thread*		pThreadTail;
} Qcb;


typedef struct _QcbTblEntry {
	int		key;
	Qcb*    	pQcb;
} QcbTblEntry;	

QcbTblEntry  qcbTblEntry[MAX_QCB_SIZE];


void	_InitMsgQueue(void);
int 	mymsgget(int key, int msgflg);
int 	mymsgsnd(int msqid, const void *msgp, int msgsz, int msgflg);
int	mymsgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);
int 	mymsgctl(int msqid, int cmd, void* buf);



#endif /* __MSGQ_H__ */
