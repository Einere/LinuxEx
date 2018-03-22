#include <stdio.h>
#include "fs.h"

void FileSysInit(void)
{
	char* pInit = (char*)malloc(512);
	memset(pInit, 0, 512);
	for(int i = 0; i < 7; i++){ 
		DevWriteBlock(i, pInit);
	}
	free(pInit);
}

void SetInodeBitmap(int blkno)
{
	char* pIB = (char*)malloc(512);
	DevReadBlock(INODE_BITMAP_BLK_NUM, pIB);
	pIB |= (1 >> bit_num + 1); //block number? bit number?
	DevWriteBlock(INODE_BITMAP_BLK_NUM, pIB);
	free(pIB);
}


void ResetInodeBitmap(int blkno)
{
	char* pIB = (char*)malloc(512);
	DevReadBlock(INODE_BITMAP_BLK_NUM, pIB);
	pIB &= ~(1 >> bit_num + 1); //block number? bit number?
	DevWriteBlock(INODE_BITMAP_BLK_NUM, pIB);
	free(pIB);

}

void SetBlockBitmap(int blkno)
{
	char* pBB = (char*)malloc(512);
	DevReadBlock(BLOCK_BITMAP_BLK_NUM, pBB);
	pBB |= (1 >> bit_num + 1); //block number? bit number?
	DevWriteBlock(BLOCK_BITMAP_BLK_NUM, pBB);
	free(pBB);
}


void ResetBlockBitmap(int blkno)
{
	char* pBB = (char*)malloc(512);
	DevReadBlock(BLOCK_BITMAP_BLK_NUM, pBB);
	pBB &= ~(1 >> bit_num + 1); //block number? bit number?
	DevWriteBlock(BLOCK_BITMAP_BLK_NUM, pBB);
	free(pBB);
}


void PutInode(int blkno, Inode* pInode)
{
		
}


void GetInode(int blkno, Inode* pInode)
{
	char* pblock = (char*)malloc(512);
	int block_num = Inode_num / 8 + INODELIST_BLK_FIRST; //block number? inode number?
	DevReadBlock(block_num, pblock);
	__Inode pInode = (__Inode)malloc(sizeof(__Inode));
}


int GetFreeInodeNum(void)
{

}


int GetFreeBlockNum(void)
{

}






