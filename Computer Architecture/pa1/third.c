#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "third.h"

int main(int argc, const char *argv[]){


	FILE *fp;
	fp = fopen(argv[1],"r");

	if(fp == NULL){
		printf("error\n");
		return 1;
	}

	int length = 0;

	//start reading from file & check if file is empty
	char buff[256];
	if(fscanf(fp, "%s", buff) == EOF){
		printf("\n");
		return 0;
	}

	//Initialize a linked list
	Node *head = (Node*) malloc(sizeof(Node));
	head->next = NULL;
	Node *ptr = head;

	//Begin loop
	do{

		// If insert
		if (buff[0] == 'i'){
			fscanf(fp, "%s", buff);
			int len = strlen(buff);

			// check if input is a number
			for (int i = 0; i < len; i++){
       			if (!isdigit(buff[i]))
        		{
            		printf("error\n");
            		return 0;
        		}
			}
			int num = atoi(buff);

			ptr = head;
			// if LL is empty
			if(length == 0){
				head->val = num;
				length++;
				continue;
			}

			while(ptr != NULL){
				// inserting before pointer but not at end
				if(num < ptr->val && ptr->next != NULL){
					Node *temp = ptr->next;
					ptr->next = (Node*) malloc(sizeof(Node));
					ptr->next->next = temp;
					ptr->next->val = ptr->val;
					ptr->val = num;
					length++;
					break;
				}
				// inserting before last
				else if(num < ptr->val && ptr->next == NULL){
					ptr->next = (Node*) malloc(sizeof(Node));
					ptr->next->next = NULL;
					ptr->next->val = ptr->val;
					ptr->val = num;
					length++;
					break;
				}
				// if duplicate
				else if (ptr->val == num){
					break;
				}
				// inserting to end of LL
				else if(num > ptr->val  && ptr->next == NULL){
					ptr->next = (Node*) malloc(sizeof(Node));
					ptr->next->val = num;
					ptr->next->next = NULL;
					length++;
					break;
				}
				ptr = ptr->next;

			} // end insert loop
		} // end insert if

		// if delete
		else if (buff[0] == 'd'){

			fscanf(fp, "%s", buff);
			int len = strlen(buff);
			// check if input is a number
			for (int i = 0; i < len; i++){
       			if (!isdigit(buff[i]))
        		{
            		printf("error\n");
            		return 0;
        		}
			}
			int num = atoi(buff);

			ptr = head;

			while(ptr != NULL){

				// if LL if empty
				if(length == 0){
					break;
				}
				// delete head when no other nodes
				else if(ptr == head && num == ptr->val && ptr->next == NULL){
					ptr->val = 0;
					length--;
					break;
				}
				// delete head 
				else if(ptr == head && num == ptr->val){
					head = ptr->next;
					ptr = head;
					length--;
					continue;
				}
				// if pointer is last element
				else if(ptr->next == NULL && num != ptr->val){
					break;
				}
				// delete last node
				else if (ptr->next->val == num && ptr->next->next == NULL){
					ptr->next = NULL;
					length--;
					break;
				}
				// delete node
				else if (ptr->next->val == num){
					ptr->next = ptr->next->next;
					length--;
					continue;
				}
				ptr = ptr->next;

			} //end delete loop
		} //end delete if

		else{
			printf("error5\n");
			return 1;
		}

	}while(fscanf(fp,"%s",buff) != EOF); // end outer loop
    
    // Close the file
    fclose(fp);

	if(length > 0){
		for(ptr = head; ptr!= NULL; ptr = ptr->next){
			printf("%d\t",ptr->val);
		}
	}
	printf("\n");

	return 0;

}

