#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fs.h"
#include "Disk.h"


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
			fsPtr->numFreeBlocks = (BLOCK_SIZE * 8) - (3 + INODELIST_BLKS + 1);
			fsPtr->numAllocInodes = 1;
			fsPtr->blockBitmapBlock = BLOCK_BITMAP_BLK_NUM;
			fsPtr->inodeBitmapBlock = INODE_BITMAP_BLK_NUM;
			fsPtr->inodeListBlock = INODELIST_BLK_FIRST;
			fsPtr->dataReionBlock = 19;
			DevWriteBlock(FILESYS_INFO_BLOCK, (char*)fsPtr);
			
			//set bitmap
			SetBlockBitmap(fBlkIndex);
			SetInodeBitmap(fInoIndex);

			//update Inode
			Inode* inoPtr = (Inode*)malloc(sizeof(Inode));
			GetInode(fInoIndex, inoPtr);
			inoPtr->size = 0;
			inoPtr->type = FILE_TYPE_DIR;
			inoPtr->dirBlkPtr[0] = fBlkIndex;
			inoPtr->indirBlkPointer = 0;
			PutInode(fInoIndex, inoPtr);
			break;

		//if MT_TYPE_READWRITE
		case MT_TYPE_READWRITE:
			//open virtual disk
			DevOpenDisk();	
			break;
	}
	
}


void		Unmount(void)
{
	//close(fd);
}

