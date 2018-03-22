#include <stdio.h>
#include "fs.h"

void FileSysInit(void)
{
	__Inode pInit = (__Inode)malloc(sizeof(__Inode));
	memset(pInit, 0, sizeof(__Inode));
	for(int i = 0; i < 6; i++){ 
		DevWriteBlock(i, pInit);
	}
}
void SetInodeBitmap(int blkno)
{

}


void ResetInodeBitmap(int blkno)
{

}


void SetBlockBitmap(int blkno)
{

}


void ResetBlockBitmap(int blkno)
{

}


void PutInode(int blkno, Inode* pInode)
{

}


void GetInode(int blkno, Inode* pInode)
{

}


int GetFreeInodeNum(void)
{

}


int GetFreeBlockNum(void)
{

}






