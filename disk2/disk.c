#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "Disk.h"

int fd; 

void DevCreateDisk(void)
{
    fd = open("MY_DISK", O_RDWR | O_CREAT | O_TRUNC, 0644);
}

void DevOpenDisk(void)
{
	fd = open("MY_DISK", O_RDWR);
}

void __DevMoveBlock(int blkno){
    lseek(fd, (off_t)+(BLOCK_SIZE*blkno),SEEK_SET);
}

void DevReadBlock(int blkno, char* pBuf)
{
   __DevMoveBlock(blkno);
   read(fd, pBuf, BLOCK_SIZE);
}

void DevWriteBlock(int blkno, char* pBuf)
{
   __DevMoveBlock(blkno);
   write(fd, pBuf, BLOCK_SIZE);
}
