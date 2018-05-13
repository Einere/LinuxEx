#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fs.h"
#include "disk.h"


FileSysInfo* pFileSysInfo = NULL;


void		Mount(MountType type)
{
	switch(type){
		//IF MT_TYPE_FORMAT
		case MT_TYPE_FORMAT:
			//init disk
			FileSysInit();
			
			//get free block index & free Inode index
			int fBlkIndex = GetFreeBlockNum();
			int fInoIndex = GetFreeInodeNum(); 
			
			//alloc memory, cast to DirEntry
			DirEntry* dirEntryPtr = (DirEntry*)malloc(BLOCK_SIZE);

			//set root dir, write to disk
			strcpy(dirEntryPtr[0].name, "."); 
			dirEntryPtr[0].inodeNum = fInoIndex;
			DevWriteBlock(fBlkIndex, (char*)dirEntryPtr);

			//alloc memory, cast to FileSysInfo
			FileSysInfo* fsPtr = (FileSysInfo*)malloc(BLOCK_SIZE);

			//set sys info, write to disk
			fsPtr->blocks = BLOCK_SIZE * 8;
			fsPtr->rootInodeNum = fInoIndex;
			fsPtr->diskCapacity = FS_DISK_CAPACITY;
			fsPtr->numAllocBlocks = 1;
			fsPtr->numFreeBlocks = (BLOCK_SIZE * 8) - (3 + INODELIST_BLOCKS + 1);
			fsPtr->numAllocInodes = 1;
			fsPtr->blockBitmapBlock = BLOCK_BITMAP_BLOCK_NUM;
			fsPtr->inodeBitmapBlock = INODE_BITMAP_BLOCK_NUM;
			fsPtr->inodeListBlock = INODELIST_BLOCK_FIRST;
			fsPtr->dataRegionBlock = 19;
			DevWriteBlock(FILESYS_INFO_BLOCK, (char*)fsPtr);
			
			//set bitmap
			SetBlockBitmap(fBlkIndex);
			SetInodeBitmap(fInoIndex);

			//update Inode
			Inode* inoPtr = (Inode*)malloc(sizeof(Inode));
			GetInode(fInoIndex, inoPtr);
			inoPtr->size = 0;
			inoPtr->type = FILE_TYPE_DIR;
			inoPtr->dirBlockPtr[0] = fBlkIndex;
			inoPtr->indirBlockPtr = 0;
			PutInode(fInoIndex, inoPtr);
			
			//free
			free(dirEntryPtr);
			free(fsPtr);
			break;

		//if MT_TYPE_READWRITE
		case MT_TYPE_READWRITE:
			//open virtual disk
			DevOpenDisk();	
			
			//init fd table
			pFileDescTable = (FileDescTable*)malloc(sizeof(FileDescTable));
			memset(pFileDescTable, 0, sizeof(FileDescTable));

			//load file sys info
			if(pFileSysInfo == NULL){
				pFileSysInfo = (FileSysInfo*)malloc(BLOCK_SIZE);
				DevReadBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
			}

			break;
	}
	
}


void		Unmount(void)
{
	DevCloseDisk();
}

