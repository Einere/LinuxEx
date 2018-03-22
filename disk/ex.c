#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
	char* ptr = (char*)malloc(4);
//	memset(ptr, 0, 4);
	for(int i=3; i>=0; i--){
		printf("%d...\n", *ptr >> i & 1 );
	}
	return 0;
}
