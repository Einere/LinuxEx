#include <stdio.h>
#include <string.h>

typedef struct{
	char** array;
	size_t used;
	size_t capa;
} Matrix;

void initMatrix(Matrix* m, size_t initCapa);
void insertMatrix(Matrix* m, char* element);
void freeMatrix(Matrix* m);
