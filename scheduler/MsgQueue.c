#include "MsgQueue.h"
#include "Helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <string.h>

/* 반드시 구현할 필요는 없음. 만일 구현했다면, Init.c에 있는 Init()에 추가해야 함.*/ 
void _InitMsgQueue(void){
	//reset qcbTblEntry's key to be -1, pQcb to be NULL
	for(int i = 0; i < MAX_QCB_SIZE; i++){   
		qcbTblEntry[i].key = -1;
		qcbTblEntry[i].pQcb = NULL;
	}
}

int	mymsgget(int key, int msgflg){
	int i = -1;
	
	//find unused index at qcbTblEntry
	for(i = 0; i < MAX_QCB_SIZE; i++){
		//if unused index if fined
		if(qcbTblEntry[i].key == -1){
			//malloc and reset Qcb	
			Qcb* tmp_qcb = (Qcb*)malloc(sizeof(Qcb));
			tmp_qcb->msgCount = 0;
			tmp_qcb->pMsgHead = NULL;
			tmp_qcb->pMsgTail = NULL;
			tmp_qcb->waitThreadCount = 0;
			tmp_qcb->pThreadHead = NULL;
			tmp_qcb->pThreadTail = NULL;

			//set tmp_qcb at qcbTblEntry[i]
			qcbTblEntry[i].key = key;
			qcbTblEntry[i].pQcb = tmp_qcb;
			break;
		}
	}
	
	//if no index to create msgQ, set index to be -1
	if(i == MAX_QCB_SIZE) i = -1;
	
	if(i >= 0) fprintf(stderr, "qcbTblEntry[%d].key = %d\n", i, qcbTblEntry[i].key);
	else fprintf(stderr, "failed to get msgQ. index = %d\n", i);

	return i;
}

int mymsgsnd(int msqid, const void *msgp, int msgsz, int msgflg){
	Qcb* tmp_qcb = NULL;

	//get Qcb with msqid in qcbTblEntry
	if(qcbTblEntry[msqid].pQcb != NULL){
		tmp_qcb = qcbTblEntry[msqid].pQcb;
		Message* my_msgp = (Message*)msgp;

		//copy msgp's content to tmp_msg
		Message* tmp_msg = (Message*)malloc(sizeof(Message));
		tmp_msg->type = my_msgp->type;
		strcpy(tmp_msg->data, my_msgp->data);
		tmp_msg->size = msgsz;
		tmp_msg->pPrev = NULL;
		tmp_msg->pNext = NULL;

		//if tmp_qcb has no message
		mq_push(tmp_qcb, tmp_msg);
		/*
		if(tmp_qcb->pMsgHead == NULL){
			tmp_qcb->pMsgHead = tmp_msg;
			tmp_qcb->pMsgTail = tmp_msg;
			tmp_qcb->msgCount++;
		}
		//if tmp_qcb has any message
		else{
			tmp_qcb->pMsgTail->pNext = tmp_msg;
			tmp_msg->pPrev = tmp_qcb->pMsgTail;
			tmp_qcb->pMsgTail = tmp_msg;
			tmp_qcb->msgCount++;
		}
		*/
	}
	//if no exist qcb, set msgsz to be -1
	else {
		perror("no exist message Q with qid, msgsz is -1");
		msgsz = -1;
	}

	printf("tmp_qcb->pMsgTail->data = %s...\n", tmp_qcb->pMsgTail->data);	
	return msgsz;
}

int	mymsgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg){
	Qcb* tmp_qcb = NULL;
	Message* my_msgp = (Message*)msgp;

	//get Qcb with msqid in qcbTblEntry
	if(qcbTblEntry[msqid].pQcb != NULL){
		tmp_qcb = qcbTblEntry[msqid].pQcb;
		//Message* iter = tmp_qcb->pMsgHead;
		
		//find message with type from head to tail
		Message* iter = mq_search(tmp_qcb, msgtyp);
		
		//if find searching message
		if(iter != NULL){
			//copy iter's content to my_msgp
			my_msgp->type = iter->type;
			strncpy(my_msgp->data, iter->data, msgsz);
			my_msgp->size = iter->size;
			
			//reove searching message from message list
			mq_remove(tmp_qcb, msgtyp);
		}
		//if don't exist searching message
		else{
			Thread* tcb = __getThread(thread_self());
			RunQHead = NULL;
			
			tq_push(tmp_qcb, tcb);
			tcb->status = THREAD_STATUS_BLOCKED;
			tcb->type = msgtyp;
			
			__thread_wait_handler(1);
		}
	}
	//if no exist qcb, set msgsz to be -1
	else{
		perror("no exist message Q with qid, msgsz is -1");
		msgsz = -1;	
	}

	return msgsz;
}

int mymsgctl(int msqid, int cmd, void* buf){
	return 0;
}


//insert message at message queue
void mq_push(Qcb *qcb, Message* in_msg){
	//if message queue is empty
	if(qcb->pMsgHead == NULL){
		qcb->pMsgHead = in_msg;
		qcb->pMsgTail = in_msg;
		qcb->msgCount++;
	}
	//if message queue is not empty
	else{
		qcb->pMsgTail->pNext = in_msg;
		in_msg->pPrev = qcb->pMsgTail;
		qcb->pMsgTail = qcb->pMsgTail->pNext;
		qcb->msgCount++;
	}
}

//search 1st message by type in message queue
Message* mq_search(Qcb* qcb, long s_type){
	Message* tmp = qcb->pMsgHead;
	
	//search until last messasge
	while(tmp != NULL){
		if(tmp->type == s_type){
			return tmp;
		}
		tmp = tmp->pNext;
	}
	//if not founded, return null
	return tmp;
}

//remove 1st corresponding message from message queue
void mq_remove(Qcb* qcb, long r_type){
	Message* tmp = qcb->pMsgHead;
	
	//if no message in message queue
	if((tmp = mq_search(qcb, r_type)) == NULL){
		perror("not exist searching message : ");
		return ;
	}

	//if exist message in message queue
	if(tmp->pPrev != NULL){
		tmp->pPrev->pNext = tmp->pNext;
		
		if(tmp->pNext != NULL) qcb->pMsgTail = tmp->pNext;
		else qcb->pMsgTail = tmp->pPrev;
	}
	if(tmp->pNext != NULL){
		tmp->pNext->pPrev = tmp->pPrev;

		if(tmp->pPrev != NULL) qcb->pMsgHead = tmp->pPrev;
		else qcb->pMsgHead = tmp->pNext;
	}
	if(tmp->pPrev == NULL && tmp->pNext == NULL) qcb->pMsgHead = qcb->pMsgTail = NULL;

	//init tmp
	tmp->pPrev = NULL;
	tmp->pNext = NULL;
	qcb->msgCount--;
	
	free(tmp);
}

//pop head message from messsage queue
Message* mq_pop(Qcb* qcb){
	Message* tmp = qcb->pMsgHead;

	if(tmp == NULL){
		perror("not exist message in message queue : ");
		return NULL;
	}
	if(tmp->pNext != NULL) tmp->pNext->pPrev = NULL;
	
	qcb->pMsgHead = tmp->pNext;
	tmp->pNext = NULL;
	qcb->msgCount--;

	return tmp;
}

void print_mq(Qcb* qcb){
	Message* iter = qcb->pMsgHead;
	while(iter != NULL){
		printf("type = %ld\ndata = %s...\n", iter->type, iter->data);
		iter = iter->pNext;

	}
	printf("print messasge queue end\n");
}

//insert tcb at waiting queue
void tq_push(Qcb* qcb, Thread *tcb){
	//if ready queue is empty
	if(qcb->pThreadHead == NULL){
		qcb->pThreadHead = tcb;
		qcb->pThreadTail = tcb;
		qcb->waitThreadCount++;
	}
	//if ready queue is not empty
	else{
		qcb->pThreadTail->pNext = tcb;
		tcb->pPrev = qcb->pThreadTail;
		qcb->pThreadTail = qcb->pThreadTail->pNext;
		qcb->waitThreadCount++;
	}
}

//search TCB by s_type in waiting queue
Thread* tq_search(Qcb* qcb, long s_type){
	Thread* tmp = qcb->pThreadHead;
	
	//search until last TCB
	while(tmp != NULL){
		if(tmp->type == s_type){
			//fprintf(stderr, "rq_search - find\n");
			return tmp;
		}
		tmp = tmp->pNext;
	}
	
	//if not founded
	return tmp;
}

//remove corresponding TCB from ready queue
Thread* tq_remove(Qcb* qcb, long r_type){
	Thread* tmp;
	
	//if no tcb with r_type in waiting queue
	if((tmp = tq_search(qcb, r_type)) == NULL){
		perror("not exist searching TCB : ");
		return NULL;
	}

	//if exist tcb with r_type in waiting queue
	if(tmp->pPrev != NULL){
		tmp->pPrev->pNext = tmp->pNext;
		
		if(tmp->pNext != NULL) qcb->pThreadTail = tmp->pNext;
		else qcb->pThreadTail = tmp->pPrev;
	}
	if(tmp->pNext != NULL){
		tmp->pNext->pPrev = tmp->pPrev;

		if(tmp->pPrev != NULL) qcb->pThreadHead = tmp->pPrev;
		else qcb->pThreadHead = tmp->pNext;
	}
	if(tmp->pPrev == NULL && tmp->pNext == NULL) qcb->pThreadHead = qcb->pThreadTail = NULL;

	//init tmp
	tmp->pPrev = NULL;
	tmp->pNext = NULL;
	qcb->waitThreadCount--;
	
	return tmp;
}

//pop head TCB from waiting queue
Thread* tq_pop(Qcb* qcb){
	Thread* tmp = qcb->pThreadHead;

	//if no tcb in waiting queue
	if(tmp == NULL){
		perror("not exist TCB in waiting queue : ");
		return NULL;
	}
	//if exist tcb in waiting queue
	if(tmp->pNext != NULL) tmp->pNext->pPrev = NULL;
	qcb->pThreadHead = tmp->pNext;
	tmp->pNext = NULL;
	
	return tmp;
}

void print_tq(Qcb* qcb){
	Thread* iter = qcb->pThreadHead;
	while(iter != NULL){
		printf("type = %ld\ntid = %d...\n", iter->type, (int)iter->tid);
		iter = iter->pNext;
	}
	printf("print waiting queue end\n");
}

