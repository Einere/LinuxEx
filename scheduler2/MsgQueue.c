#include "MsgQueue.h"
#include "Helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <string.h>

/* �ݵ�� ������ �ʿ�� ����. ���� �����ߴٸ�, Init.c�� �ִ� Init()�� �߰��ؾ� ��.*/ 
void _InitMsgQueue(void){
	//reset qcbTblEntry's key to be -1, pQcb to be NULL
	for(int i = 0; i < MAX_QCB_SIZE; i++){   
		qcbTblEntry[i].key = -1;
		qcbTblEntry[i].pQcb = NULL;
	}
}

int	mymsgget(int key, int msgflg){
	int i;
	
	//if already exist qcb with key
	for(i = 0; i < MAX_QCB_SIZE; i++){
		if(qcbTblEntry[i].key == key) return i;
	}

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
			
			//fprintf(stderr, "qcbTblEntry[%d].key = %d\n", i, qcbTblEntry[i].key);
			return i;
		}
	}
	
	//if no index to create msgQ, set index to be -1
	return -1;
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

		//push message at qcb's messsage queue
		mq_push(tmp_qcb, tmp_msg);

		//if exist wating thread until message having type is push at message queue
		Thread* wait_tcb = NULL;
		if((wait_tcb = tq_remove(tmp_qcb, tmp_msg->type)) != NULL){
			wait_tcb->status = THREAD_STATUS_READY;
			rq_push(wait_tcb);
		}
	}
	//if no exist qcb, set msgsz to be -1
	else {
		perror("no exist message Q with qid, msgsz is -1 (mymsgsnd)");
		msgsz = -1;
	}
	//printf("tmp_qcb(key = %d)->pMsgTail->data = %s...\n", qcbTblEntry[msqid].key, tmp_qcb->pMsgTail->data);	
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
			
			//set msgsz to actual read size 
			msgsz = strlen(my_msgp->data);

			//remove searching message from message list
			mq_remove(tmp_qcb, msgtyp);
		}
		//if don't exist searching message, caller will be blocked
		else{
			Thread* tcb = __getThread(thread_self());
			RunQHead = NULL;

			//if(tcb == NULL) fprintf(stderr, "tcb(tid = %d) is null (mymsgrcv)\n", (int)thread_self());
			//else fprintf(stderr, "tcb(tid = %d) is exist (mymsgrcv)\n", (int)thread_self());
			tq_push(tmp_qcb, tcb);
			tcb->status = THREAD_STATUS_BLOCKED;
			tcb->type = msgtyp;
			tcb->bRunnable = false;

			__thread_wait_handler(1);
			
			//if searching message is pushed, and caller is runned by scheduler, find searching message
			//need to check qcb is exist
			if((tmp_qcb = qcbTblEntry[msqid].pQcb) != NULL){
				if((iter = mq_search(tmp_qcb, msgtyp)) != NULL){
					//copy iter's content to my_msgp
					my_msgp->type = iter->type;
					strncpy(my_msgp->data, iter->data, msgsz);
					my_msgp->size = iter->size;

					//set msgsz to actual read size 
					msgsz = strlen(my_msgp->data);

					//remove searching message from message Q
					mq_remove(tmp_qcb, msgtyp);
				}	
			}
			else fprintf(stderr, "i wake up. but no qcb in table....(mymsgrcv)\n");
		}
	}
	//if no exist qcb, set msgsz to be -1
	if(qcbTblEntry[msqid].pQcb == NULL){
		perror("no exist message Q with qid, msgsz is setted -1 ");
		msgsz = -1;	
	}

	return msgsz;
}

//remove msq and reset qcbTblEntry[n] 
int mymsgctl(int msqid, int cmd, void* buf){
	Qcb* qcb = NULL;
		
	//if exist qcb at qcbTblEntry[msqid]
	if((qcb = qcbTblEntry[msqid].pQcb) != NULL){

		/*
		//free message queue
		Message* iter_msg = qcb->pMsgHead;
		while(iter_msg != NULL){
			Message* f_msg = iter_msg;
			iter_msg = iter_msg->pNext;
			free(f_msg);
			f_msg = NULL;
		}
		//fprintf(stderr, "iter_msg = %p (mymsgctl)\n", iter_msg);
		*/

		//if message is remain at qcb's message Q
		if(qcb->pMsgHead != NULL){
			return -1;	
		}
		fprintf(stderr, "no message in message Q(key = %d), move waiting tcb to ready Q (mymsgctl)\n", qcbTblEntry[msqid].key);
		//move all waiting tcb at ready Q
		Thread* m_tcb = NULL;
		while((m_tcb = tq_pop(qcb)) != NULL){
			//m_tcb->type = 0;
			m_tcb->status = THREAD_STATUS_READY;
			rq_push(m_tcb);
		}

		qcbTblEntry[msqid].pQcb = NULL;
		return 0;
	}
	//if no exist qcb at qcbTblEntry, return -1
	else return -1;
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
	tmp = NULL;
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
		printf("type = %ld,data = %s...\n", iter->type, iter->data);
		iter = iter->pNext;

	}
	printf("*****print messasge queue end*****\n\n");
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

//search TCB by tid in all waiting queue
Thread* tq_all_search(thread_t s_tid){
	Thread* tmp;
	Qcb* qcb;
	
	//for all qcb
	for(int i = 0; i < MAX_QCB_SIZE; i++){
		if((qcb = qcbTblEntry[i].pQcb) != NULL){
			tmp = qcb->pThreadHead;
		
			//search TCB at qcb's waiting queue
			while(tmp != NULL){
				if(tmp->tid == s_tid){
					return tmp;		
				}
				tmp = tmp->pNext;
			}
		}
	}
	//if no TCB in all qcb
	return NULL;
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
		perror("not exist searching TCB (tq_remove) ");
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
		perror("not exist TCB in waiting queue (tq_pop) ");
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
		printf("[print_tq]type = %ld, tid = %d...\n", iter->type, (int)iter->tid);
		iter = iter->pNext;
	}
	printf("*****print waiting queue end*****\n\n");
}


