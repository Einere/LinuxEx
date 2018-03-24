#include <stdio.h>
#include "fs.h"
#include "disk.h"

void testcase1(void)
{
    int n = GetFreeInodeNum();
    printf("free inode num : %d\n",n);
    
    Inode inode={10,11,0};
    Inode check;
    PutInode(1, &inode);
    SetInodeBitmap(1);

    GetInode(1, &check);
    printf("%d, %d\n", check.allocBlocks, check.type);

    n = GetFreeInodeNum();
    printf("free inode num : %d\n",n);

    inode.allocBlocks = 1234;
    inode.type = 9999;
    PutInode(0, &inode);
    SetInodeBitmap(0);

    GetInode(0, &check);
    printf("%d, %d\n", check.allocBlocks, check.type);

    n = GetFreeInodeNum();
    printf("free inode num : %d\n",n);
}

// checking set inode bitmap and get free inode number
void testcase2()
{
    for(int i=1; i<100; ++i)
    {
        SetInodeBitmap(i);
    }
    int n =GetFreeInodeNum();
    printf("free : %d\n",n);

    SetInodeBitmap(0);
    n =GetFreeInodeNum();
    printf("free : %d\n",n);
}

void testcase3()
{
    Inode inode = {1, 2};
    PutInode(1, &inode);
    
    inode.allocBlocks = 11;
    inode.type = 22;
    PutInode(2, &inode);


    Inode result ={7,7};
    for(int i=0; i<3; ++i)
    {
        GetInode(i, &result);
        printf("%d, %d\n",result.allocBlocks, result.type);
    }

}
void testcase4()
{
	DevCreateDisk();
	DevOpenDisk();


	int blkno, inodeno; 
	FileSysInit(); 

	Inode inode = { 3, 1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
	PutInode(0, &inode);
	GetInode(0, &inode);
	
	for(int i=0; i<NUM_OF_BLK_PTR; i++)
		printf("blockPointer[%d] : %d\n", i, inode.blockPointer[i]);


	inodeno = GetFreeInodeNum(); 
	printf("before setInodeBitmap() : %d\n", inodeno);
	SetInodeBitmap(inodeno); 
	printf("after setInodeBitmap() : %d\n", GetFreeInodeNum());

	blkno = GetFreeBlockNum(); 
	SetBlockBitmap(blkno);

	printf("GetFreeBlocNum() and SetBlockBitamp() : %d\n", blkno);
	printf("next free block num : %d\n", GetFreeBlockNum());


	FileSysInit();
	printf("File System Initialize ...\n");
	printf("GetFreeInodeNum() : %d\n", GetFreeInodeNum());
	printf("GetFreeBlockNum() : %d\n", GetFreeBlockNum());
}

void testcase5()
{
    const int maxBitCount = 512<<3;
    int accum = 0;
    for(int i=0; i<maxBitCount; ++i)
    {
        SetBlockBitmap(i);
    }
    for(int i=maxBitCount-1; i>=0; --i)
    {
        ResetBlockBitmap(i);
        int n = GetFreeBlockNum();
        accum += n;
    }
    int correctAns = (((maxBitCount-1)*(maxBitCount))>>1);
    int ans = accum;
    printf("%d\n", correctAns^ans);
}

void testcase6()
{
    int inodeCount = INODELIST_BLKS*BLOCK_SIZE/sizeof(Inode);
    Inode inode = {0, 0, 0};
    for(int index=0; index<inodeCount; ++index)
    {
        PutInode(index, &inode);
        inode.allocBlocks++;
        inode.type++;
        /** inode.allocBlocks += inode.type; */
        /** inode.type += inode.allocBlocks; */
    }

    int sum;
    for(int index=0; index<inodeCount; ++index)
    {
        GetInode(index, &inode);
        sum = inode.allocBlocks+inode.type;
        printf("%d ",sum);
    }
    printf("\nresult : ");
    if(sum==(inodeCount-1)*2)
        printf("success! :)");
    else
        printf("FAILED... :(");
    puts("");
}

void main(void)
{
	FileSysInit();
    testcase6();
}
