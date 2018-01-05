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
	int j;

	while(i <= len-3){
		if(buff[i] == buff[i+1] && buff[i] == buff[i+2]){
			for(j = i; j < len-3; j++){
				buff[j] = buff[j+3];
			}
			len -= 3;
			buff[len] = '\0';
			if(i < 3){
				i = 0;
			}
			else{
				i -= 3;
			}
		}
		else{
			i++;
		}
	}

	printf("output: %s\n", buff);


	return 0;

}



