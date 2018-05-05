#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "Disk.h"
#include "fs.h"
#include "Matrix.h"

#define SET true
#define RESET false

FileDescTable* pFileDescTable = NULL;

void copyDirEnt(DirEntry* target, DirEntry* source);
bool findName(int* inoIndex, Matrix* m, int depth, DirEntry* retDirPtr, int* retDirPtrIndex, int* retBlkIndex);
bool myMakeDir(Matrix* m, int depth, DirEntry* pDirPtr, int pDirPtrIndex, int pInoIndex, int pBlkIndex);
int myMakeFile(Matrix* m, int depth, DirEntry* pDirPtr, int pDirPtrIndex, int pInoIndex, int pBlkIndex);

void printbit(char* ptr){
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

void initFdTbl(){
	//alloc
	pFileDescTable = (FileDescTable*)malloc(sizeof(FileDescTable));
	
	//init with 0
	memset(pFileDescTable, 0, sizeof(FileDescTable));	
}

void copyDirEnt(DirEntry* target, DirEntry* source){
	for(int i = 0; i < NUM_OF_DIRENT_PER_BLK; i++){
		strcpy(target[i].name, source[i].name);
		target[i].inodeNum = source[i].inodeNum;
	}
}

bool findName(int* inoIndex, Matrix* m, int depth, DirEntry* retDirPtr, int* retDirPtrIndex, int* retBlkIndex){
	//stop condition
	if(m != NULL && m->array[depth] == NULL) return false;
	if(m != NULL) printf("seraching name : %s\n", m->array[depth]); 

	//get current inode
	Inode* root = (Inode*)malloc(sizeof(Inode));
	GetInode(*inoIndex, root);

	//alloc memory
	char* blkPtr = (char*)malloc(BLOCK_SIZE);
	
	//check file name is already exist
	//in direct ptr	
	for(int i = 0; i < NUM_OF_DIRECT_BLK_PTR; i++){
		//read blcok, cast to DirEntry
		//check dirBlkPtr is 0 => if 0, no more meaningful block is alloced
		if(root->dirBlkPtr[i] != 0) DevReadBlock(root->dirBlkPtr[i], blkPtr);
		else break; 
		DirEntry* dirPtr = (DirEntry*)blkPtr;
		
		//compare paresed name with DirEntry's name
		for(int j = 0; j < NUM_OF_DIRENT_PER_BLK; j++){
			if(m == NULL){
				printf("eee...dirPtr[%d].name = '%s'\n",j, dirPtr[j].name);
				//if file name is ""
				if(strcmp(dirPtr[j].name, "") == 0){
					//set retDirPtr, retDirPtrIndex, retBlkIndex
					*inoIndex = root->dirBlkPtr[i];
					copyDirEnt(retDirPtr, dirPtr);
					*retDirPtrIndex = j;
					*retBlkIndex = root->dirBlkPtr[i];
					printf("ccc...retDirPtrIndex = %d, retBlkIndex = %d\n", *retDirPtrIndex, *retBlkIndex);	
					//free
					free(root);
					free(blkPtr);
					
					return true;	
				}
			}
			else{
				//if file is already exist
				if(strcmp(dirPtr[j].name, m->array[depth]) == 0){
					//set retDirPtr, retDirPtrIndex
					*inoIndex = root->dirBlkPtr[i];
					copyDirEnt(retDirPtr, dirPtr);
					*retDirPtrIndex = j;
					
					//free
					free(root);
					free(blkPtr);
					
					return true;
				}
			}
		}
	}
	//check if exist is false & indir pointer in not 0
	if(root->indirBlkPointer != 0){
		//get indirect block, cast to indirect block
		DevReadBlock(root->indirBlkPointer, blkPtr);
		int* indirect = (int*)blkPtr;

		//check indirect block
		for(int i=0; i < BLOCK_SIZE/sizeof(int); i++){
			//read blcok, cast to DirEntry
			//check indirect[i] is 0 => if 0, no more block is alloced
			if(indirect[i] != 0) DevReadBlock(indirect[i], blkPtr);
			else break;
			DirEntry* dirPtr = (DirEntry*)blkPtr;
			
			//compare paresed name with DirEntry's name
			for(int j = 0; j < NUM_OF_DIRENT_PER_BLK; j++){
				if(m == NULL){
					//if file name is ""
					if(strcmp(dirPtr[j].name, "") == 0){
						//set retDirPtr, retDirPtrIndex, retBlkIndex
						*inoIndex = root->dirBlkPtr[i];
						copyDirEnt(retDirPtr, dirPtr);
						*retDirPtrIndex = j;
						*retBlkIndex = root->dirBlkPtr[i];
						
						//free
						free(root);
						free(blkPtr);
						
						return true;	
					}
				}
				else{
					//if file is already exist
					if(strcmp(dirPtr[j].name, m->array[depth]) == 0){
						//set retDirPtr, retDirPtrIndex
						*inoIndex = root->dirBlkPtr[i];
						copyDirEnt(retDirPtr, dirPtr);
						*retDirPtrIndex = j;
						
						//free
						free(root);
						free(blkPtr);
						
						return true;
					}
				}
			}
		}
	}
	//if not found, free
	free(root);
	free(blkPtr);
	return false;
}

bool myMakeDir(Matrix* m, int depth, DirEntry* pDirPtr, int pDirPtrIndex, int pInoIndex, int pBlkIndex){
	//get Free index
	int cFreeInoIndex = GetFreeInodeNum();
	int cFreeBlkIndex = GetFreeBlockNum();
	printf("myMakeDir()... cFreeInoIndex = %d, cFreeBlkIndex = %d, pBlkIndex = %d\n", cFreeInoIndex, cFreeBlkIndex, pBlkIndex);

	//set parent's DirEntry, write to disk
	strcpy(pDirPtr[pDirPtrIndex].name, m->array[depth]);
	pDirPtr[pDirPtrIndex].inodeNum = cFreeInoIndex; 
	DevWriteBlock(pBlkIndex, (char*)pDirPtr);
	printf("pDirPtr[0].name = '%s', [1].name = '%s', [2].name = '%s', [3].name = '%s'\n",pDirPtr[0].name , pDirPtr[1].name, pDirPtr[2].name, pDirPtr[3].name); 
	//alloc memory for newly created dir's block, cast to DirEntry
	char* cBlkPtr = (char*)malloc(BLOCK_SIZE);
	DirEntry* cDirPtr = (DirEntry*)cBlkPtr;
	
	//set DirEntry's data, write to disk
	strcpy(cDirPtr[0].name, ".");
	cDirPtr[0].inodeNum = cFreeInoIndex;
	strcpy(cDirPtr[1].name, "..");
	cDirPtr[1].inodeNum = pInoIndex;
	DevWriteBlock(cFreeBlkIndex, cBlkPtr);

	//get created newly created dir's inode, set data, write to disk
	Inode* inoPtr = (Inode*)malloc(sizeof(Inode));
	GetInode(cFreeInoIndex, inoPtr);
	inoPtr->size = 0;
	inoPtr->type = FILE_TYPE_DIR;
	inoPtr->dirBlkPtr[0] = cFreeBlkIndex;
	PutInode(cFreeInoIndex, inoPtr);
		
	//set bitmap
	SetInodeBitmap(cFreeInoIndex);
	SetBlockBitmap(cFreeBlkIndex);
		
	//get sys info, write to disk
	DevReadBlock(FILESYS_INFO_BLOCK, cBlkPtr);
	FileSysInfo* sysPtr = (FileSysInfo*)cBlkPtr;	
	sysPtr->numAllocBlocks++;
	sysPtr->numFreeBlocks--;
	sysPtr->numAllocInodes++;
	DevWriteBlock(FILESYS_INFO_BLOCK, cBlkPtr);

	//free
	free(cBlkPtr);
	free(inoPtr);
	return true;
}

int myMakeFile(Matrix* m, int depth, DirEntry* pDirPtr, int pDirPtrIndex, int pInoIndex, int pBlkIndex){
	//get free index for child
	int cFreeInoIndex = GetFreeInodeNum();
	int cFreeBlkIndex = GetFreeBlockNum();
	
	//set parent's DirEntry, write to disk
	strcpy(pDirPtr[pDirPtrIndex].name, m->array[depth]);
	pDirPtr[pDirPtrIndex].inodeNum = cFreeInoIndex; 
	DevWriteBlock(pBlkIndex, (char*)pDirPtr);
	
	//alloc memory for newly created file's block, cast to DirEntry
	char* cBlkPtr = (char*)malloc(BLOCK_SIZE);
	//dirEntry* cDirPtr = (DirEntry*)cBlkPtr;
	
	//get created newly created dir's inode, set data, write to disk
	Inode* inoPtr = (Inode*)malloc(sizeof(Inode));
	GetInode(cFreeInoIndex, inoPtr);
	inoPtr->size = 0;
	inoPtr->type = 	FILE_TYPE_FILE; 
	PutInode(cFreeInoIndex, inoPtr);
		
	//set bitmap
	SetInodeBitmap(cFreeInoIndex);
		
	//get sys info, write to disk
	DevReadBlock(FILESYS_INFO_BLOCK, cBlkPtr);
	FileSysInfo* sysPtr = (FileSysInfo*)cBlkPtr;	
	sysPtr->numAllocInodes++;
	DevWriteBlock(FILESYS_INFO_BLOCK, cBlkPtr);

	//set fd table
	//it need to alloc at program start point, init 0
	if(pFileDescTable == NULL) initFdTbl();
	FileDesc* fdPtr = (FileDesc*)pFileDescTable;
	int fFdIndex = 0;
	for(int i=0; i<MAX_FD_ENTRY_LEN; i++){
		//if find free fd index
		if(fdPtr[i].bUsed == 0){
			fFdIndex = i;
			fdPtr[i].bUsed = 1;
			fdPtr[i].fileOffset = 0;
			fdPtr[i].inodeNum = cFreeInoIndex;
			break; 
		}
	}

	//free
	free(cBlkPtr);
	free(inoPtr);
	
	return fFdIndex;
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
	switch(flag){
		case OPEN_FLAG_CREATE:{
			int* inoIndex = (int*)malloc(sizeof(int));
			*inoIndex = 0;
	
			//for save string
			Matrix m;
			initMatrix(&m, 2);
			//split path
			char* path = (char*)malloc(sizeof(szFileName));
			strcpy(path, szFileName);
			char* parsePtr = strtok(path, "/");
			//parse, save string
			while(parsePtr != NULL){
				insertMatrix(&m, parsePtr);
				parsePtr = strtok(NULL , "/");
			}
			int depth = 0;
			DirEntry* retDirPtr = (DirEntry*)malloc(BLOCK_SIZE);
			int* retDirPtrIndex = (int*)malloc(sizeof(int));
			*retDirPtrIndex = 0;
			int* retBlkIndex = (int*)malloc(sizeof(int));
			*retBlkIndex = 19;
	
			//find
			while(findName(inoIndex, &m, depth, retDirPtr, retDirPtrIndex, retBlkIndex)){
				*inoIndex = retDirPtr[*retDirPtrIndex].inodeNum;
				depth++;
			}
			//make dir
			if(m.array[depth] != NULL){
				findName(inoIndex, NULL, 0, retDirPtr, retDirPtrIndex, retBlkIndex);
				return myMakeFile(&m, depth, retDirPtr, *retDirPtrIndex, *inoIndex, *retBlkIndex);
			}
			else return -1;
		break;
		}
		case OPEN_FLAG_READWRITE:

		break;
	}
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




int		MakeDir(const char* szDirName)
{
	int* inoIndex = (int*)malloc(sizeof(int));
	*inoIndex = 0;
	
	//for save string
	Matrix m;
	initMatrix(&m, 2);
	//split path
	char* path = (char*)malloc(sizeof(szDirName));
	strcpy(path, szDirName);
	char* parsePtr = strtok(path, "/");
	//parse, save string
	while(parsePtr != NULL){
		insertMatrix(&m, parsePtr);
		parsePtr = strtok(NULL , "/");
	}
	int depth = 0;
	DirEntry* retDirPtr = (DirEntry*)malloc(BLOCK_SIZE);
	int* retDirPtrIndex = (int*)malloc(sizeof(int));
	*retDirPtrIndex = 0;
	int* retBlkIndex = (int*)malloc(sizeof(int));
	*retBlkIndex = 19;
	
	//find
	while(findName(inoIndex, &m, depth, retDirPtr, retDirPtrIndex, retBlkIndex)){
		*inoIndex = retDirPtr[*retDirPtrIndex].inodeNum;
		depth++;
	}
	//make dir
	if(m.array[depth] != NULL){
		findName(inoIndex, NULL, 0, retDirPtr, retDirPtrIndex, retBlkIndex);
		if(myMakeDir(&m, depth, retDirPtr, *retDirPtrIndex, *inoIndex, *retBlkIndex)) return 0;
		else return -1;
	}
	else return -1;
}


int		RemoveDir(const char* szDirName)
{

}


void		EnumerateDirStatus(const char* szDirName, DirEntry* pDirEntry, int* pNum)
{

}
