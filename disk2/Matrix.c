#include "Matrix.h"
#include <stdlib.h>

void initMatrix(Matrix* m, size_t initCapa){
	m->array = (char**)malloc(initCapa * sizeof(char*));
	memset(m->array, 0, initCapa * sizeof(char*));
	m->used = 0;
	m->capa = initCapa;
}

void insertMatrix(Matrix* m, char* element){
	if(m->used == m->capa){
		m->capa *= 2;
		m->array = (char**)realloc(m->array, m->capa * sizeof(char*));
	}
	m->array[m->used] = (char*)malloc(sizeof(element) + 1);
	memset(m->array[m->used], 0, sizeof(element) + 1);
	strcpy(m->array[m->used++], element);
}

void printMatrix(Matrix* m){
	for(int i = 0; i < m->capa; i++){
		printf("printMatrix...m->array[%d] = %s\n", i, m->array[i]);
	}
}

void freeMatrix(Matrix* m){
	for(int i = 0; i < m->capa; i++){
		if(m->array[i] != NULL) free(m->array[i]);
	} 
	free(m->array);
	m->array = NULL;
	m->used = m->capa = 0;
}

