#include "fs.h"
#include "Disk.h"
#include "Matrix.h"
#include <stdio.h>

int main(){
	Mount(MT_TYPE_FORMAT);
	
	int a = MakeDir("/hi");
	
	int b = MakeDir("/hi/bye");
	printf("MakeDir(/hi) retrun %d, MakeDir(/hi/bye) return %d\n", a, b);

	printf("----------------------------------------\n");
	int c = OpenFile("/aa", OPEN_FLAG_CREATE);
	printf("OpenFile(/aa) return %d\n",c);

	int d = OpenFile("/hi/bb", OPEN_FLAG_CREATE);
	printf("OpenFile(/hi/bb) return %d\n", d);

	int e = OpenFile("/hi/bye/cc", OPEN_FLAG_CREATE);
	printf("OpenFile(/hi/bye/cc) return %d\n", e);
}
