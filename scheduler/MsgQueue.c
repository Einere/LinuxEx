#include "Thread.h"
#include "MsgQueue.h"

/* �ݵ�� ������ �ʿ�� ����. ���� �����ߴٸ�, Init.c�� �ִ� Init()�� �߰��ؾ� ��.*/ 
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