#ifndef __DISK_H__
#define __DISK_H__


#define BLOCK_SIZE (64)

extern void DevCreateDisk(void);

extern void DevOpenDisk(void);
    
extern void DevReadBlock(int blkno, char* pBuf);

extern void DevWriteBlock(int blkno, char* pBuf);

#endif /* __DISK_H__ */
