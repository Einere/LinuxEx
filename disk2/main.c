#include "fs.h"
#include "Disk.h"
#include "Matrix.h"
#include <stdio.h>

int main(){
	Mount(MT_TYPE_FORMAT);
	
	printf("before MakeDir hi)))\nfirst free inode index : %d\nfirst free block index : %d\n", GetFreeInodeNum(), GetFreeBlockNum());
	int a = MakeDir("/hi");
	
	printf("after MakeDir hi)))\nfirst free inode index : %d\nfirst free block index : %d\n", GetFreeInodeNum(), GetFreeBlockNum());
	int b = MakeDir("/hi/bye");
	printf("after MakeDir hi/bye)))\nfirst free inode index : %d\nfirst free block index : %d\n", GetFreeInodeNum(), GetFreeBlockNum());
	printf("MakeDir(/hi) retrun %d, MakeDir(/hi/bye) return %d\n", a, b);

}
