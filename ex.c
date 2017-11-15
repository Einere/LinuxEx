#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_matrix(char **p_matrix, int p_row, int p_column);

int main(){
	//input matrix's row and column
	int i = 0, j = 0;
	int row = 0, column = 0;
	printf("enter the matrix row and column\n");
	scanf("%d %d", &row, &column);
	
	//assign the memory for matrix and set the matrix with 1
	char **matrix = (char **)malloc(row * sizeof(char*));
	
	for(i=0; i<row; i++){	
		matrix[i] = (char *)malloc(column * sizeof(char));
		memset(matrix[i], 1, column* sizeof(char));
	}

	print_matrix(matrix, row, column);

	while(1){
		//input i,j to add 1
		i=0, j=0;
		printf("-----------------------\n");
		printf("enter the i, j to add 1\n");
		scanf("%d %d", &i, &j);
		fflush(stdout);

		//if input is out of mtrix index
		if(i>row || j>column){
			for(i=0; i<row; i++){
				free(matrix[i]);
			}
			free(matrix);
			break;
		}

		//add 1 at matrix[i,j]
		matrix[i][j] += 1;
		
		//if entry is bigger than 2
		if(matrix[i][j] > 2){
			matrix[i][j] = -2;
		}

		print_matrix(matrix, row, column);
	}

	return 0;
}

void print_matrix(char **p_matrix, int p_row, int p_column){
	//print defalt matrix
	int i=0, j=0;

	for(i=0; i<p_row ; i++){
		for(j=0; j<p_column; j++){
			printf("%2d ", p_matrix[i][j]);
		}
		printf("\n");
	}
	
}
