#include "Matrix.h"
#include <stdlib.h>

void initMatrix(Matrix* m, size_t initCapa){
	m->array = (char**)malloc(initCapa * sizeof(char));
	m->used = 0;
	m->capa = initCapa;
}

void insertMatrix(Matrix* m, char* element){
	if(m->used == m->capa){
		m->capa *= 2;
		m->array = (char**)realloc(m->array, m->capa * sizeof(char));
	}
	m->array[m->used++] = element;
}

void freeMatrix(Matrix* m){
	for(int i = 0; i < m->capa; i++) free(m->array[i]);
	free(m->array);
	m->array = NULL;
	m->used = m->capa = 0;
}

