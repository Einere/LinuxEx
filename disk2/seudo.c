이름찾기
다이렉트포인터
    찾았다
        재귀호출. 
        플래그 true.
        break.
인다이렉트포인터
    찾았다
        재귀호출.
        플래그 true.
        break. 
!플래그 -> 디렉만들기 호출.


bool findName(int inoIndex, Matrix* m, int depth, DirEntry* retDirPtr, int* retDirPtrIndex, int* retBlkIndex)
다이렉트포인터
    if(m == NULL)
        if(dirPtr[j] == "")
            set retDirPtr, retDirPtrIndex, retBlkIndex;
            return true;
    else
        if(dirPtr[j] == m->array[depth])
            set retDirPtr, retDirPtrIndex;
            return true.
인다이렉트포인터
    if(m == NULL)
        if(dirPtr[j] == "")
            set retDirPtr, retDirPtrIndex, retBlkIndex;
            return true;
    else
        if(dirPtr[j] == m->array[depth])
            set retDirPtr, retDirPtrIndex;
            return true.
(못찾았다)
set retDirPtr, retDirPtrIndex, retBlkIndex;
return false.


MakeDir(path)
//set argument
int root = 0;
int* inoIndex = &root;
Matrix mat;
parse & save;
Matrix* m = &mat;
depth = 0;
DirEntry* retDirPtr = (DirEntry*)malloc();
int* retDirPtrIndex = (int*)malloc(sizeof(int));
int* retBlkIndex; = (int*)malloc(sizeof(int));
//recursive find
while(findName(inoIndex, m, depth, retDirPtr, retDirPtrIndex, retBlkIndex)) {
    inoIndex = retDirPtr[retDirPtrIndex].inodeNum;
    depth++;
}
//not find 중복되는 name
if(m->array[depth] != NULL){
    if(myMakeDir(m, depth, retDirPtr, retDirPtrIndex, inoIndex, retBlkIndex)) return 0;
    else return -1;
}
//만들고자 하는 path와 이미 만들어진 path가 동일한 경우.
else return -1;


bool myMakeDir(Matrix* m, int depth, DirEntry* dirPtr, int* dirPtrIndex, int* blkIndex)
dirPtr[dirPtrIndex].name = m->array[depth];
dirPtr[dirPtrIndex].inodeNum = GetFreeInodeNum();
DevWriteBlock(blkIndex, (char*)retDirPtr);
//나머지 설정 후 write

/*----------------------------------------------------------*/
//get inode
int inoIndex = fdTbl[fd].inodeNum;
Inode* inoPtr = (Inode*)malloc(sizeof(Inode));
GetInode(inoIndex, inoPtr);

int count = 0;
int preCount = 0;
char* tmpBlk = (char*)malloc(BLOCK_SIZE);
int nWrite = 0;
//append
if(offset + length <= 18 * BLOCK_SIZE){
    int occupy = offset % BLOCK_SIZE;
    int avail = BLOCK_SIZE - occupy;
    int written = 0;
    int lastIndex = offset / BLOCK_SIZE;
    int appendIndex = 0;
    //if remain data to write
    while(written < length){
        //at direct
        if(lastIndex < NUM_OF_DIRECT_BLK_PTR){
            //get block index to write
            if(inoPtr->dirPtr[lastIndex] != 0) appendIndex = inoPtr->dirPtr[lastIndex];
            else appendIndex = GetFreeBlockNum();
            
            //if use already exist block
            if(written == 0){
                //set block, write to disk
                DevReadBlock(appendIndex, tmpBlk);
                nWrite = strncpy(tmpBlk+occupy, buffer, avail);
                DevWriteBlock(appendIndex, tmpBlk);
            }
            //if use newly alloced block
            else{
                //set block, write to disk
                nWrite = strncpy(tmpBlk, buffer, BLOCK_SIZE);
                DevWriteBlock(appendIndex, tmpBlk);
                //modify dirPtr, write to disk
                inoPtr->dirPtr[lastIndex] = appendIndex;
                PutInode(inoIndex, inoPtr);
                //set bitmap
                SetBlockBitmap(appendIndex);
                //increase count
                count++:
            }
            //modify offset
            fdTbl[fd].fileOffset += nWrite;
            //modify lastIndex, written
            lastIndex++;
            written += nWrite;
        }
        //at indirect
        else{
            //get block index to write
            int* indirPtrArr = (int*)malloc(BLOCK_SIZE);
            DevReadBlock(inoPtr->indirPtr, (char*)indirPtrArr);
            
            if(indirPtrArr[lastIndex-2] != 0) appendIndex = indirPtrArr[lastIndex-2];
            else appendIndex = GetFreeBlockNum();

            //if use already exist block
            if(written == 0){
                //set block, write to disk
                DevReadBlock(appendIndex, tmpBlk);
                nWrite = strncpy(tmpBlk+occupy, buffer, avail);
                DevWriteBlock(appendIndex, tmpBlk);
            }
            //if use newly alloced block
            else{
                //set block, write to disk
                nWrite = strncpy(tmpBlk, buffer, BLOCK_SIZE);
                DevWriteBlock(appendIndex, tmpBlk);
                //modify indirPtr, write to disk
                indirPtrArr[lastIndex-2] = appendIndex;
                DevWriteBlock(inoPtr->indirPtr, (char*)indirPtrArr);
                //set bitmap
                SetBlockBitmap(appendIndex);
                //increase count
                count++:
            }
            //modify offset
            fdTbl[fd].fileOffset += nWrite;
            //modify occupy, avail,  lastIndex, written
            lastIndex++;
            written += nWrite;
        }
    }
}
//over-write
else{
    count = 0, preCount = ceil(fdTbl[fd].fileOffset / BLOCK_SIZE);
    fdTbl[fd].fileOffset = 0;
    
    int overIndex = 0;
    //copy data block
    //at direct ptr
    for(int i = 0; i < NUM_OF_DIRECT_BLK_PTR; i++){
        //get block index to write
        if(inoPtr->dirPtr[i] != 0) overIndex = inoPtr->dirPtr[i];
        else overIndex = GetFreeBlockNum();
        
        //if remain data to write
        if((BLOCK_SIZE * count) < length){
            //copy data, write to disk
            nWrite = strncpy(tmpBlk, buffer+BLOCK_SIZE*count, BLOCK_SIZE);
            DevWriteBlock(overIndex, tmpBlk);
            //if alloc new block
            if(count >= preCount){
                //modify dirPtr, write to disk
                inoPtr->dirPtr[i] = overIndex;
                PutInode(inoIndex, inoPtr);
                //set bitmap
                SetBlockBitmap(overIndex);
            }
            //modify offset
            fdTbl[fd].fileOffset += nWrite;
            //increase count
            count++;
        }
        //if no data to write, write black block
        else{
            memset(tmpBlk, 0, BLOCK_SIZE);
            DevWriteBlock(overIndex, tmpBlk);
            //reset dirPtr
            inoPtr->dirPtr[i] = 0;
            PutInode(inoIndex, inoPtr);
            //reset bitmap
            ResetBlockBitmap(overIndex);
        }
    }
    //at indirect ptr
    for(int i = 0; i < BLOCK_SIZE / sizeof(int); i++){
        //get block index to write
        int* indirPtrArr = (int*)malloc(BLOCK_SIZE);
        DevReadBlock(inoPtr->indirPtr, (char*)indirPtrArr);
        //overIndex = indirPtrArr[i];

        //if remain data to write
        if((BLOCK_SIZE * count) < length){
            //copy data, write to disk
            nWrite = strncpy(tmpBlk, buffer + BLOCK_SIZE * count, BLOCK_SIZE);
            DevWriteBlock(indirPtrArr[i], tmpBlk);
            
            //if alloc new block
            if(count >= preCount){
                //modify dirPtr, write to disk
                inoPtr->dirPtr[i] = indirPtrArr[i];
                PutInode(inoIndex, inoPtr);
                //set bitmap
                SetBlockBitmap(indirPtrArr[i]);
            }
            //modify offset
            fdTbl[fd].fileOffset += nWrite;
            //increase count
            count++;
        }
        //if no data to write, write black block
        else{
            memset(tmpBlk, 0, BLOCK_SIZE);
            DevWriteBlock(indirPtrArr[i], tmpBlk);
            //reset indirPtr
            inoPtr->indirPtr[i] = 0;
            //reset bitmap
            ResetBlockBitmap(indirPtrArr[i]);
        }
    }
}
//update inode
PutInode(inoIndex, inoPtr);

//update file sys info
DevReadBlock(0, tmpBlk);
FileSysInfo* fsInfoPtr = (FileSysInfo*)tmpBlk;
fsInfoPtr->numAllocBlocks -= (preCount - count);
fsInfoPtr->numFreeBlocks = (BLOCK_SIZE*8) - fsInfoPtr->numAllocBlocks;
    

    
