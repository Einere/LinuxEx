#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h> 
#include "disk.h"
#include "fs.h"
#include "Matrix.h"

#define SET true
#define RESET false

FileDescTable* pFileDescTable = NULL;

void copyDirEnt(DirEntry* target, DirEntry* source);
bool findName(int* inoIndex, Matrix* m, int depth, DirEntry* retDirPtr, int* retDirPtrIndex, int* retBlkIndex, bool* is_full);
bool myMakeDir(Matrix* m, int depth, DirEntry* pDirPtr, int pDirPtrIndex, int pInoIndex, int pBlkIndex, bool is_full);
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

void copyDirEnt(DirEntry* target, DirEntry* source){
	for(int i = 0; i < NUM_OF_DIRENT_PER_BLOCK; i++){
		strcpy(target[i].name, source[i].name);
		target[i].inodeNum = source[i].inodeNum;
	}
}

bool findName(int* inoIndex, Matrix* m, int depth, DirEntry* retDirPtr, int* retDirPtrIndex, int* retBlkIndex, bool* is_full){
	//stop condition
	if(m != NULL && m->array[depth] == NULL) return false;
	//if(m != NULL && (strcmp(m->array[depth], "") != 0) ) printf("\nsearching name : %s\n", m->array[depth]);
	*is_full = false;
	//get current inode
	Inode* root = (Inode*)malloc(sizeof(Inode));
	GetInode(*inoIndex, root);
	//printf("findName start...inoIndex = %d, retDirPtrIndex = %d, retBlkIndex = %d\n", *inoIndex, *retDirPtrIndex, *retBlkIndex);
	//alloc memory
	char* blkPtr = (char*)malloc(BLOCK_SIZE);
	char* blkPtr2 = (char*)malloc(BLOCK_SIZE);
	//check file name is already exist
	//at direct ptr	
	for(int i = 0; i < NUM_OF_DIRECT_BLOCK_PTR; i++){
		//read blcok, cast to DirEntry
		//check dirBlkPtr is 0 => if 0, no more meaningful block is alloced
		if(root->dirBlockPtr[i] != 0) DevReadBlock(root->dirBlockPtr[i], blkPtr);
		else {
			//printf("root->dirBlockPtr[%d] = %d\n", i, root->dirBlockPtr[i]);
			break; 
		}
		DirEntry* dirPtr = (DirEntry*)blkPtr;
		
		//compare paresed name with DirEntry's name
		for(int j = 0; j < NUM_OF_DIRENT_PER_BLOCK; j++){
			if(m == NULL){
				//printf("eee...[%d][%d].name = '%s'\n", i, j, dirPtr[j].name);
				//if file name is ""
				if(strcmp(dirPtr[j].name, "") == 0){
					//set retDirPtr, retDirPtrIndex, retBlkIndex
					copyDirEnt(retDirPtr, dirPtr);
					*retDirPtrIndex = j;
					*retBlkIndex = root->dirBlockPtr[i];
					*is_full = false;
					//printf("ccc...inoIndex = %d, retDirPtrIndex = %d, retBlkIndex = %d\n", *inoIndex, *retDirPtrIndex, *retBlkIndex);	
					//free
					free(root);
					free(blkPtr);
					
					return true;	
				}
				else if (j == NUM_OF_DIRENT_PER_BLOCK - 1) *is_full = true;
			}
			else{
				//printf("zzz...[%d][%d].name = %s\n",i , j, dirPtr[j].name);
				//if file is already exist
				if(strcmp(dirPtr[j].name, m->array[depth]) == 0){
					//set retDirPtr, retDirPtrIndex
					copyDirEnt(retDirPtr, dirPtr);
					*retDirPtrIndex = j;
					*retBlkIndex = root->dirBlockPtr[i];
					//printf("yyy... inoIndex = %d, root->dirBloPtr[0] = %d, [1] = %d, indirect = %d\n",*inoIndex, root->dirBlockPtr[0], root->dirBlockPtr[1], root->indirBlockPtr);

					//free
					free(root);
					free(blkPtr);
					
					return true;
				}
			}
		}
	}
	//check indir pointer in not 0
	if(root->indirBlockPtr != 0){
		//get indirect block, cast to indirect block
		DevReadBlock(root->indirBlockPtr, blkPtr);
		int* indirect = (int*)blkPtr;

		//check indirect block
		for(int i=0; i < BLOCK_SIZE/sizeof(int); i++){
			//printf("xxx...indirect[%d] = %d\n", i, indirect[i]);
			//read blcok, cast to DirEntry
			//check indirect[i] is 0 => if 0, no more block is alloced
			if(indirect[i] != 0) DevReadBlock(indirect[i], blkPtr2);
			else break;
			DirEntry* dirPtr = (DirEntry*)blkPtr2;
			
			//compare paresed name with DirEntry's name
			for(int j = 0; j < NUM_OF_DIRENT_PER_BLOCK; j++){
				//printf("www...[%d][%d].name = '%s'\n",i, j, dirPtr[j].name);
				if(m == NULL){
					//if file name is ""
					if(strcmp(dirPtr[j].name, "") == 0){
						//printf("vvv...indirect[%d], dirPtr[%d].name = '%s'\n",i, j, dirPtr[j].name);
						//set retDirPtr, retDirPtrIndex, retBlkIndex
						copyDirEnt(retDirPtr, dirPtr);
						*retDirPtrIndex = j;
						*retBlkIndex = indirect[i];
						*is_full = false;
						//free
						free(root);
						free(blkPtr);
						free(blkPtr2);
						return true;	
					}
					else if (j == NUM_OF_DIRENT_PER_BLOCK - 1) *is_full = true;
				}
				else{
					//printf("uuu\n");
					//if file is already exist
					if(strcmp(dirPtr[j].name, m->array[depth]) == 0){
						//set retDirPtr, retDirPtrIndex
						copyDirEnt(retDirPtr, dirPtr);
						*retDirPtrIndex = j;
						*retBlkIndex = indirect[i];	
						//free
						free(root);
						free(blkPtr);
						free(blkPtr2);
						return true;
					}
				}
			}
		}
	}
	//printf("findName end...\n");
	//if not found, free
	free(root);
	free(blkPtr);
	free(blkPtr2);
	return false;
}

bool myMakeDir(Matrix* m, int depth, DirEntry* pDirPtr, int pDirPtrIndex, int pInoIndex, int pBlkIndex, bool is_full){
	//if pDirPtr is full
	if(is_full){
		//get parent inoPtr
		Inode* pInoPtr = (Inode*)malloc(sizeof(Inode));
		GetInode(pInoIndex, pInoPtr);

		if(pInoPtr->dirBlockPtr[1] == 0){
			//init pDirPtr, pDirPtrIndex
			memset(pDirPtr, 0, BLOCK_SIZE);
			pDirPtrIndex = 0;
			//link dirPtr with parent inoPtr, write to disk, modify pBlkIndex
			int pFreeBlkIndex = GetFreeBlockNum();
			pInoPtr->dirBlockPtr[1] = pFreeBlkIndex;
			PutInode(pInoIndex, pInoPtr);
			pBlkIndex = pFreeBlkIndex;
			//set bitmap
			SetBlockBitmap(pFreeBlkIndex);
			//update file sys info
			DevReadBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
			pFileSysInfo->numAllocBlocks++;
			pFileSysInfo->numFreeBlocks--;
			DevWriteBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo); 
			//printf("myMakeDir()... if full...pInoIndex = %d, pFreeBlkIndex = %d...\n", pInoIndex, pFreeBlkIndex);
		}
		else{
			if(pInoPtr->indirBlockPtr == 0){
				//init pDirPtr, pDirPtrIndex
				memset(pDirPtr, 0, BLOCK_SIZE);
				pDirPtrIndex = 0;
				//link indirPtr with parent inoPtr
				int pFreeBlkIndex = GetFreeBlockNum();
				pInoPtr->indirBlockPtr = pFreeBlkIndex;
				PutInode(pInoIndex, pInoPtr);
				//set bitmap
				SetBlockBitmap(pFreeBlkIndex);
				//update file sys info for indirPtr
				DevReadBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
				pFileSysInfo->numAllocBlocks++;
				pFileSysInfo->numFreeBlocks--;
				DevWriteBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
				//printf("myMakeDir()... if full...pInoIndex = %d, pInoPtr->indirBlockPtr = %d, pFreeBlkIndex = %d...\n", pInoIndex, pInoPtr->indirBlockPtr ,pFreeBlkIndex);

				//get & set indirPtr, write to disk
				char* pIndirBlkPtr = (char*)malloc(BLOCK_SIZE);
				memset(pIndirBlkPtr, 0 ,BLOCK_SIZE);
				int* pIndirPtr = (int*)pIndirBlkPtr;
				int pFreeBlkIndex2 = GetFreeBlockNum();
				pIndirPtr[0] = pFreeBlkIndex2;
				DevWriteBlock(pFreeBlkIndex, (char*)pIndirPtr);
				//set bitmap
				SetBlockBitmap(pFreeBlkIndex2);
				//update file sys info for indirPtr
				DevReadBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
				pFileSysInfo->numAllocBlocks++;
				pFileSysInfo->numFreeBlocks--;
				DevWriteBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
				//printf("myMakeDir()... if full... pInoIndex = %d, pFreeBlkIndex2 = %d, pIndirPtr[0] = %d...\n", pInoIndex, pFreeBlkIndex2, pIndirPtr[0]);
				
				//modify pBlkIndex
				pBlkIndex = pFreeBlkIndex2;
				//free
				free(pIndirBlkPtr);
			}
			else{
				//get indirPtr, link dirPtr with parent inoPtr, write to disk, modify pBlkIndex
				char* tmpBlk = (char*)malloc(BLOCK_SIZE);
				DevReadBlock(pInoPtr->indirBlockPtr, tmpBlk);
				int* indirPtr = (int*)tmpBlk;
				
				for(int i = 0; i < BLOCK_SIZE/sizeof(int); i++){
					if(indirPtr[i] == 0){
						//init pDirPtr, pDirPtrIndex
						memset(pDirPtr, 0, BLOCK_SIZE);
						pDirPtrIndex = 0;
						int pFreeBlkIndex = GetFreeBlockNum();
						indirPtr[i] = pFreeBlkIndex;
						DevWriteBlock(pInoPtr->indirBlockPtr, (char*)indirPtr);
						pBlkIndex = pFreeBlkIndex;
						//set bitmap
						SetBlockBitmap(pFreeBlkIndex);
						//update file sys info
						DevReadBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
						pFileSysInfo->numAllocBlocks++;
						pFileSysInfo->numFreeBlocks--;
						DevWriteBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo); 
						//printf("myMakeDir()... if full & indirect != 0... indirPtr[%d] = %d, pBlkIndex = %d\n", i, indirPtr[i], pBlkIndex);
					}
				}
				free(tmpBlk);
			}
		}
		//free
		free(pInoPtr);
	}

	//get free inode index, block index
	int cFreeInoIndex = GetFreeInodeNum();
	int cFreeBlkIndex = GetFreeBlockNum();
	//printf("cFreeInoIndex = %d, cFreeBlkIndex = %d...\n", cFreeInoIndex, cFreeBlkIndex);
	
	
	//set parent dirPtr, write to disk
	strcpy(pDirPtr[pDirPtrIndex].name, m->array[depth]);
	pDirPtr[pDirPtrIndex].inodeNum = cFreeInoIndex;
	DevWriteBlock(pBlkIndex, (char*)pDirPtr);
	//printf("myMakeDir()... pBlkIndex = %d, pDirPtr[%d].name = %s, [].inodeNum = %d...\n", pBlkIndex, pDirPtrIndex, pDirPtr[pDirPtrIndex].name, pDirPtr[pDirPtrIndex].inodeNum);
	//make child dirBlk, write to disk
	char* cBlkPtr = (char*)malloc(BLOCK_SIZE);
	memset(cBlkPtr, 0, BLOCK_SIZE);
	DirEntry* cDirPtr = (DirEntry*)cBlkPtr;
	strcpy(cDirPtr[0].name, ".");
	cDirPtr[0].inodeNum = cFreeInoIndex;
	strcpy(cDirPtr[1].name, "..");
	cDirPtr[1].inodeNum = pInoIndex;
	DevWriteBlock(cFreeBlkIndex, (char*)cDirPtr);
	//printf("cDirPtr[0].inodeNum = %d, cDirPtr[1].inodeNum = %d...\n", cDirPtr[0].inodeNum, cDirPtr[1].inodeNum);
	//printf("cDirPtr[0].name = '%s', [1].name = '%s', [2].name = '%s', [3].name = '%s'...\n", cDirPtr[0].name, cDirPtr[1].name, cDirPtr[2].name, cDirPtr[3].name);
	//set child inoPtr, write to disk
	Inode* cInoPtr = (Inode*)malloc(sizeof(Inode));
	memset(cInoPtr, 0, sizeof(Inode));
	GetInode(cFreeInoIndex, cInoPtr);
	cInoPtr->type = FILE_TYPE_DIR;
	cInoPtr->size = 0;
	cInoPtr->dirBlockPtr[0] = cFreeBlkIndex;
	cInoPtr->dirBlockPtr[1] = 0;
	cInoPtr->indirBlockPtr = 0;
	PutInode(cFreeInoIndex, cInoPtr);
	//printf("cInoPtr->dirBlockPtr[0] = %d, [1] = %d, indirec = %d...\n", cInoPtr->dirBlockPtr[0], cInoPtr->dirBlockPtr[1], cInoPtr->indirBlockPtr);

	//set bitmap
	SetInodeBitmap(cFreeInoIndex);
	SetBlockBitmap(cFreeBlkIndex);

	//get & update file sys info, write to disk
	DevReadBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
	pFileSysInfo->numAllocInodes++;
	pFileSysInfo->numAllocBlocks++;
	pFileSysInfo->numFreeBlocks--;
	DevWriteBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);

	//free  malloc
	free(cBlkPtr);
	free(cInoPtr);
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

bool myRemoveFile(Matrix* m, int depth, DirEntry* pDirPtr, int pDirPtrIndex, int pInoIndex, int pBlkIndex){
	//get parent inode, child inode
	Inode* pInoPtr = (Inode*)malloc(sizeof(Inode));
	GetInode(pInoIndex, pInoPtr);
	Inode* cInoPtr = (Inode*)malloc(sizeof(Inode));
	int cInoIndex = pDirPtr[pDirPtrIndex].inodeNum;
	GetInode(cInoIndex, cInoPtr);

	//alloc tmp block for reset
	char* tmpBlk = (char*)malloc(BLOCK_SIZE);
	memset(tmpBlk, 0, BLOCK_SIZE);
	char* tmpBlk2 = (char*)malloc(BLOCK_SIZE);

	//get fs info
	if(pFileSysInfo == NULL) pFileSysInfo = (FileSysInfo*)malloc(BLOCK_SIZE);
	DevReadBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);

	//remove child file data
	//at direct
	for(int i = 0; i < NUM_OF_DIRECT_BLOCK_PTR; i++){
		if(cInoPtr->dirBlockPtr[i] != 0){
			//reset data block
			DevWriteBlock(cInoPtr->dirBlockPtr[i], tmpBlk);
			ResetBlockBitmap(cInoPtr->dirBlockPtr[i]);
			pFileSysInfo->numAllocBlocks--;
			pFileSysInfo->numFreeBlocks++;
		}
	}
	//at indirect
	if(cInoPtr->indirBlockPtr != 0){
		//get child inode's indirect ptr arr
		DevReadBlock(cInoPtr->indirBlockPtr, tmpBlk2);
		int* indirPtr = (int*)tmpBlk2;

		for(int i = 0; i < BLOCK_SIZE/sizeof(int); i++){
			if(indirPtr[i] != 0){
				//reset data block
				DevWriteBlock(indirPtr[i], tmpBlk);
				ResetBlockBitmap(indirPtr[i]);
				pFileSysInfo->numAllocBlocks--;
				pFileSysInfo->numFreeBlocks++;
			}
		}
	}
	//reset child inode
	memset(cInoPtr, 0, sizeof(Inode));
	PutInode(cInoIndex, cInoPtr);
	ResetInodeBitmap(cInoIndex);

	//Modify parent dirEntry, write to disk
	strcpy(pDirPtr[pDirPtrIndex].name, "");
	pDirPtr[pDirPtrIndex].inodeNum = 0;
	DevWriteBlock(pBlkIndex, (char*)pDirPtr);

	//check if emtpy dirEntryPtr
	bool empty = true;
	for(int i = 0; i < NUM_OF_DIRENT_PER_BLOCK; i++){
		if(strcmp(pDirPtr[i].name, "") != 0) empty = false;
	}
	//optimize dirEntry
	if(!empty){
		for(int i = NUM_OF_DIRENT_PER_BLOCK - 1; i >= 0; i--){
			if(strcmp(pDirPtr[i].name, "") != 0){
				strcpy(pDirPtr[pDirPtrIndex].name, pDirPtr[i].name);
				pDirPtr[pDirPtrIndex].inodeNum = pDirPtr[i].inodeNum;
				strcmp(pDirPtr[i].name, "");
				pDirPtr[i].inodeNum = 0;
				DevWriteBlock(pBlkIndex, (char*)pDirPtr);
				break;
			}
		}
	}
	//if empty dirEntryPtr, unlink with dirPtr
	if(empty){
		//at direct
		if(pInoPtr->dirBlockPtr[1] == pBlkIndex){
			//unlink, reset data block
			pInoPtr->dirBlockPtr[1] = 0;
		}
		
		//at indirect
		if(pInoPtr->indirBlockPtr != 0){
			DevReadBlock(pInoPtr->indirBlockPtr, tmpBlk2);
			int* indirPtr = (int*)tmpBlk2;
			int indirPtrIndex = 0;

			for(int i=0; i < BLOCK_SIZE/sizeof(int); i++){
				if(indirPtr[i] == pBlkIndex){
					indirPtr[i] = 0;
					indirPtrIndex = i;
				}
				if(indirPtr[i] != 0) empty = false; 
			}
			//optimize indirPtr
			if(!empty){
				for(int i = BLOCK_SIZE/sizeof(int) -1; i>=0; i--){
					if(indirPtr[i] != 0){
						indirPtr[indirPtrIndex] = i;
						indirPtr[i] = 0;
						DevWriteBlock(pInoPtr->indirBlockPtr, tmpBlk2);
					}
				}
			}
			//if all indirPtr is empty
			if(empty){
				//unlink, reset block data
				pInoPtr->indirBlockPtr = 0;
				DevWriteBlock(pInoPtr->indirBlockPtr, tmpBlk);
				pFileSysInfo->numAllocBlocks--;
				pFileSysInfo->numFreeBlocks++;
			}
		}
	}

	//update fsInfo
	DevWriteBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);

	//free
	free(pInoPtr);
	free(cInoPtr);
	free(tmpBlk);
	free(tmpBlk2);

	return true;
}

bool myRemoveDir(Matrix* m, int depth, DirEntry* pDirPtr, int pDirPtrIndex, int pInoIndex, int pBlkIndex){
	//get parent inode, child inode
	Inode* pInoPtr = (Inode*)malloc(sizeof(Inode));
	GetInode(pInoIndex, pInoPtr);
	Inode* cInoPtr = (Inode*)malloc(sizeof(Inode));
	int cInoIndex = pDirPtr[pDirPtrIndex].inodeNum;
	GetInode(cInoIndex, cInoPtr);
	//printf("pInoIndex = %d, pBlkIndex = %d, pDirPtr[%d].name = %s, [].inodeNum = %d, cInoIndex = %d...\n", pInoIndex, pBlkIndex, pDirPtrIndex, pDirPtr[pDirPtrIndex].name, pDirPtr[pDirPtrIndex].inodeNum ,cInoIndex);
	//get tmpBlk
	char* tmpBlk = (char*)malloc(BLOCK_SIZE);
	char* tmpBlk2 = (char*)malloc(BLOCK_SIZE);
	
	bool empty = true;
	//check if child dir is empty at direct
	for(int i = 0; i < NUM_OF_DIRECT_BLOCK_PTR; i++){
		//get dirBlk, cast to DirEntry*
		if(cInoPtr->dirBlockPtr[i] != 0) DevReadBlock(cInoPtr->dirBlockPtr[i], tmpBlk);
		else continue;
		DirEntry* dirPtr = (DirEntry*)tmpBlk;
		
		for(int j = 0; j < NUM_OF_DIRENT_PER_BLOCK; j++){
			//if found corresponding dir name
			if(strcmp(dirPtr[j].name, ".") == 0 || strcmp(dirPtr[j].name, "..") == 0){
				//printf("dirPtr[%d].name = '%s'\n", j, dirPtr[j].name);
				continue; 
			} 
			if(strcmp(dirPtr[j].name, "") != 0){
				empty = false; 
				//printf("dirPtr[%d].name = '%s'\n", j, dirPtr[j].name);
			}	
		}
	}
	//check if child dir is empty at indirect
	if(cInoPtr->indirBlockPtr != 0 && empty){
		//get indirPtr, cast to int*
		DevReadBlock(cInoPtr->indirBlockPtr, tmpBlk);
		int* indirPtr = (int*)tmpBlk;

		for(int k = 0; k < BLOCK_SIZE/sizeof(int); k++){
			//if not empty dir block
			if(indirPtr[k] != 0){
				//get block, cast to DirEntry*
				DevReadBlock(indirPtr[k], tmpBlk2);
				DirEntry* dirPtr = (DirEntry*)tmpBlk2;

				for(int l = 0; l < NUM_OF_DIRENT_PER_BLOCK; l++){
					if(strcmp(dirPtr[l].name, "") != 0) empty = false;
				}
			}
			//if empty dir block
			else continue;
		}
	}
	//if empty dir
	if(empty){
		//reset dirEntry block
		memset(tmpBlk, 0, BLOCK_SIZE);
		DevWriteBlock(cInoPtr->dirBlockPtr[0], tmpBlk);
		
		//reset child inode
		memset(cInoPtr, 0 , sizeof(Inode));
		PutInode(pDirPtr[pDirPtrIndex].inodeNum, cInoPtr);
			
		//reset bitmap
		ResetBlockBitmap(cInoPtr->dirBlockPtr[0]);
		ResetInodeBitmap(pDirPtr[pDirPtrIndex].inodeNum);

		//update fs info
		DevReadBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
		pFileSysInfo->numAllocBlocks--;
		pFileSysInfo->numFreeBlocks++;
		pFileSysInfo->numAllocInodes--;
		
		//Modify parent dirEntry, write to disk
		strcpy(pDirPtr[pDirPtrIndex].name, "");
		pDirPtr[pDirPtrIndex].inodeNum = 0;
		DevWriteBlock(pBlkIndex, (char*)pDirPtr);

		empty = true;
		int moveIndex = 0;
		//modify parent inode
		//check if emtpy dirEntryPtr
		bool optimize = false;
		for(int i = 0; i < NUM_OF_DIRENT_PER_BLOCK; i++){
			if(i != pDirPtrIndex &&strcmp(pDirPtr[i].name, "") != 0){
				empty = false;
				if(i > pDirPtrIndex){
					optimize = true;
					moveIndex = i;
				}
			}
		}
		//optimize dirEntry
		if(optimize){
			strcpy(pDirPtr[pDirPtrIndex].name, pDirPtr[moveIndex].name);
			pDirPtr[pDirPtrIndex].inodeNum = pDirPtr[moveIndex].inodeNum;
			strcmp(pDirPtr[moveIndex].name, "");
			pDirPtr[moveIndex].inodeNum = 0;
			DevWriteBlock(pBlkIndex, (char*)pDirPtr);
		}
		//if empty dirEntry
		if(empty){
			//at direct
			if(pInoPtr->dirBlockPtr[1] == pBlkIndex){
				//unlink, reset data block
				pInoPtr->dirBlockPtr[1] = 0;
				memset(tmpBlk, 0, BLOCK_SIZE);
				DevWriteBlock(pBlkIndex, tmpBlk);
				//reset bitmap, update fs info
				ResetBlockBitmap(pBlkIndex);
				DevReadBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
				pFileSysInfo->numAllocBlocks--;
				pFileSysInfo->numFreeBlocks++;
				DevWriteBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
			}
			//at indirect
			else if(pInoPtr->indirBlockPtr != 0){
				//get indir block, cast to int*
				DevReadBlock(pInoPtr->indirBlockPtr, tmpBlk);
				int* indirPtr = (int*)tmpBlk;
				int moveToIndex = 0;
				int moveFromIndex = 0;
				empty = true;
				optimize = false;
				for(int i=0; i < BLOCK_SIZE/sizeof(int); i++){
					//if found empty block at indirect, reset data block
					if(indirPtr[i] == pBlkIndex){
						moveToIndex = i;
						//unlink, reset data block
						indirPtr[i] = 0;
						memset(tmpBlk2, 0, BLOCK_SIZE);
						DevWriteBlock(pBlkIndex, tmpBlk2); 
						//reset bitmap, update fs info
						ResetBlockBitmap(pBlkIndex);
						DevReadBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
						pFileSysInfo->numAllocBlocks--;
						pFileSysInfo->numFreeBlocks++;
						DevWriteBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
					}
					if(indirPtr[i] != 0){
						empty = false;
						if(i > moveToIndex){
							optimize = true;
							moveFromIndex = i;
						} 
					} 
				}
				//optimize indirPtr
				if(optimize){
					indirPtr[moveToIndex] = indirPtr[moveFromIndex];
					indirPtr[moveFromIndex] = 0;
					DevWriteBlock(pInoPtr->indirBlockPtr, (char*)indirPtr);
				}
				//if all indirPtr is empty
				if(empty){
					//reset bitmap, update fs info
					ResetBlockBitmap(pInoPtr->indirBlockPtr);
					DevReadBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
					pFileSysInfo->numAllocBlocks--;
					pFileSysInfo->numFreeBlocks++;
					DevWriteBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
					//unlink, reset block data
					memset(tmpBlk2, 0, BLOCK_SIZE);
					DevWriteBlock(pInoPtr->indirBlockPtr, tmpBlk2);
					pInoPtr->indirBlockPtr = 0;
				}
			}
		}

	//update fsInfo
	DevWriteBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
	}
	else{
		//free
		free(pInoPtr);
		free(cInoPtr);
		free(tmpBlk);
		free(tmpBlk2);
		//if not empty dir

		return false;
	}
	return true;
}

void FileSysInit(void){
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

	//init fd table
	pFileDescTable = (FileDescTable*)malloc(sizeof(FileDescTable));
	memset(pFileDescTable, 0, sizeof(FileDescTable));

	//load file sys info
	pFileSysInfo = (FileSysInfo*)malloc(BLOCK_SIZE);
	DevReadBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
}

void SetInodeBitmap(int inodeno){
	read_do_bit(INODE_BITMAP_BLOCK_NUM, inodeno, SET);
}

void ResetInodeBitmap(int inodeno){
	read_do_bit(INODE_BITMAP_BLOCK_NUM, inodeno, RESET);
}

void SetBlockBitmap(int blkno){
	read_do_bit(BLOCK_BITMAP_BLOCK_NUM, blkno, SET);
}

void ResetBlockBitmap(int blkno){
	read_do_bit(BLOCK_BITMAP_BLOCK_NUM, blkno, RESET);
}

void PutInode(int inodeno, Inode* pInode){
	//check inodeno is in 0~63
	if(inodeno >= INODELIST_BLOCKS * (BLOCK_SIZE / sizeof(Inode))) return;
	
	//alloc temp block and read
	char* pBlock = (char*)malloc(BLOCK_SIZE);
	int block_index = (inodeno / NUM_OF_INODE_PER_BLOCK) + INODELIST_BLOCK_FIRST; 
	DevReadBlock(block_index, pBlock);
	
	//copy from pInode to inode list, write and free
	memcpy(pBlock + (inodeno % NUM_OF_INODE_PER_BLOCK) * sizeof(Inode), pInode, sizeof(Inode));
	DevWriteBlock(block_index, pBlock);	
	free(pBlock);
}

void GetInode(int inodeno, Inode* pInode){
	//alloc temp block and read
	char* pBlock = (char*)malloc(BLOCK_SIZE);
	int block_index = (inodeno / NUM_OF_INODE_PER_BLOCK) + INODELIST_BLOCK_FIRST; 
	DevReadBlock(block_index, pBlock);
	
	//copy from inode list to pInode and free
	memcpy(pInode, pBlock + (inodeno % NUM_OF_INODE_PER_BLOCK) * sizeof(Inode), sizeof(Inode));
	free(pBlock);
}

int GetFreeInodeNum(void){
	//alloc temp block and read 
	char* pBlock = (char*)malloc(BLOCK_SIZE);
	DevReadBlock(INODE_BITMAP_BLOCK_NUM, pBlock);

	//find first zero bit and return bit index
	for(int i = 0; i < NUM_OF_INODE_PER_BLOCK * INODELIST_BLOCKS / 8; i++){
		//if pBlock[i] == 11111111, continue
		if(pBlock[i] == -1) continue;

		for(int j = 7; j >= 0; j--){
			if(!(pBlock[i] >> j & 1)){
				free(pBlock);
				return (i * 8) + (7 - j);
			}
		}
	}
}

int GetFreeBlockNum(void){
	//alloc temp block and read 
	char* pBlock = (char*)malloc(BLOCK_SIZE);
	DevReadBlock(BLOCK_BITMAP_BLOCK_NUM, pBlock);
	
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
			if(!(pBlock[i] >> j & 1)){
				free(pBlock);
				return (i * 8) + (7 - j);
			}
		}
	}
}

int	OpenFile(const char* szFileName, OpenFlag flag){
	int inoIndex = 0;
	
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
	free(path);

	int depth = 0;
	DirEntry* retDirPtr = (DirEntry*)malloc(BLOCK_SIZE);
	int retDirPtrIndex = 0;
	int retBlkIndex = 19;
	bool is_full = false;
	//find
	while(findName(&inoIndex, &m, depth, retDirPtr, &retDirPtrIndex, &retBlkIndex, &is_full)){
		//if found file name to make
		if(m.used == depth + 1){
			//get fd table
			FileDesc* fdPtr = (FileDesc*)pFileDescTable;
			for(int i = 0; i < MAX_FD_ENTRY_LEN; i++){
				//if found corresponding entry
				if(fdPtr[i].inodeNum == inoIndex) return i;
			}
		}
		inoIndex = retDirPtr[retDirPtrIndex].inodeNum;
		depth++;
	}
	//if flag is create & not found, make file
	if(flag == OPEN_FLAG_CREATE && m.array[depth] != NULL){
		findName(&inoIndex, NULL, 0, retDirPtr, &retDirPtrIndex, &retBlkIndex, &is_full);
		return myMakeFile(&m, depth, retDirPtr, retDirPtrIndex, inoIndex, retBlkIndex);
	}
	else return -1;
}

int	WriteFile(int fileDesc, char* pBuffer, int length){
	//get inode
	FileDesc* fdTblPtr = (FileDesc*)pFileDescTable;
	int inoIndex = fdTblPtr[fileDesc].inodeNum;
	Inode* inoPtr = (Inode*)malloc(sizeof(Inode));
	GetInode(inoIndex, inoPtr);

	int count = 0;
	int preCount = 0;
	char* tmpBlk = (char*)malloc(BLOCK_SIZE);
	int nWrite = 0;
	int written = 0;
	int* indirPtrArr = (int*)malloc(BLOCK_SIZE);
	//append
	if(fdTblPtr[fileDesc].fileOffset + length <= (NUM_OF_DIRECT_BLOCK_PTR + BLOCK_SIZE/sizeof(int)) * BLOCK_SIZE){
	    int occupy = fdTblPtr[fileDesc].fileOffset % BLOCK_SIZE;
	    int avail = BLOCK_SIZE - occupy;
	    int lastIndex = floor(fdTblPtr[fileDesc].fileOffset / BLOCK_SIZE);
	    int appendIndex = 0;
	    //if remain data to write
	    while(written < length){
	        //at direct
	        if(lastIndex < NUM_OF_DIRECT_BLOCK_PTR){
	            //get block index to write
	            if(inoPtr->dirBlockPtr[lastIndex] != 0) appendIndex = inoPtr->dirBlockPtr[lastIndex];
	            else appendIndex = GetFreeBlockNum();
	            
	            //if use already exist block
	            if(written == 0){
	                //set block, write to disk
	                DevReadBlock(appendIndex, tmpBlk);
					strncpy(tmpBlk+occupy, pBuffer, avail);
	                nWrite = avail;
	                DevWriteBlock(appendIndex, tmpBlk);
					printf("writeFile) nWrite = %d, avail = %d\n", nWrite, avail); 
	            }
	            //if use newly alloced block
	            else{
	                //set block, write to disk
					strncpy(tmpBlk, pBuffer, BLOCK_SIZE);
	                nWrite = BLOCK_SIZE;
	                DevWriteBlock(appendIndex, tmpBlk);
	                //modify dirPtr, write to disk
	                inoPtr->dirBlockPtr[lastIndex] = appendIndex;
	                PutInode(inoIndex, inoPtr);
	                //set bitmap
	                SetBlockBitmap(appendIndex);
	                //increase count
	                count++;
					printf("writeFile) nWrite = %d\n", nWrite); 
	            }
	            //modify offset
	            fdTblPtr[fileDesc].fileOffset += nWrite;
	            //modify lastIndex, written
	            lastIndex++;
	            written += nWrite;
	        }
	        //at indirect
	        else{
	            //get block index to write
	            DevReadBlock(inoPtr->indirBlockPtr, (char*)indirPtrArr);
	            
	            if(indirPtrArr[lastIndex-2] != 0) appendIndex = indirPtrArr[lastIndex-2];
	            else appendIndex = GetFreeBlockNum();
	
	            //if use already exist block
	            if(written == 0){
	                //set block, write to disk
	                DevReadBlock(appendIndex, tmpBlk);
					strncpy(tmpBlk+occupy, pBuffer, avail);
	                nWrite = avail;
	                DevWriteBlock(appendIndex, tmpBlk);
	            }
	            //if use newly alloced block
	            else{
	                //set block, write to disk
					strncpy(tmpBlk, pBuffer, BLOCK_SIZE);
	                nWrite = BLOCK_SIZE;
	                DevWriteBlock(appendIndex, tmpBlk);
	                //modify indirPtr, write to disk
	                indirPtrArr[lastIndex-2] = appendIndex;
	                DevWriteBlock(inoPtr->indirBlockPtr, (char*)indirPtrArr);
	                //set bitmap
	                SetBlockBitmap(appendIndex);
	                //increase count
	                count++;
	            }
	            //modify offset
	            fdTblPtr[fileDesc].fileOffset += nWrite;
	            //modify lastIndex, written
	            lastIndex++;
	            written += nWrite;
	        }
	    }
	}
	//over-write
	else{
	    count = 0, preCount = ceil(fdTblPtr[fileDesc].fileOffset / BLOCK_SIZE);
	    fdTblPtr[fileDesc].fileOffset = 0;
	    
	    int overIndex = 0;
	    //copy data block
	    //at direct ptr
	    for(int i = 0; i < NUM_OF_DIRECT_BLOCK_PTR; i++){
	        //get block index to write
	        if(inoPtr->dirBlockPtr[i] != 0) overIndex = inoPtr->dirBlockPtr[i];
	        else overIndex = GetFreeBlockNum();
	        
	        //if remain data to write
	        if((BLOCK_SIZE * count) < length){
	            //copy data, write to disk
				strncpy(tmpBlk, pBuffer+BLOCK_SIZE*count, BLOCK_SIZE);
	            nWrite = BLOCK_SIZE;
	            DevWriteBlock(overIndex, tmpBlk);
	            //if alloc new block
	            if(count >= preCount){
	                //modify dirBlkPtr, write to disk
	                inoPtr->dirBlockPtr[i] = overIndex;
	                PutInode(inoIndex, inoPtr);
	                //set bitmap
	                SetBlockBitmap(overIndex);
	            }
	            //modify offset
	            fdTblPtr[fileDesc].fileOffset += nWrite;
	            //increase count
	            count++;
	        }
	        //if no data to write, write black block
	        else{
	            memset(tmpBlk, 0, BLOCK_SIZE);
	            DevWriteBlock(overIndex, tmpBlk);
	            //reset dirBlkPtr
	            inoPtr->dirBlockPtr[i] = 0;
	            PutInode(inoIndex, inoPtr);
	            //reset bitmap
	            ResetBlockBitmap(overIndex);
	        }
			written = nWrite;
	    }
	    //at indirect ptr
	    for(int i = 0; i < BLOCK_SIZE / sizeof(int); i++){
	        //get block index to write
	        DevReadBlock(inoPtr->indirBlockPtr, (char*)indirPtrArr);
	
	        //if remain data to write
	        if((BLOCK_SIZE * count) < length){
	            //copy data, write to disk
				strncpy(tmpBlk, pBuffer + BLOCK_SIZE * count, BLOCK_SIZE);
	            nWrite = BLOCK_SIZE;
	            DevWriteBlock(indirPtrArr[i], tmpBlk);
	            
	            //if alloc new block
	            if(count >= preCount){
	                //modify indirPtr, write to disk
	                indirPtrArr[i] = GetFreeBlockNum();
	                DevWriteBlock(indirPtrArr[i], (char*)indirPtrArr);
	                //set bitmap
	                SetBlockBitmap(indirPtrArr[i]);
	            }
	            //modify offset
	            fdTblPtr[fileDesc].fileOffset += nWrite;
	            //increase count
	            count++;
	        }
	        //if no data to write, write black block
	        else{
	            memset(tmpBlk, 0, BLOCK_SIZE);
	            DevWriteBlock(indirPtrArr[i], tmpBlk);
	            //reset indirPtrArr[i]
	            indirPtrArr[i] = 0;
	            //reset bitmap
	            ResetBlockBitmap(indirPtrArr[i]);
	        }
			written = nWrite;
	    }
	}
	//update inode
	PutInode(inoIndex, inoPtr);
	
	//update file sys info
	DevReadBlock(FILESYS_INFO_BLOCK, tmpBlk);
	FileSysInfo* fsInfoPtr = (FileSysInfo*)tmpBlk;
	fsInfoPtr->numAllocBlocks -= (preCount - count);
	fsInfoPtr->numFreeBlocks = (BLOCK_SIZE*8) - fsInfoPtr->numAllocBlocks;
	DevWriteBlock(0, (char*)fsInfoPtr);

	//free
	free(inoPtr);
	free(tmpBlk);
	free(indirPtrArr);

	return written; 
}

int	ReadFile(int fileDesc, char* pBuffer, int length){
	//get inode
	FileDesc* fdTblPtr = (FileDesc*)pFileDescTable;
	int inoIndex = fdTblPtr[fileDesc].inodeNum;
	Inode* inoPtr = (Inode*)malloc(sizeof(Inode));
	GetInode(inoIndex, inoPtr);
	
	char* tmpBlk = (char*)malloc(BLOCK_SIZE);
	int nRead = 0;
	int read = 0;
	int* indirPtrArr = (int*)malloc(BLOCK_SIZE);
	    
	int occupy = fdTblPtr[fileDesc].fileOffset % BLOCK_SIZE;
	int avail = BLOCK_SIZE - occupy;
	int lastIndex = floor(fdTblPtr[fileDesc].fileOffset / BLOCK_SIZE);
	int readIndex = 0;
	//if remain data to read
	while(read < length){
	    //at direct
	    if(lastIndex < NUM_OF_DIRECT_BLOCK_PTR){
	        //get block index to write
	        readIndex = inoPtr->dirBlockPtr[lastIndex];
	            
	        //if first read
	        if(read == 0){
	            //set block, read to buffer
	            DevReadBlock(readIndex, tmpBlk);
				strncpy(pBuffer, tmpBlk+occupy, avail);
	            nRead = avail;
	        }
	        //if not first read
	        else{
	       		//set block, read to buffer
	            DevReadBlock(readIndex, tmpBlk);
				strncpy(pBuffer+read, tmpBlk, BLOCK_SIZE);
	            nRead = BLOCK_SIZE;
			}
	        //modify offset
	        fdTblPtr[fileDesc].fileOffset += nRead;
	        //modify lastIndex, read
	        lastIndex++;
	        read += nRead;
	    }
        //at indirect
        else{
            //get block index to read
            DevReadBlock(inoPtr->indirBlockPtr, (char*)indirPtrArr);
            readIndex = indirPtrArr[lastIndex-2];

            //if first read
            if(read == 0){
                //set block, read to disk
                DevReadBlock(readIndex, tmpBlk);
				strncpy(pBuffer, tmpBlk+occupy, avail);
                nRead = avail;
            }
            //if not first read
            else{
				//set block, read to buffer
	            DevReadBlock(readIndex, tmpBlk);
				strncpy(pBuffer+read, tmpBlk, BLOCK_SIZE);
	            nRead = BLOCK_SIZE;
            }
            //modify offset
            fdTblPtr[fileDesc].fileOffset += nRead;
            //modify lastIndex, written
            lastIndex++;
            read += nRead;
        }
    }
	//free
	free(inoPtr);
	free(tmpBlk);
	//free(indirPtrArr);
	return read;
}

int	CloseFile(int fileDesc){
	FileDesc* fdPtr = (FileDesc*)pFileDescTable;
	
	if(fdPtr[fileDesc].bUsed != 0){
		fdPtr[fileDesc].inodeNum = 0;
		fdPtr[fileDesc].fileOffset = 0;
		fdPtr[fileDesc].bUsed = 0;

		return 0;
	}
	return -1;
}

int	RemoveFile(const char* szFileName){
	int inoIndex = 0;
	
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
	free(path);
	int depth = 0;
	DirEntry* retDirPtr = (DirEntry*)malloc(BLOCK_SIZE);
	int retDirPtrIndex = 0;
	int retBlkIndex = 19;
	bool is_full = false;
	//find
	while(findName(&inoIndex, &m, depth, retDirPtr, &retDirPtrIndex, &retBlkIndex ,&is_full)){
		//if found file name to remove
		if(m.used == depth + 1){
			//get fd table
			FileDesc* fdPtr = (FileDesc*)pFileDescTable;
			for(int i = 0; i < MAX_FD_ENTRY_LEN; i++){
				//if found corresponding entry
				if(fdPtr[i].inodeNum == inoIndex) return -1;
			}
			//remove file
			if(myRemoveFile(&m, depth, retDirPtr, retDirPtrIndex, inoIndex, retBlkIndex)) return 0;
		}
		inoIndex = retDirPtr[retDirPtrIndex].inodeNum;
		depth++;
	}
	//if not found
	return -1;
}

int	MakeDir(const char* szDirName){
	int inoIndex = 0;
	
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
	free(path);
	int depth = 0;
	DirEntry* retDirPtr = (DirEntry*)malloc(BLOCK_SIZE);
	int retDirPtrIndex = 0;
	int retBlkIndex = 19;
	bool is_full = false;
	//find
	while(findName(&inoIndex, &m, depth, retDirPtr, &retDirPtrIndex, &retBlkIndex, &is_full)){
		inoIndex = retDirPtr[retDirPtrIndex].inodeNum;
		depth++;
	}
	//make dir
	if(m.array[depth] != NULL){
		findName(&inoIndex, NULL, 0, retDirPtr, &retDirPtrIndex, &retBlkIndex, &is_full);
		if(myMakeDir(&m, depth, retDirPtr, retDirPtrIndex, inoIndex, retBlkIndex, is_full)){
			freeMatrix(&m);
			return 0;
		} 
		else{
			freeMatrix(&m);
			return -1;
		}
	}
	else{
		freeMatrix(&m);
		return -1;
	} 
}

int	RemoveDir(const char* szDirName){
	int inoIndex = 0;
	
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
	free(path);
	int depth = 0;
	DirEntry* retDirPtr = (DirEntry*)malloc(BLOCK_SIZE);
	int retDirPtrIndex = 0;
	int retBlkIndex = 19;
	bool is_full = false;

	//find
	while(findName(&inoIndex, &m, depth, retDirPtr, &retDirPtrIndex, &retBlkIndex, &is_full)){
		//if found dir name to remove
		if(m.used == depth + 1){
			//printf("RemoveDir()... inoIndex = %d, retDirPtr[%d].name = %s, [].inodeNum = %d...\n", inoIndex, retDirPtrIndex, retDirPtr[retDirPtrIndex].name, retDirPtr[retDirPtrIndex].inodeNum);
			//remove dir
			if(myRemoveDir(&m, depth, retDirPtr, retDirPtrIndex, inoIndex, retBlkIndex)) return 0;
		}
		//printf("RemoveDir()... inoIndex = %d, retDirPtr[%d].name = %s, [].inodeNum = %d...\n", inoIndex, retDirPtrIndex, retDirPtr[retDirPtrIndex].name, retDirPtr[retDirPtrIndex].inodeNum);
		inoIndex = retDirPtr[retDirPtrIndex].inodeNum;
		depth++;
	}
	//if not found
	return -1;

}

int	EnumerateDirStatus(const char* szDirName, DirEntryInfo* pDirEntry, int dirEntrys){
	int inoIndex = 0;
	int count = 0;

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
	free(path);

	int depth = 0;
	DirEntry* retDirPtr = (DirEntry*)malloc(BLOCK_SIZE);
	int retDirPtrIndex = 0;
	int retBlkIndex = 19;
	bool is_full = false;
	//find
	while(findName(&inoIndex, &m, depth, retDirPtr, &retDirPtrIndex, &retBlkIndex, &is_full)){
		//if found dir name to make
		if(m.used == depth + 1){
			//get inode
			Inode* inoPtr = (Inode*)malloc(sizeof(Inode));
			Inode* cInoPtr = (Inode*)malloc(sizeof(Inode));
			GetInode(retDirPtr[retDirPtrIndex].inodeNum, inoPtr);
			char* tmpBlk = (char*)malloc(BLOCK_SIZE);
			char* tmpBlk2 = (char*)malloc(BLOCK_SIZE);

			//get DirEntryInfo at direct
			for(int i=0; i < NUM_OF_DIRECT_BLOCK_PTR; i++){
				if(inoPtr->dirBlockPtr[i] != 0){
					//get dir block, cast to dirEntry*
					DevReadBlock(inoPtr->dirBlockPtr[i], tmpBlk);
					DirEntry* dirPtr = (DirEntry*)tmpBlk;
					
					//for all dir entry
					for(int j = 0; j < NUM_OF_DIRENT_PER_BLOCK; j++){
						if(strcmp(dirPtr[j].name, "") != 0){
							//get child inode to get type
							GetInode(dirPtr[j].inodeNum, cInoPtr);
							pDirEntry[count].type = cInoPtr->type;
							strcpy(pDirEntry[count].name, dirPtr[j].name);
							pDirEntry[count].inodeNum = dirPtr[j].inodeNum;
							count++;
						}
					}
				}

			}
			//get DirEntryInfo at indirect
			if(inoPtr->indirBlockPtr != 0){
				//get indir block, cast to DirEntry*
				DevReadBlock(inoPtr->indirBlockPtr, tmpBlk);
				int* indirPtr = (int*)tmpBlk;
				for(int i=0; i < BLOCK_SIZE/sizeof(int); i++){
					if(indirPtr[i] != 0){
						//get data block, cast to DirEntry*
						DevReadBlock(indirPtr[i], tmpBlk2);
						DirEntry* dirPtr = (DirEntry*)tmpBlk2;

						//for all dir entry
						for(int j = 0; j < NUM_OF_DIRENT_PER_BLOCK; j++){
							if(strcmp(dirPtr[j].name, "") != 0){
								//get child inode to get type
								GetInode(dirPtr[j].inodeNum, cInoPtr);
								pDirEntry[count].type = cInoPtr->type;
								strcpy(pDirEntry[count].name, dirPtr[j].name);
								pDirEntry[count].inodeNum = dirPtr[j].inodeNum;
								count++;
							}
						}
					}
				}
			}
			if(retDirPtr != NULL) free(retDirPtr);
			if(inoPtr != NULL) free(inoPtr);
			if(cInoPtr != NULL) free(cInoPtr);
			if(tmpBlk != NULL) free(tmpBlk);
			if(tmpBlk2 != NULL) free(tmpBlk2);
			return count;
		}
		inoIndex = retDirPtr[retDirPtrIndex].inodeNum;
		depth++;
	}
	if(retDirPtr != NULL) free(retDirPtr);
	return -1;
}
