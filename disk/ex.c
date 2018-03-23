#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_SIZE 4

void setbit(char* ptr, int bit_index);

int main(){
	char* ptr = (char*)malloc(MAX_SIZE);
	memset(ptr, 0, MAX_SIZE);
	setbit(ptr, 10);
	
	for(int i = MAX_SIZE - 1; i >=0; i--){
		for(int j = 0; j < 8; j++){
			printf("%d", (ptr[i] >> j) & 1);
		}
		printf(" ");
	}
	
	printf("\n");	
	return 0;
}

void setbit(char* ptr, int bit_index){
	int byte_index = bit_index / 8;
	int shift_num = bit_index % 8;
	printf("byte_index = %d, shift_num = %d\n", byte_index, shift_num);	
	*(ptr + byte_index) |= 1 << (8 - 1 - shift_num);
	//*(ptr + byte_index) |= 1 << shift_num;
}
