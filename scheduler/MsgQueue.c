#include "Thread.h"
#include "MsgQueue.h"
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
		Message* iter = tmp_qcb->pMsgHead;
		
		//find message with type from head to tail
		//it's better that search until iter is NULL because linked list
		for(int i = 0; i < tmp_qcb->msgCount; i++){
			if(iter->type == msgtyp) break; 
			else iter = iter->pNext;
		}
		/* !!!!!! if don't exist searching message, need to deal it !!!!! */
		
		//if find searching message
		if(iter != NULL){
			//copy iter's content to my_msgp
			my_msgp->type = iter->type;
			strncpy(my_msgp->data, iter->data, msgsz);
			my_msgp->size = iter->size;
			
			//reove searching message from message list
			if(iter->pPrev != NULL){
				iter->pPrev->pNext = iter->pNext;
				
				if(iter->pNext != NULL) tmp_qcb->pMsgTail = iter->pNext;
				else tmp_qcb->pMsgTail = iter->pPrev;
			}
			if(iter->pNext != NULL){
				iter->pNext->pPrev = iter->pPrev;

				if(iter->pPrev != NULL) tmp_qcb->pMsgHead = iter->pPrev;
				else tmp_qcb->pMsgHead = iter->pNext;
			}
			if(iter->pPrev == NULL && iter->pNext == NULL) tmp_qcb->pMsgHead = tmp_qcb->pMsgTail = NULL;
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
