#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include "Disk.h"


// ------- Caution -----------------------------------------
#define FS_DISK_CAPACITY	(8388608) /* 8M */
#define MAX_FD_ENTRY_LEN		(64)


#define NUM_OF_INODE_PER_BLK	(BLOCK_SIZE / sizeof(Inode))
#define NUM_OF_DIRENT_PER_BLK	(BLOCK_SIZE / sizeof(DirEntry))


#define NUM_OF_DIRECT_BLK_PTR	(2)
#define MAX_INDEX_OF_DIRBLK	(NUM_OF_DIRECT_PER_BLK)
#define MAX_NAME_LEN      (12)


#define FILESYS_INFO_BLOCK    (0) /* file system info block no. */
#define BLOCK_BITMAP_BLK_NUM  (1) /* block bitmap block no. */
#define INODE_BITMAP_BLK_NUM  (2) /* inode bitmap block no. */
#define INODELIST_BLK_FIRST   (3) /* the first block no. of inode list */
#define INODELIST_BLKS        (16) /* the number of blocks in inode list */


// ----------------------------------------------------------


typedef enum __openFlag {
	OPEN_FLAG_READWRITE,
	OPEN_FLAG_CREATE
} OpenFlag;


typedef enum __fileType {
    FILE_TYPE_FILE,
    FILE_TYPE_DIR,
    FILE_TYPE_DEV
} FileType;



typedef enum __fileMode {
	FILE_MODE_READONLY,
	FILE_MODE_READWRITE,
	FILE_MODE_EXEC
}FileMode;



typedef struct  __dirEntry {
     char name[MAX_NAME_LEN];        // file name
     int inodeNum; 
} DirEntry;



typedef enum __mountType {
    MT_TYPE_FORMAT,
    MT_TYPE_READWRITE
} MountType;



typedef struct _FileSysInfo {
    int blocks;              // \B5\F0\BD\BAũ\BF\A1 \C0\FA\C0\E5\B5\C8 \C0\FCü \BA\ED\B7\CF \B0\B3\BC\F6
    int rootInodeNum;        // \B7\E7Ʈ inode\C0\C7 \B9\F8ȣ
    int diskCapacity;        // \B5\F0\BD\BAũ \BF뷮 (Byte \B4\DC\C0\A7)
    int numAllocBlocks;      // \C6\C4\C0\CF \B6Ǵ\C2 \B5\F0\B7\BA\C5丮\BF\A1 \C7Ҵ\E7\B5\C8 \BA\ED\B7\CF \B0\B3\BC\F6
    int numFreeBlocks;       // \C7Ҵ\E7\B5\C7\C1\F6 \BE\CA\C0\BA \BA\ED\B7\CF \B0\B3\BC\F6
    int numAllocInodes;       // \C7Ҵ\E7\B5\C8 inode \B0\B3\BC\F6 
    int blockBitmapBlock;     // block bitmap\C0\C7 \BD\C3\C0\DB \BA\ED\B7\CF \B9\F8ȣ
    int inodeBitmapBlock;     // inode bitmap\C0\C7 \BD\C3\C0\DB \BA\ED\B7\CF \B9\F8ȣ
    int inodeListBlock;		// inode list\C0\C7 \BD\C3\C0\DB \BA\ED\B7\CF \B9\F8ȣ
    int dataReionBlock;		// data region\C0\C7 \BD\C3\C0\DB \BA\ED\B7\CF \B9\F8ȣ
} FileSysInfo;




typedef struct _Inode {
    short 	size;              // \C6\C4\C0\CF ũ\B1\E2
    short 	type;              // \C6\C4\C0\CF Ÿ\C0\D4
    int 	dirBlkPtr[NUM_OF_DIRECT_BLK_PTR];	// Direct block pointers
    int   	indirBlkPointer;	// Single indirect block pointer
} Inode;



typedef struct __fileDesc {
	int	bUsed;
	int	fileOffset;
	int	inodeNum;
}FileDesc;

typedef struct __fileDescTable {
	FileDesc	file[MAX_FD_ENTRY_LEN];
}FileDescTable;

extern int		OpenFile(const char* szFileName, OpenFlag flag);
extern int		WriteFile(int fileDesc, char* pBuffer, int length);
extern int		ReadFile(int fileDesc, char* pBuffer, int length);
extern int		CloseFile(int fileDesc);
extern int		RemoveFile(const char* szFileName);
extern int		MakeDir(const char* szDirName);
extern int		RemoveDir(const char* szDirName);
extern void		EnumerateDirStatus(const char* szDirName, DirEntry* pDirEntry, int* pNum);
extern void		Mount(MountType type);
extern void		Unmount(void);

extern FileDescTable* pFileDescTable;
extern FileSysInfo* pFileSysInfo;

/*  File system internal functions */


void FileSysInit(void);
void SetInodeBitmap(int inodeno);
void ResetInodeBitmap(int inodeno);
void SetBlockBitmap(int blkno);
void ResetBlockBitmap(int blkno);
void PutInode(int inodeno, Inode* pInode);
void GetInode(int inodeno, Inode* pInode);
int GetFreeInodeNum(void);
int GetFreeBlockNum(void);



#endif /* FILESYSTEM_H_ */
