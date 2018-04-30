#ifndef __VALIDATE_H__
#define __VALIDATE_H__
#include "fs.h"
int power(int val);
int vGetBitValue(int blkno, int bitno);
void vGetInodeValue(int inodeno, Inode* pTmpInode);
#endif

