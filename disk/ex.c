#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_SIZE 4

void setbit(char* ptr, int bit_index);
void resetbit(char* ptr, int bit_index);
void printbit(char* ptr);
void find_first_zero(char* ptr);

int main(){
	char* ptr = (char*)malloc(MAX_SIZE);
	memset(ptr, 0, MAX_SIZE);

	for(int i=0; i < ; i++) setbit(ptr, i);
	
	find_first_zero(ptr);
	printbit(ptr);	
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

void resetbit(char* ptr, int bit_index){
	int byte_index = bit_index / 8;
	int shift_num = bit_index % 8;
	printf("byte_index = %d, shift_num = %d\n", byte_index, shift_num);	
	*(ptr + byte_index) &= ~(1 << (8 - 1 - shift_num));
	//*(ptr + byte_index) |= 1 << shift_num;
}

void printbit(char* ptr){
	for(int i = MAX_SIZE - 1; i >= 0; i--){
		for(int j = 0; j < 8; j++){
			printf("%d", (ptr[i] >> j) & 1);
		}
		printf(" ");
	}
}

void find_first_zero(char* ptr){
	for(int i = 0; i <= MAX_SIZE - 1; i++){
		if(ptr[i] == -1) continue;
		//printf("%d\n", ptr[i]);
		
		for(int j = 7; j >= 0; j--){
			//printf("%d", (ptr[i] >> j) & 1);
			if(!(ptr[i] >> j & 1)){
				//return i * 8 + (7 - j);
				printf("bit index = %d...\n", i*8 + (7-j));
				i = MAX_SIZE;
				break;
			}
		}
		//printf(" ");
	}

