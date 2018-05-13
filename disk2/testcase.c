#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>
#include "disk.h"
#include "fs.h"



#define FILENAME_MAX_LEN 30
#define DIR_NUM_MAX      100


void PrintInodeBitmap(void)
{
	int i;
	int count;
	int* pBitmap = (int*)malloc(BLOCK_SIZE);

	count = BLOCK_SIZE / sizeof(int);
	DevReadBlock(2, (char*)pBitmap);
	printf("Inode bitmap: ");
	for (i = 0; i < count; i++)
		printf("%d", pBitmap[i]);
	printf("\n");
}

void PrintBlockBitmap(void)
{
	int i;
	int count;
	int* pBitmap = (int*)malloc(BLOCK_SIZE);

	count = BLOCK_SIZE / sizeof(int);         /* bit ������ 64*8 ���� ���� */
	DevReadBlock(1, (char*)pBitmap);
	printf("Block bitmap");
	for (i = 0; i < count; i++)
		printf("%d", pBitmap[i]);
	printf("\n");
}

void ReadInode(Inode* pInode, int inodeNo)
{
	char* pBuf = NULL;
	Inode* pMem = NULL;
	int block = pFileSysInfo->inodeListBlock + inodeNo / NUM_OF_INODE_PER_BLOCK;
	int inode = inodeNo % NUM_OF_INODE_PER_BLOCK;

	pBuf = (char*)malloc(BLOCK_SIZE);

	DevReadBlock(block, pBuf);
	pMem = (Inode*)pBuf;
	memcpy(pInode, &pMem[inode], sizeof(Inode));
}

void ListDirContentsAndSize(const char* dirName)
{
	int i;
	int count;
	DirEntryInfo pDirEntry[DIR_NUM_MAX];
	Inode pInode;

	count = EnumerateDirStatus(dirName, pDirEntry, DIR_NUM_MAX);

	printf("[%s]Sub-directory:\n", dirName);
	for (i = 0; i < count; i++)
	{
		if (pDirEntry[i].type == FILE_TYPE_FILE) {
			GetInode(pDirEntry[i].inodeNum, &pInode);
			printf("\t name:%s, inode no:%d, type:file, size:%d\n", pDirEntry[i].name, pDirEntry[i].inodeNum, pInode.size);
		}
		else if (pDirEntry[i].type == FILE_TYPE_DIR)
			printf("\t name:%s, inode no:%d, type:directory\n", pDirEntry[i].name, pDirEntry[i].inodeNum);
		else
		{
			assert(0);
		}
	}
}
void ListDirContents(const char* dirName)
{
	int i;
	int count;
	DirEntryInfo pDirEntry[DIR_NUM_MAX];

	count = EnumerateDirStatus(dirName, pDirEntry, DIR_NUM_MAX);
	printf("[%s]Sub-directory:\n", dirName);
	for (i = 0; i < count; i++)
	{
		if (pDirEntry[i].type == FILE_TYPE_FILE)
			printf("\t name:%s, inode no:%d, type:file\n", pDirEntry[i].name, pDirEntry[i].inodeNum);
		else if (pDirEntry[i].type == FILE_TYPE_DIR)
			printf("\t name:%s, inode no:%d, type:directory\n", pDirEntry[i].name, pDirEntry[i].inodeNum);
		else
		{
			assert(0);
		}
	}
}


void TestCase1(void)
{
	int i;
	char dirName[MAX_NAME_LEN];

	printf(" ---- Test Case 1 ----\n");

	MakeDir("/tmp");
	MakeDir("/usr");
	MakeDir("/etc");
	MakeDir("/home");
	/* make home directory */
	for (i = 0; i < 7; i++)
	{
		memset(dirName, 0, MAX_NAME_LEN);
		sprintf(dirName, "/home/user%d", i);
		MakeDir(dirName);
	}
	/* make etc directory */
	for (i = 0; i < 24; i++)
	{
		memset(dirName, 0, MAX_NAME_LEN);
		sprintf(dirName, "/etc/dev%d", i);
		MakeDir(dirName);
	}
	ListDirContents("/home");
	ListDirContents("/etc");

	/* remove subdirectory of etc directory */
	for (i = 23; i >= 0; i--)
	{
		memset(dirName, 0, MAX_NAME_LEN);
		sprintf(dirName, "/etc/dev%d", i);
		RemoveDir(dirName);
	}

	ListDirContents("/etc");

	/* remove subdirectory of root directory except /home */
	RemoveDir("/etc");
	RemoveDir("/usr");
	RemoveDir("/tmp");
}


void TestCase2(void)
{
	int i, j;
	int fd;
	char fileName[FILENAME_MAX_LEN];
	char dirName[MAX_NAME_LEN];
	char pBuffer1[BLOCK_SIZE];

	printf(" ---- Test Case 2 ----\n");

	
	ListDirContents("/home");
	/* make home directory */
	for (i = 0; i < 7; i++)
	{

		for (j = 0; j < 7; j++)
		{

			memset(fileName, 0, FILENAME_MAX_LEN);
			sprintf(fileName, "/home/user%d/file%d", i, j);
			fd = OpenFile(fileName, OPEN_FLAG_CREATE);
			memset(pBuffer1, 0, BLOCK_SIZE);
			strcpy(pBuffer1, fileName);
			WriteFile(fd, pBuffer1, BLOCK_SIZE);

			CloseFile(fd);
		}
	}

	for (i = 0; i < 7; i++)
	{
		memset(dirName, 0, MAX_NAME_LEN);
		sprintf(dirName, "/home/user%d", i);
		ListDirContents(dirName);
	}
}

void TestCase3(void) {
	int i, j, k;
	char alphabet[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$^&*()_";
	char fileName[FILENAME_MAX_LEN];
	char* pBuffer1 = (char*)malloc(BLOCK_SIZE);
	char* pBuffer2 = (char*)malloc(BLOCK_SIZE);
	int cIndex = 0;
	int cmpCount = 0;
	int fd[4] = { 0, };
	ListDirContents("/home");
	MakeDir("/home/test");
	
	for (i = 0; i < 4; i++)
	{
		memset(fileName, 0, FILENAME_MAX_LEN);
		sprintf(fileName, "/home/test/file%d", i);
		fd[i] = OpenFile(fileName, OPEN_FLAG_CREATE);
	}
	ListDirContents("/home/test");
	for (i = 0; i < 18; i++)
	{
		for (j = 0; j < 4; j++)
		{
			char* str = (char*)malloc(BLOCK_SIZE);
			memset(str, 0, BLOCK_SIZE);
			for (k = 0; k < BLOCK_SIZE; k++)
				str[k] = alphabet[cIndex];
			WriteFile(fd[j], str, BLOCK_SIZE);
			cIndex++;
			free(str);
		}
	}
	printf("will close fileN...\n");
	for (i = 0; i < 4; i++)
		CloseFile(fd[i]);

	ListDirContents("/home/test");
	for (i = 0; i < 4; i++)
	{
		memset(fileName, 0, FILENAME_MAX_LEN);
		sprintf(fileName, "/home/test/file%d", i);
		fd[i] = OpenFile(fileName, OPEN_FLAG_READWRITE);
		printf("received fd[%d] = %d\n", i, fd[i]);
	}

	cIndex = 0;

	for (i = 0; i < 18; i++)
	{
		for (j = 0; j < 4; j++)
		{
			memset(pBuffer1, 0, BLOCK_SIZE);

			for (k = 0; k < BLOCK_SIZE; k++)
				pBuffer1[k] = alphabet[cIndex];

			memset(pBuffer2, 0, BLOCK_SIZE);
			ReadFile(fd[j], pBuffer2, BLOCK_SIZE);
			if (strcmp(pBuffer1, pBuffer2) == 0)
				cmpCount++;
			else
			{
				printf("TestCase 3 : error!!\n");
				exit(0);
			}
			cIndex++;
		}
	}
	if (cmpCount == 72)
		printf("TestCase3 : Complete!!!\n");
}

void TestCase4(void)
{
	int i;
	int fd;
	char fileName[FILENAME_MAX_LEN];
	char pBuffer[1024];

	printf(" ---- Test Case 4 ----\n");
	for (i = 0; i < 7; i++)
	{
		if (i % 2 == 0)
		{
			memset(fileName, 0, FILENAME_MAX_LEN);
			sprintf(fileName, "/home/user6/file%d", i);
			RemoveFile(fileName);
		}
	}
	printf(" ---- Test Case 4: files of even number removed ----\n");

	for (i = 0; i < 7; i++)
	{
		if (i % 2)
		{
			memset(fileName, 0, FILENAME_MAX_LEN);
			sprintf(fileName, "/home/user6/file%d", i);
			fd = OpenFile(fileName, OPEN_FLAG_READWRITE);

			memset(pBuffer, 0, 1024);
			strcpy(pBuffer, fileName);
			WriteFile(fd, pBuffer, 513);
			CloseFile(fd);
		}
	}
	printf(" ---- Test Case 4: files of odd number overwritten ----\n");

	ListDirContents("/home/user6");

	for (i = 0; i < 7; i++)
	{
		if (i % 2 == 0)
		{
			memset(fileName, 0, FILENAME_MAX_LEN);
			sprintf(fileName, "/home/user6/file%d", i);
			fd = OpenFile(fileName, OPEN_FLAG_CREATE);

			memset(pBuffer, 0, 1024);
			strcpy(pBuffer, fileName);
			WriteFile(fd, pBuffer, 513);
			WriteFile(fd, pBuffer, 513);
			CloseFile(fd);
		}
	}
	printf(" ---- Test Case 4: files of even number re-created & written ----\n");

	ListDirContents("/home/user6");
}
int main(int argc, char** argv)
{
	int TcNum;
	if (argc < 3)
	{
	ERROR:
		printf("usage: a.out [format | readwrite] [1-4])\n");
		return -1;
	}
	if (strcmp(argv[1], "format") == 0)
		Mount(MT_TYPE_FORMAT);
	else if (strcmp(argv[1], "readwrite") == 0)
		Mount(MT_TYPE_READWRITE);
	else
		goto ERROR;

	TcNum = atoi(argv[2]);


	switch (TcNum)
	{
	case 1:
		TestCase1();
		PrintInodeBitmap(); PrintBlockBitmap();
		break;
	case 2:
		TestCase2();
		PrintInodeBitmap(); PrintBlockBitmap();
		break;
	case 3:
		TestCase3();
		PrintInodeBitmap(); PrintBlockBitmap();
		break;
	case 4:
		TestCase4();
		PrintInodeBitmap(); PrintBlockBitmap();
		break;

	default:
		Unmount();
		goto ERROR;
	}
	Unmount();


	return 0;
}