#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h> /* memset */

int main(){

    unsigned int len_max = 128;
    unsigned int current_size = 0;
    
    char *pStr = malloc(len_max);
    current_size = len_max;

    printf("Enter a String of words to be sorted: \n");

    if(pStr != NULL){
	int c = EOF;
	unsigned int i = 0;

    // Accept user input until hit enter or end of file
	while (( c = getchar() ) != '\n' && c != EOF){
		pStr[i++] = (char) c;

		// If maximize size is reached then realloc size
		if(i == current_size){
            current_size = i+len_max;
			pStr = realloc(pStr, current_size);
		}
	}

	printf("%c",&pStr);

        printf("\nLong String value:%s \n\n",pStr);
        //free it 
	free(pStr);
	pStr = NULL;


    }
    return 0;
}