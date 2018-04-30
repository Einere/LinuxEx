#include <stdlib.h>
#include "fs.h"
#include "disk.h"
#include "validate.h"
#include <assert.h>
#include <stdio.h>

#define INODE_SIZE (64)
#define NUM_INODE_PER_BLOCK (BLOCK_SIZE/INODE_SIZE)	// 512/64 = 8
#define TOTAL_INODE_NUM (NUM_INODE_PER_BLOCK * INODELIST_BLKS) 	// 8 * 4 = 32
#define TOTAL_BLOCK_NUM (TOTAL_INODE_NUM)	//32

void testcase1(void)
{
	int i =0;
	FileSysInit();
	
	for(i=0; i < TOTAL_BLOCK_NUM;i++)
	{
		if(i%4==3)
		{
			SetBlockBitmap(i);
			
			if(vGetBitValue(BLOCK_BITMAP_BLK_NUM, i)==1)
			{
				printf("Success about SetBlockBitmap().\n");
			}
			else
			{
				printf("Fail about SetBlockBitmap().\n");
				return;
			}
			
		}
	}
	
	for(i=0;i<TOTAL_BLOCK_NUM;i++)
	{
		if(i%8==7)
		{
			ResetBlockBitmap(i);
			
			if(vGetBitValue(BLOCK_BITMAP_BLK_NUM,i)==0)
			{
				printf("Success about ResetBlockBitmap().\n");
			}
			else
			{
				printf("Fail about ResetBlockBitmap().\n");
				return;
			}
			
		}
	}
	
	printf("Testcase 1 Complete!\n");
}


void testcase2(void)
{
	int i=0;
	int BitValue =0;
	
	FileSysInit();
	
	for(i=0;i<TOTAL_BLOCK_NUM;i++)
	{
		SetBlockBitmap(i);
	}
	
	for(i=0;i<TOTAL_BLOCK_NUM;i++)
	{
		if(i%8==7)
		{
			ResetBlockBitmap(i);
			if(i== GetFreeBlockNum())
			{
				
				if(vGetBitValue(BLOCK_BITMAP_BLK_NUM,i)==0)
				{
					BitValue = vGetBitValue(BLOCK_BITMAP_BLK_NUM,i);
					printf("Reseult of GetFreeBlockNum() : %d ,	The Value of Bit %d : %d \n", GetFreeBlockNum(), GetFreeBlockNum(), BitValue);
					SetBlockBitmap(i);
				}
				else
				{
					printf("Fail : The Value of Bit %d is not 0.\n",GetFreeBlockNum());
					return;
				}
				
			}
			else
			{
				printf("Fail : GetFreeBlockNum() is incorrect!\n");
				return;
			}
		}
	}
	
	printf("TestCase 2 Complete!\n");
}


void testcase3(void)
{
	int i =0;
	FileSysInit();
	
	for(i=0; i < TOTAL_INODE_NUM;i++)
	{
		if(i%4==3)
		{
			SetInodeBitmap(i);
			
			if(vGetBitValue(INODE_BITMAP_BLK_NUM, i)==1)
			{
				printf("Success about SetInodeBitmap().\n");
			}
			else
			{
				printf("Fail about SetInodeBitmap().\n");
				return;
			}
			
		}
	}
	
	for(i=0;i<TOTAL_INODE_NUM;i++)
	{
		if(i%8==7)
		{
			ResetInodeBitmap(i);
			
			if(vGetBitValue(INODE_BITMAP_BLK_NUM,i)==0)
			{
				printf("Success about ResetInodeBitmap().\n");
			}
			else
			{
				printf("Fail about ResetInodeBitmap().\n");
				return;
			}
			
		}
	}
	
	printf("Testcase 3 Complete!\n");

}

void testcase4(void)
{
	int i=0;
	int BitValue =0;
	FileSysInit();
	
	for(i=0;i<TOTAL_INODE_NUM;i++)
	{
		SetInodeBitmap(i);
	}
	
	for(i=0;i<TOTAL_INODE_NUM;i++)
	{
		if(i%8==7)
		{	
			ResetInodeBitmap(i);
			if(i== GetFreeInodeNum())
			{
				
				if(vGetBitValue(INODE_BITMAP_BLK_NUM,i)==0 )
				{
					BitValue = vGetBitValue(INODE_BITMAP_BLK_NUM,i);
					printf("Reseult of GetFreeInodeNum() : %d ,	The Value of Bit %d : %d \n", GetFreeInodeNum(), GetFreeInodeNum(), BitValue);
					SetInodeBitmap(i);
				}
				else
				{
					printf("Fail : The Value of Bit %d is not 0.\n",GetFreeInodeNum());
					return;
				}
				
			}
			else
			{
				printf("Fail : GetFreeInodeNum() is incorrect!\n");
				return;
			}
		}
	}	
	printf("TestCase 4 Complete!\n");
}

void testcase5(void)
{
    FileSysInit();
    int i =0;
    
    
    Inode * pInode = (Inode*)malloc(sizeof(Inode));
    Inode * pTmpInode = (Inode*)malloc(sizeof(Inode));
    for(i=0;i<TOTAL_INODE_NUM;i++)
    {
        if(i%2==1)
        {
            pInode->allocBlocks = i;
            pInode->blockPointer[2]=i;
            pInode->blockPointer[11]=i;

            PutInode(i,pInode);
               vGetInodeValue(i, pTmpInode); 

			
            if(pTmpInode->allocBlocks == pInode->allocBlocks 
            && pTmpInode->blockPointer[2] == pInode->blockPointer[2]
            && pTmpInode->blockPointer[11]== pInode->blockPointer[11] )
            {
                
            }
            else
            {
                printf("TestCase 5: PutInode Failed!\n");
                return;
            }
           
        }
    }

    for(i=0;i<TOTAL_INODE_NUM;i++)
    {
        if(i%2==1)
        {
            GetInode(i,pInode);
               vGetInodeValue(i, pTmpInode); 

			
           if(pTmpInode->allocBlocks == pInode->allocBlocks
                && pTmpInode->blockPointer[2] == pInode->blockPointer[2]
                && pTmpInode->blockPointer[11]== pInode->blockPointer[11])
            {
            
            }
            else
            {
                printf("TestCase 5 Fail!\n");
            }
            
        }
    }
    printf("Testcase 5 Complete!\n");
}

int main(int argc, char* argv[])
{
	int tcNum;
	if(argc != 2)
	{
		perror("Input TestCase Number!");
		exit(0);
	}
	
	tcNum = atoi(argv[1]);
	
	switch(tcNum)
	{
		case 1:
			testcase1();
			break;
		case 2:
			testcase2();
			break;
		case 3:
			testcase3();
			break;
		case 4:
			testcase4();
			break;
		case 5:
			testcase5();
			break;
	}
	
	return 0;
}
