#include "fs.h"
#include "disk.h"
#include "Matrix.h"
#include <stdio.h>
#include <stdlib.h>
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

	int f = CloseFile(d);
	printf("CloseFile(d) return %d\n", f);

	int g = WriteFile(c, "hello", 5);
	printf("WriteFile(c, \"hello\", 5) return %d\n", g);
	
	char* buffer = (char*)malloc(10);
	int h = ReadFile(c, buffer, 5);
	printf("ReadFile(c, %s, 5) return %d\n", buffer, h);

	int i = RemoveFile("/aa");
	printf("RemoveFile(/aa) return %d\n", i);

	printf("RemoveFile(/hi/bb) return %d\n", RemoveFile("/aa"));
}
