#include "Thread.h"
#include "MsgQueue.h"

/* 반드시 구현할 필요는 없음. 만일 구현했다면, Init.c에 있는 Init()에 추가해야 함.*/ 
void _InitMsgQueue(void)
{
}

int 	mymsgget(int key, int msgflg)
{
	return 0;
}

int 	mymsgsnd(int msqid, const void *msgp, int msgsz, int msgflg)
{
	return 0;
}

int	mymsgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg)
{
	return 0;
}

int 	mymsgctl(int msqid, int cmd, void* buf)
{
	return 0;
}