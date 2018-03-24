#define NUM_OF_BLK_PTR  (14)
	
#define INODE_BITMAP_BLK_NUM  (1) /* inode bitmap block no. */
#define BLOCK_BITMAP_BLK_NUM  (2) /* block bitmap block no. */
#define INODELIST_BLK_FIRST   (3) /* the first block no. of inode list */
#define INODELIST_BLKS        (4) /* the number of blocks in inode list */
 

typedef struct __Inode {
	int			allocBlocks;
	int			type;
	int			blockPointer[NUM_OF_BLK_PTR];	// Direct block pointers
} Inode;

void FileSysInit(void);
void SetInodeBitmap(int inodeno);
void ResetInodeBitmap(int inodeno);
void SetBlockBitmap(int blkno);
void ResetBlockBitmap(int blkno);
void PutInode(int blkno, Inode* pInode);
void GetInode(int blkno, Inode* pInode);
int GetFreeInodeNum(void);
int GetFreeBlockNum(void);




