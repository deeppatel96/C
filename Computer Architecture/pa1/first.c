#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "first.h"

int main(){

	int num;
	char data[100];
	fgets(data, 100, stdin);
	if(strlen(data) == 1){
		printf("error\n");
		return 1;
	}
	num = atoi(data);

	if(num%2 == 1){

		printf("odd\n");
	}
	else if (num%2 == 0){

		printf("even\n");
	}

	return 0;

}

