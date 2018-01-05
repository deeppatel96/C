#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct Node{

	char text[256];
	struct Node *next;

}Node;

int main(){

	char input[256];
	printf("Type a String\n");
	scanf("%[^\n]s" , input);
	printf("--------------------------------------\n");


	printf("The input address is: %p\n" , input);
	printf("The input address + 1 is: %p\n" , input + 1);
	char* front = input;
	char* back = input + 1;

	Node* head = (Node*)malloc(sizeof(Node));
	Node* tmp = head;
	tmp->next = tmp;	

	printf("----------------------------------\n");
	printf("The address of head is: %p\n" , head);
	printf("The address of tmp is: %p\n" , tmp);	

	
	
	while(*back != '\0'){
	
	while(isalpha(*back)){
	back++;
	}

	int i = 0;
	while(front!=back){
	tmp->text[i] = *front;
	i++;
	front++;
	
}tmp->text[i+1] = '\0';

	tmp->next = (Node*)malloc(sizeof(Node));
	tmp = tmp->next;

	back++;
	front++;
}	
	tmp = NULL;
	Node* tmp1 = head;
	
	int j;
	while(tmp1!=NULL){
	j = 0;
	while(tmp1->text[j] != '\0'){
	printf("%c",tmp1->text[j]);
	j++;
	
}
	tmp1 = tmp1->next;

	printf("\n");

}	
		

	 
	return 0;


}
