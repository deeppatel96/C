#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>



void printMatrix(int** matrix, int m, int n){

   int i = 0;
   int j = 0;

   for(i = 0; i < m; i++){
      for(j = 0; j < n; j++){
         printf("%d ",matrix[i][j]);
      }
      printf("\n");

   }
}


int** matrixRevolve(int** matrix, int n, int m, int flag){

	int** newMatrix = (int**) malloc(sizeof(int*)*n);
   for(int i = 0; i < n; i++){
      newMatrix[i] = (int*) malloc(sizeof(int)*m);
   }

   if(flag == 0){
      for(int i = 0; i < m; i++){
         for(int j = 0; j < n; j++){
            int r = j;
            int c = m-i-1;
            newMatrix[r][c] = matrix[i][j];
         }
      }
   }
   else{
      for(int i = 0; i < m; i++){
         for(int j = 0; j < n; j++){
            newMatrix[n-j-1][i] = matrix[i][j];
         }
      }
   }

   printMatrix(newMatrix,n,m);

	return newMatrix;


}

int main(int argc, const char *argv[]){


   int n = 3;
   int m = 5;
   int **matrix = (int**) malloc(sizeof(int*)*m);
   for(int i = 0; i < m; i++){
      matrix[i] = (int*) malloc(sizeof(int)*n);
   }



   for(int i = 0; i < m; i++){
         for(int j = 0; j < n; j++){
            matrix[i][j] = i*j;
         }
   }

   int flag = 0;
   
   printf("\n");
   printMatrix(matrix,m,n);

   int** newMatrix = matrixRevolve(matrix, m, n, flag);

      printf("\n");

   // printMatrix(newMatrix,n,m);



   return 0;

}
