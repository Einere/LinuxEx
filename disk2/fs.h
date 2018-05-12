#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include "disk.h"


// ------- Caution -----------------------------------------
#define FS_DISK_CAPACITY		(BLOCK_SIZE*512) 	/* 수정 */
#define MAX_FD_ENTRY_LEN		(64)


#define NUM_OF_INODE_PER_BLOCK	(BLOCK_SIZE / sizeof(Inode))	/* block 당 inode 개수 */
#define NUM_OF_DIRENT_PER_BLOCK	(BLOCK_SIZE / sizeof(DirEntry)) /* block 당 directory entry 개수 */


#define NUM_OF_DIRECT_BLOCK_PTR	(2)	/* direct block pointer의 개수 */
#define MAX_NAME_LEN     	(12)


#define FILESYS_INFO_BLOCK    (0) /* file system info block no. */
#define BLOCK_BITMAP_BLOCK_NUM  (1) /* block bitmap block no. */
#define INODE_BITMAP_BLOCK_NUM  (2) /* inode bitmap block no. */
#define INODELIST_BLOCK_FIRST   (3) /* the first block no. of inode list */
#define INODELIST_BLOCKS        (16) /* the number of blocks in inode list */


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


/* 추가 */
typedef struct __dirEntryInfo {
    char name[MAX_NAME_LEN];
    int inodeNum;
    FileType type;
} DirEntryInfo; 



typedef enum __mountType {
    MT_TYPE_FORMAT,
    MT_TYPE_READWRITE
} MountType;



typedef struct _FileSysInfo {
    int blocks;              // 디스크에 저장된 전체 블록 개수
    int rootInodeNum;        // 루트 inode의 번호
    int diskCapacity;        // 디스크 용량 (Byte 단위)
    int numAllocBlocks;      // 파일 또는 디렉토리에 할당된 블록 개수
    int numFreeBlocks;       // 할당되지 않은 블록 개수
    int numAllocInodes;       // 할당된 inode 개수 
    int blockBitmapBlock;     // block bitmap의 시작 블록 번호
    int inodeBitmapBlock;     // inode bitmap의 시작 블록 번호
    int inodeListBlock;		// inode list의 시작 블록 번호
    int dataRegionBlock;		// data region의 시작 블록 번호 (수정)
} FileSysInfo;




typedef struct _Inode {
    short 	size;              	// 파일 크기
    short 	type;              	// 파일 타입
    int 	dirBlockPtr[NUM_OF_DIRECT_BLOCK_PTR];	// Direct block pointers (수정)
    int   	indirBlockPtr;	// Single indirect block pointer
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
extern int 		EnumerateDirStatus(const char* szDirName, DirEntryInfo* pDirEntry, int dirEntrys );  
extern void		Mount(MountType type);
extern void		Unmount(void);


extern FileDescTable* pFileDescTable;
extern FileSysInfo* pFileSysInfo;

extern void FileSysInit(void);	

/*  File system internal functions */



void SetInodeBitmap(int inodeno);
void ResetInodeBitmap(int inodeno);
void SetBlockBitmap(int blkno);
void ResetBlockBitmap(int blkno);
void PutInode(int inodeno, Inode* pInode);
void GetInode(int inodeno, Inode* pInode);
int GetFreeInodeNum(void);
int GetFreeBlockNum(void);



#endif /* FILESYSTEM_H_ */
