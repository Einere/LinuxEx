#include <stdio.h>
#include <stdlib.h>

void func(int* ptr){
	*ptr = 10;
}

int main(){
	int a = 2;
	//func(&a);
	printf("%d\n",a);
}
