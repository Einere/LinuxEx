#include <stdio.h>
#include "fs.h"
#include "disk.h"
#define BLOCK_SIZE 512

void setbit(char* ptr, int bit_index){
	int byte_index = bit_index / 8;
	int shift_num = bit_index % 8;
	printf("byte_index = %d, shift_num = %d\n", byte_index, shift_num);	
	*(ptr + byte_index) |= 1 << (8 - 1 - shift_num);
	//*(ptr + byte_index) |= 1 << shift_num;
}

void FileSysInit(void)
{
	char* pInit = (char*)malloc(BLOCK_SIZE);
	memset(pInit, 0, BLOCK_SIZE);
	for(int i = 0; i < 7; i++){ 
		DevWriteBlock(i, pInit);
	}
	free(pInit);
}

void SetInodeBitmap(int blkno)
{
	char* pIB = (char*)malloc(BLOCK_SIZE);
	DevReadBlock(INODE_BITMAP_BLK_NUM, pIB);
	setbit(pIB, blkno);
	DevWriteBlock(INODE_BITMAP_BLK_NUM, pIB);
	free(pIB);
}


void ResetInodeBitmap(int blkno)
{
	char* pIB = (char*)malloc(512);
	DevReadBlock(INODE_BITMAP_BLK_NUM, pIB);
	pIB &= ~(1 << (BLOCK_SIZE - 1 - bit_num)); //block number? bit number?
	DevWriteBlock(INODE_BITMAP_BLK_NUM, pIB);
	free(pIB);

}

void SetBlockBitmap(int blkno)
{
	char* pBB = (char*)malloc(512);
	DevReadBlock(BLOCK_BITMAP_BLK_NUM, pBB);
	pBB |= (1 << (BLOCK_SIZE - 1 - bit_num)); //block number? bit number?
	DevWriteBlock(BLOCK_BITMAP_BLK_NUM, pBB);
	free(pBB);
}


void ResetBlockBitmap(int blkno)
{
	char* pBB = (char*)malloc(512);
	DevReadBlock(BLOCK_BITMAP_BLK_NUM, pBB);
	pBB &= ~(1 << (BLOCK_SIZE - 1 - bit_num)); //block number? bit number?
	DevWriteBlock(BLOCK_BITMAP_BLK_NUM, pBB);
	free(pBB);
}


void PutInode(int blkno, Inode* pInode)
{
	char* pBlock = (char*)malloc(BLOCK_SIZE);
	int block_num = (Inode_num / 8) + INODELIST_BLK_FIRST; //block number? inode number?
	DevReadBlock(block_num, pBlock);
	memcpy(pBlock + (Inode_num % 8) * sizeof(__Inode), pInode, sizeof(__Inode));
	DevWriteBlock(block_num, pBlock);	
	free(pBlock);
}


void GetInode(int blkno, Inode* pInode)
{
	char* pBlock = (char*)malloc(BLOCK_SIZE);
	int block_num = (Inode_num / 8) + INODELIST_BLK_FIRST; //block number? inode number?
	DevReadBlock(block_num, pBlock);
	memcpy(pInode, pBlock + (Inode_num % 8) * sizeof(__Inode), sizeof(__Inode));
	free(pBlock);
}


int GetFreeInodeNum(void)
{
	char* pBlock = (char*)malloc(BLOCK_SIZE);
	DevReadBlock(INODE_BITMAP_BLK_NUM, pBlock);
	
	for(int i = (BLOCK_SIZE * 8) - 1; i >= 0; i--){
		if(!((*pBlock >> i) | 0)) return BLOCK_SIZE * 8 - i;	
	}
}


int GetFreeBlockNum(void)
{

}






