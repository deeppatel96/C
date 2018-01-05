#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "second.h"

int main(int argc, const char *argv[]){

	if(argc < 2){
		printf("error\n");
		return 0;
	}

	char *buff = (char*) argv[1];
	int num = atoi(buff);

	for(int i = num-1; i > 1; i--){
		if(num%i == 0){
			printf("no\n");
			return 0;
		}
	}

	printf("yes\n");
	return 0;

}

