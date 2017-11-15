#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
	char *p1 = malloc(10);
	char *p2 = malloc(10);

	strcpy(p1, "test");
	
	printf("before realloc : %p\n", p1);
	p1 = realloc(p1, 20);

	printf("after realloc : %p\n", p1);

	strcpy(p1 + strlen(p1), ", appended text");

	printf("%s",p1);

	free(p1);
	free(p2);

	return 0;
}
