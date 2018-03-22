#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(){
	int* ptr = (int *)malloc(4);
	memset(ptr, 1, 4);
	printf("%d...\n", *ptr);
	
	return 0;
}
