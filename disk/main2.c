#include <stdio.h>
#include <stdlib.h>
#include "fs.h"
#include "disk.h"

void main(void)
{
	int inodeno, blkno;
	Inode temp, temp2, temp3;
	FileSysInit();
	for(int i=0; i<30; i++)
		SetInodeBitmap(i);
	temp.allocBlocks = 3;
	temp.type = 7;
	temp.blockPointer[0] = 227;
	temp.blockPointer[1] = 158;
	inodeno = GetFreeInodeNum();
	blkno = GetFreeBlockNum();
	printf("free inodenum : %d\n", inodeno);
	PutInode(inodeno, &temp);
	SetInodeBitmap(inodeno);
	GetInode(inodeno, &temp2);
	printf("Inode 2 -- AllocBlocks : %d\n", temp2.allocBlocks);
	printf("Inode 2 -- Type : %d\n", temp2.type);
	temp2.allocBlocks = 582;
	inodeno = GetFreeInodeNum();
	printf("free inodenum : %d\n", inodeno);
	PutInode(inodeno, &temp2);
	SetInodeBitmap(inodeno);
	GetInode(inodeno, &temp3);
	printf("Inode 3 -- AllockBlocks : %d\n", temp3.allocBlocks);
	printf("Inode 3 -- Type : %d\n", temp3.type);
	printf("Inode 3 -- blockPointer[0] : %d\n", temp3.blockPointer[0]);
	printf("Inode 3 -- blockPointer[1] : %d\n", temp3.blockPointer[1]);
	ResetInodeBitmap(7);
	ResetInodeBitmap(14);
	ResetInodeBitmap(21);
	ResetInodeBitmap(28);
	char *block = (char*)malloc(BLOCK_SIZE);
	DevReadBlock(1, block);
	for(int i=0; i<5; i++) {
		for(int j=7; j>=0; j--)
			printf("%d", *(block+i) >> j & 1);
		printf("\n");
	}
	printf("실행 완료\n");
}
