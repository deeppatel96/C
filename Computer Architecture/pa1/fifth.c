#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "fifth.h"

int main(int argc, const char *argv[]){


	FILE *fp;
	fp = fopen(argv[1],"r");

	if(fp == NULL){
		printf("error\n");
		return 1;
	}

	//start reading from file
	char buff[256];
	fscanf(fp, "%s", buff);
	int m = atoi(buff);
	fscanf(fp, "%s", buff);
	int n = atoi(buff);

	int **A = malloc(m*sizeof(int*));
	int **B = malloc(m*sizeof(int*));
	for (int i = 0; i < m; i++) {
  		A[i] = malloc(n*sizeof(int));
  		B[i] = malloc(n*sizeof(int));

	}


	for(int i = 0; i<m; i++){
		for(int j = 0; j<n; j++){
			fscanf(fp, "%s", buff);
			int x = atoi(buff);
			A[i][j] = x;
		}
	}

	for(int i = 0; i<m; i++){
		for(int j = 0; j<n; j++){
			fscanf(fp, "%s", buff);
			int x = atoi(buff);
			B[i][j] = x;
		}
	}

	for(int i = 0; i<m; i++){
		for(int j = 0; j<n; j++){
			B[i][j] = A[i][j]+B[i][j];
			printf("%d\t",B[i][j]);
		}
		printf("\n");
	}

	free(A);
	free(B);

	return 0;

}

