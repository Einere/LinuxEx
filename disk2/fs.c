#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "Disk.h"
#include "fs.h"
#include "Matrix.h"

#define SET true
#define RESET false

FileDescTable* pFileDescTable = NULL;

bool findMakeDir(int inoIndex, int fInoIndex, int fBlkIndex, Matrix* m, int depth);
bool myMakeDir(int inoIndex, int fInoIndex, int fBlkIndex, Matrix* m, int depth);

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
	for(int i = 0; i < BLOCK_SIZE*8; i++){ 
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
	for(int i = 0; i < NUM_OF_INODE_PER_BLK * INODELIST_BLKS / 8; i++){
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
	//start at 19 bit because 0~18 bit is for file system
	int i = 2;
	for(int j = 4; j >= 0; j--){
		if(!(pBlock[i] >> j & 1)) return (i * 8) + (7 - j);
	}
	
	//start 24 bit
	for(i = 3; i < BLOCK_SIZE; i++){
		//if pBlock[i] == 11111111, continue
		if(pBlock[i] == -1) continue;
		
		for(int j = 7; j >= 0; j--){
			if(!(pBlock[i] >> j & 1)) return (i * 8) + (7 - j);
		}
	}
}


int		OpenFile(const char* szFileName, OpenFlag flag)
{

}


int		WriteFile(int fileDesc, char* pBuffer, int length)
{

}

int		ReadFile(int fileDesc, char* pBuffer, int length)
{

}


int		CloseFile(int fileDesc)
{

}

int		RemoveFile(const char* szFileName)
{

}
bool findMakeDir(int inoIndex, int fInoIndex, int fBlkIndex, Matrix* m, int depth){
	//declare flag var
	bool exist = false;

	//get current inode
	Inode* root;
	GetInode(inoIndex, root);

	//alloc memory
	char* blkPtr = (char*)malloc(BLOCK_SIZE);

	//check file name is already exist
	//in direct ptr	
	for(int i = 0; i < NUM_OF_DIRECT_BLK_PTR; i++){
		//read blcok, cast to DirEntry
		//check dirBlkPtr is NULL => if NULL, no more block is alloced
		if(root->dirBlkPtr[i] != NULL) DevReadBlock(root->dirBlkPtr[i], blkPtr);
		else break; 
		DirEntry* dirPtr = (DirEntry*)blkPtr;
	
		//compare paresed name with DirEntry's name
		for(int j = 0; j < NUM_OF_DIRENT_PER_BLK; j++){
			//if file is already exist
			if(dirPtr[j] != NULL && strcmp(m->array[depth], dirPtr[j]->name) == 0){
				exist = true;
				i = NUM_OF_DIRECT_BLK_PTR;
				break;
			}
		}
	}
	//check if exist is false & indir pointer in not null
	if(!exist && root->indirBlkPointer != NULL){
		//get first inode
		Inode* first;
		GetInode(root->indirBlkPointer, first);

		//check first inode's dir block
		for(int i=0; i<NUM_OF_DIRECT_BLK_PTR; i++){
			//read blcok, cast to DirEntry
			//check dirBlkPtr is NULL => if NULL, no more block is alloced
			if(first->dirBlkPtr[i] != NULL) DevReadBlock(first->dirBlkPtr[i], blkPtr);
			else break;
			DirEntry* dirPtr = (DirEntry*)blkPtr;
			
			//compare paresed name with DirEntry's name
			for(int j = 0; j < NUM_OF_DIRENT_PER_BLK; j++){
				//if file is already exist
				if(dirPtr[j] != NULL && strcmp(m->array[depth], dirPtr[j]->name) == 0){
					exist = true;
					i = NUM_OF_DIRECT_BLK_PTR;
					break;
				}
			}
		}
	}
	//if file name is exist
	if(exist){
		//call recursive func
		findMakeDir();
	}
	//if file name isn't exist
	else{
		myMakeDir();
	}
	//free
	free(root);
	free(blkPtr);
}

bool myMakeDir(int inoIndex, int fInoIndex, int fBlkIndex, Matrix* m, int depth){
	//get current inode
	Inode* root;
	GetInode(inoIndex, root);

	//for all dirBlkpPtr	
	for(int i = 0; i < NUM_OF_DIRECT_BLK_PTR; i++){
		//read blcok, cast to DirEntry
		//check dirBlkPtr is NULL => if NULL, no more block is alloced
		if(root->dirBlkPtr[i] != NULL) DevReadBlock(root->dirBlkPtr[i], blkPtr);
		else break; 
		DirEntry* dirPtr = (DirEntry*)blkPtr;
	
		//for all DirEntry
		for(int j = 0; j < NUM_OF_DIRENT_PER_BLK; j++){
			//if find empty DirEntry
			if(dirPtr[j] == NULL){
				//set dirEntry
				strcpy(dirPtr[j].name, m->array[depth]);
				dirPtr[j].inodeNum = fInoIndex;
					
				//write to disk
				DevWriteBlock(root->dirBlkPtr[i], dirPtr[j]);
					
				i = NUM_OF_DIRECT_BLK_PTR;
				break;
			}
		}
	}
	//check if indir pointer in not null
	if(root->indirBlkPointer != NULL){
		//get first inode
		Inode* first;
		GetInode(root->indirBlkPointer, first);

		//for all first's dirBlkPtr
		for(int i=0; i<NUM_OF_DIRECT_BLK_PTR; i++){
			//read blcok, cast to DirEntry
			//check dirBlkPtr is NULL => if NULL, no more block is alloced
			if(first->dirBlkPtr[i] != NULL) DevReadBlock(first->dirBlkPtr[i], blkPtr);
			else break;
			DirEntry* dirPtr = (DirEntry*)blkPtr;
				
			//for all DirEntry
			for(int j = 0; j < NUM_OF_DIRENT_PER_BLK; j++){
				//if file is already exist
				if(dirPtr[j] == NULL){
					//set dirEntry
					strcpy(dirPtr[j].name, m->array[depth]);
					dirPtr[j].inodeNum = fInoIndex;
					
					//write to disk
					DevWriteBlock(root->dirBlkPtr[i], dirPtr[j]);
					
					i = NUM_OF_DIRECT_BLK_PTR;
					break;
				}
			}
		}
	}

	//need to dir make 3
}

int		MakeDir(const char* szDirName)
{
	//for save string
	Matrix m;
	initMatrix(&m, 2);

	//split path
	char* parsePtr = strtok(szDirName, "/");
	
	//parse, save string
	while(parsePtr != NULL){
		inseirtMatrix(&m, parsePtr);
		parsePtr = strtok(szDirName, "/");
	}
	
	//get free block index
	int fBlkIndex = GetFreeBlockNum();
	int fInoIndex = GetFreeInodeNum();

	findMakeDir();
}


int		RemoveDir(const char* szDirName)
{

}


void		EnumerateDirStatus(const char* szDirName, DirEntry* pDirEntry, int* pNum)
{

}
