#include "fs.h"
#include "Disk.h"
#include "Matrix.h"
#include <stdio.h>

int main(){
	Mount(MT_TYPE_FORMAT);
	
	int a = MakeDir("/hi");
	
	int b = MakeDir("/hi/bye");
	printf("first free inode index : %d\nfirst free block index : %d\n", GetFreeInodeNum(), GetFreeBlockNum());
	printf("MakeDir(/hi) retrun %d, MakeDir(/hi/bye) return %d\n", a, b);

}
