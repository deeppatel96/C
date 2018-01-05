#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h> /* memset */

//Defining linked list
typedef struct Node{
	char *word;
	struct Node *next;

} Node;

/* Execution will start here */
int main()
{
	unsigned int len_max = 128;			//define original max length
    unsigned int current_size = 0;		//define current size
    
    char *pStr = malloc(len_max);		//initialize pointer of size of max length
    current_size = len_max;				//set current size equal to max length

    printf("Enter a String of words to be sorted: \n"); //ask for string input
	unsigned int i = 0;

    if(pStr != NULL){
		int c = EOF;

    	// Accept user input until hit enter or end of file
		while (( c = getchar() ) != '\n' && c != EOF){
			pStr[i++] = (char) c;

			// If max size is reached then realloc size to dynamically allocate memory
			if(i == current_size){
         	   	current_size = i+len_max;
				pStr = realloc(pStr, current_size);
			}
		}
		// Null-terminate the string
		pStr[i] = '\0';
	}

	// if no string is entered
	if (i == 0){
		return 0;
	} // if a string of 1 character is entered
	else if(i==1){
		printf("%s\n",pStr);
		return 0;
	}

	char *front = &pStr[0];
	char *back = &pStr[1];
	// initialize an empty circular linked list
	Node *tmp = (Node*) malloc(sizeof(Node));
	Node *head = tmp;
	printf("\n");

	while(*back != '\0'){

		if(isalpha(*back)){
		 	back++;
		} 
		else if(!isalpha(*back) && front!=back){
			char *w = malloc(sizeof(char)*(back-front));
			memcpy(w,front,sizeof(char)*(back-front));
			tmp->word = w;
			tmp->next = (Node*) malloc(sizeof(Node));
			tmp = tmp->next;
			back++;
			front = back;
		}
		else{
			front++;
			back++;
		}
	}
			// Makes last Node and attaches end of linked list to head
			char *w = malloc(sizeof(char)*(back-front));
			memcpy(w,front,sizeof(char)*(back-front));
			tmp->word = w;
			Node *tail = tmp;
			tmp->next = head;


	// Assign pointers for sorting
	Node *curr_min = head;
	tmp = curr_min->next;
	Node *last_sorted = tail;
	Node *last_unsorted = head;
	Node *prev = tail;
	Node *prevptr = head;

	if(head->next == tail){
		if(strcmp(head,head->next) > 0){
			head = tail;
			tail = head->next;
		}
			//Prints in alphabetical order
		tmp = head->next;
		printf("%s\n",head->word);
		while(tmp!=head){
			printf("%s\n",tmp->word);
			tmp = tmp->next;
		}
	return 0;
	}

	while(last_unsorted->next != tail){
		while(tmp != head){
			if(strcmp(curr_min->word,tmp->word) == 0){
				curr_min = tmp;
				prev = prevptr;
			}
			else if(strcmp(curr_min->word,tmp->word) > 0){
				curr_min = tmp;
				prev = prevptr;
			}
			tmp = tmp->next;
			prevptr = prevptr->next;
		}

		if(curr_min == last_unsorted){
			//Move the sorted part over by 1 word & set pointers for next loop
			last_unsorted = last_unsorted->next;
			last_sorted = last_sorted->next;
			curr_min = last_unsorted;
			prev = last_sorted;
			prevptr = last_unsorted;
			tmp = curr_min->next;
		}	
		else{
			if (last_unsorted == head && curr_min == tail){
				// Switch positions of the 2 words
				prev->next = last_unsorted;
				Node *temp = last_unsorted->next;
				last_unsorted->next = curr_min;
				curr_min->next = temp;
				last_sorted = curr_min;
				tail = prev->next;
				head = tail->next;
			}

			else if(head == last_unsorted){
				prev->next = last_unsorted;
				Node *temp = last_unsorted->next;
				last_unsorted->next = curr_min->next;
				curr_min->next = temp;
				last_sorted->next = curr_min;
				head = curr_min;
			}

			else if (curr_min == tail){
				// Switch positions of the 2 words
				prev->next = last_unsorted;
				Node *temp = last_unsorted->next;
				last_unsorted->next = curr_min->next;
				curr_min->next = temp;
				last_sorted->next = curr_min;
				tail = last_unsorted;
			}

			else{
			// Switch positions of the 2 words
			prev->next = last_unsorted;
			Node *temp = last_unsorted->next;
			last_unsorted->next = curr_min->next;
			curr_min->next = temp;
			last_sorted->next = curr_min;
		}

			//Move the sorted part over by 1 word & set pointers for next loop
			last_unsorted = curr_min->next;
			last_sorted = curr_min;
			curr_min = curr_min->next;
			tmp = curr_min->next;
			prev = last_sorted;
			prevptr = last_unsorted;
		}

	}

	//Prints in alphabetical order
	tmp = head->next;
	printf("%s\n",head->word);
	while(tmp!=head){
		printf("%s\n",tmp->word);
		tmp = tmp->next;
	}


	return 0;

}
