#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int main(int argc, const char *argv[]){

	if(argc < 2){
		printf("error\n");
		return 0;
	}

	char *buff = (char*) argv[1];

	printf("input:  %s\n", buff);
	int len = strlen(buff);
	printf("length: %d\n", len);

	int i = 0;
	char temp;

	for(i = 0; i < len/2; i++){
		temp = buff[i];
		buff[i] = buff[len-i-1];
		buff[len-i-1] = temp;
	}



	printf("output: %s\n", buff);


	return 0;

}



