#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h> /* memset */


/* Execution will start here */
int main()
{

	char input[81];
    char *ptr;

	printf("Enter string: ");
    ptr = gets(input);
    printf("%s\n", input, ptr);


	int len;				//determine length of the input array
	int i = 0;              // initializing a counter

	return 0;
}

//Defining linked list
struct Node {
	int x;
	struct Node *next;
};



