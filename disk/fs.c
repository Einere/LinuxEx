#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "fs.h"
#include "disk.h"

#define SET true
#define RESET false

void printbit2(char* ptr){
	for(int i = 0; i < 4; i++){
		for(int j = 7; j >= 0; j--){
			printf("%d", (ptr[i] >> j) & 1);
		}
		printf(" ");
	}
	printf("print finished!!!\n");
}

void read_do_bit(int bitmap_num, int bit_num, bool set_reset){
	//alloc temp block and read
	char* pB = (char*)malloc(BLOCK_SIZE);
	memset(pB, 0, BLOCK_SIZE);
	DevReadBlock(bitmap_num, pB);

	//calculate
	int byte_index = bit_num / 8;
	int shift_num = bit_num % 8;

	//set or reset bit
	if(set_reset){
		*(pB + byte_index) |= 1 << (8 - 1 - shift_num);
	}
	else{
		*(pB + byte_index) &= ~(1 << (8 - 1 - shift_num));
	}
	
	//write at disk block and free
	DevWriteBlock(bitmap_num, pB);
	free(pB);
}

void FileSysInit(void)
{
	//create and open virtual disk
	DevCreateDisk();
	DevOpenDisk();

	//alloc temp block and init
	char* pInit = (char*)malloc(BLOCK_SIZE);
	memset(pInit, 0, BLOCK_SIZE);

	//write at all disk blocks, free
	for(int i = 0; i < 7; i++){ 
		DevWriteBlock(i, pInit);
	}
	free(pInit);
}

void SetInodeBitmap(int inodeno)
{
	read_do_bit(INODE_BITMAP_BLK_NUM, inodeno, SET);
}


void ResetInodeBitmap(int inodeno)
{
	read_do_bit(INODE_BITMAP_BLK_NUM, inodeno, RESET);
}

void SetBlockBitmap(int blkno)
{
	read_do_bit(BLOCK_BITMAP_BLK_NUM, blkno, SET);
}

void ResetBlockBitmap(int blkno)
{
	read_do_bit(BLOCK_BITMAP_BLK_NUM, blkno, RESET);
}


void PutInode(int inodeno, Inode* pInode)
{
	//check inodeno is in 0~31
	if(inodeno >= INODELIST_BLKS * (BLOCK_SIZE / sizeof(Inode))) return;
	
	//alloc temp block and read
	char* pBlock = (char*)malloc(BLOCK_SIZE);
	int block_index = (inodeno / 8) + INODELIST_BLK_FIRST; 
	DevReadBlock(block_index, pBlock);
	
	//copy from pInode to inode list, write and free
	memcpy(pBlock + (inodeno % 8) * sizeof(Inode), pInode, sizeof(Inode));
	DevWriteBlock(block_index, pBlock);	
	free(pBlock);
}


void GetInode(int inodeno, Inode* pInode)
{
	//alloc temp block and read
	char* pBlock = (char*)malloc(BLOCK_SIZE);
	int block_index = (inodeno / 8) + INODELIST_BLK_FIRST; 
	DevReadBlock(block_index, pBlock);

	//copy from inode list to pInode and free
	memcpy(pInode, pBlock + (inodeno % 8) * sizeof(Inode), sizeof(Inode));
	free(pBlock);
}


int GetFreeInodeNum(void)
{
	//alloc temp block and read 
	char* pBlock = (char*)malloc(BLOCK_SIZE);
	DevReadBlock(INODE_BITMAP_BLK_NUM, pBlock);

	//find first zero bit and return bit index
	for(int i = 0; i <= BLOCK_SIZE - 1; i++){
		//if pBlock[i] == 11111111, continue
		if(pBlock[i] == -1) continue;

		for(int j = 7; j >= 0; j--){
			if(!(pBlock[i] >> j & 1)) return (i * 8) + (7 - j);
		}
	}
}

int GetFreeBlockNum(void)
{
	//alloc temp block and read 
	char* pBlock = (char*)malloc(BLOCK_SIZE);
	DevReadBlock(BLOCK_BITMAP_BLK_NUM, pBlock);

	//find first zero bit and return bit index
	for(int i = 0; i <= BLOCK_SIZE - 1; i++){
		//if pBlock[i] == 11111111, continue
		if(pBlock[i] == -1) continue;
		
		for(int j = 7; j >= 0; j--){
			if(!(pBlock[i] >> j & 1)) return (i * 8) + (7 - j);
		}
	}
}






