#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int main(int argc, const char *argv[]){

	int arr[10] = {1,2,3,4,5,6,7,8,9,10};
	int tar = 1;
	int find = 0;

	int hi = sizeof(arr)/sizeof(int) -1;
	int lo = 0;
	int mid = hi/2;

	while(hi>=lo){
		if(arr[mid] == tar){
			find = 1;
			break;
		}
		else if(tar > arr[mid]){
			lo = mid+1;
			mid = (lo+hi)/2;
		}
		else{
			hi = mid-1;
			mid = (lo+hi)/2;
		}
	}

	printf("find: %d\n", find);


	return 0;

}



