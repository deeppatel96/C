#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
// #include "mymalloc.h"

// Linked List node definition
typedef struct Block {
	
	void *ptr;				// pointer to start of block
	size_t size;			// size of block
	int isFree;				// true = 1, false = 0
	struct Block *next;		// pointer to next block of memory

} Block;



// initialize block of memory to simulate main memory
static char myblock[5000];

// Declare global variable which is the head pointer of the Linked List
Block *head;


// Initialize Linked List as 1 block of memory
// Parameters: character array which represents main memory
// Return: pointer of type Block which points to the head of the linked list
void initialize(char block[]){

	head = (Block*) malloc(sizeof(Block));
	head->ptr =  block;
	head->size = sizeof(myblock);
	head->isFree = 1;
	head->next = NULL;

}

// Input: size of memory block to allocate
// Return: void pointer to starting of memory block
void* myMalloc(size_t memSize, char file[], int lineNum){

	// if memory size requested is greater than total memory size, return NULL
	if(memSize > sizeof(myblock)){
		printf("Error in %s Line %d: Not enough memory.\n",file,lineNum);
		return NULL;
	}

	Block *curr = head;

	while(curr != NULL){

		if(curr->size == memSize && curr->isFree == 1){

			curr->isFree = 0;
			return curr->ptr;

		}
		else if(curr->size > memSize && curr->isFree == 1){
			
			Block *new = (Block*) malloc(sizeof(Block));
			new->ptr = curr->ptr + memSize;
			new->next = curr->next;
			new->isFree = 1;
			curr->isFree = 0;
			new->size = curr->size - memSize;
			curr->size = memSize;
			curr->next = new;

			return curr->ptr;

		}
		else{
			curr = curr->next;
		}

	} // end while loop

	printf("Error in %s Line %d: Not enough memory.\n",file,lineNum);
	return NULL;


}

void myMerge(){

	Block *prev = head;
	Block *curr = head->next;

	// merge free blocks
	while(curr != NULL){

		if(prev->isFree == 1 && curr->isFree ==1){
			prev->size = prev->size + curr->size;
			prev->next = curr->next;
			free(curr);
			curr = prev->next;
		}
		else{
			prev = curr;
			curr = curr->next;
		}

	}

}


// Input: void pointer to variable initialized by myMalloc
// Return: nothing
void myFree(void* ptr, char file[], int lineNum){

	if( ptr == NULL){

		printf("Error in %s Line %d: Cannot Free NULL pointer.\n",file,lineNum);
		return;

	}
	else if( ptr < head->ptr || ptr > head->ptr + sizeof(myblock)){
		printf("Error in %s Line %d: Memory not allocated by malloc.\n",file,lineNum);
		return;
	}

	Block *curr = head;

	while(curr != NULL){

		if(ptr == curr->ptr && curr->isFree == 0){
			curr->isFree = 1;
			myMerge();
			return;
		}
		else{
			curr = curr->next;
		}

	} // end while loop


	// If pointer is not found in Linked List
	printf("Error in %s Line %d: Memory not found.\n",file,lineNum);
	return;
}



int main(int argc, const char *argv[]){

	initialize(myblock);


	// char *a = (char*) myMalloc(500, __FILE__, __LINE__);
	// char *b = (char*) myMalloc(500, __FILE__, __LINE__);
	// char *c = (char*) myMalloc(500, __FILE__, __LINE__);
	// char *d = (char*) myMalloc(500, __FILE__, __LINE__);
	// char *e = (char*) myMalloc(500, __FILE__, __LINE__);
	// char *f = (char*) myMalloc(200, __FILE__, __LINE__);
	// myFree(b, __FILE__, __LINE__);
	// myFree(c, __FILE__, __LINE__);
	// myFree(e, __FILE__, __LINE__);
	// b = (char*) myMalloc(200, __FILE__, __LINE__);
	// myFree(d, __FILE__, __LINE__);
	// c = (char*) myMalloc(1000, __FILE__, __LINE__);

	// int x;
	// myFree(&x,__FILE__,__LINE__);

	// int *p;
	// myFree(p,__FILE__,__LINE__);

	// myFree(a,__FILE__,__LINE__);
	// a = (char*) myMalloc(500, __FILE__, __LINE__);
	// myFree(a,__FILE__,__LINE__);

	// char *q = (char*) myMalloc(4000, __FILE__, __LINE__);


	// Initialize time breakpoints
	struct timeval t1;
	struct timeval t2;
	struct timeval t3;
	struct timeval t4;
	struct timeval t5;
	struct timeval t6;
	struct timeval t7;


	// Workload A: 
	// malloc 1 byte 3000 times, then free 3000 1 byte pointers
	gettimeofday(&t1, NULL);	// Get start time for Workload A
	
	char *a[3000];
	for(int i = 0; i < 3000; i++){
		a[i] = (char*) myMalloc(1,__FILE__,__LINE__);
	}

	for(int i = 0; i < 3000; i++){
		myFree(a[i],__FILE__,__LINE__);
	}



	// // Workload B: 
	// // malloc 1 byte & immediately free it 3000 times in row
	// gettimeofday(&t2, NULL);	// Get end time of Workload A & start time for Workload B
	
	// char *b;
	// for(int i = 0; i < 3000; i++){
	// 	b = (char*) myMalloc(1,__FILE__,__LINE__);
	// 	myFree(b,__FILE__,__LINE__);
	// }



	// // Workload C: 
	// // randomly choose between (1 byte malloc) or (free 6000 times)
	// gettimeofday(&t3, NULL);	// Get end time of Workload B & start time for Workload C
	
	// int round = 0;
	// int count = 0;
	// int lastFreed = 0;
	// int n;
	// srand(time(NULL));
	// char *c[6000];

	// while(round < 6000){
	// 	n = rand() % 2;
	// 	if(n == 0){
	// 		c[count] = (char*) myMalloc(1,__FILE__,__LINE__);
	// 		count++;
	// 	}
	// 	else{
	// 		myFree(c[lastFreed],__FILE__,__LINE__);
	// 		lastFreed++;
	// 	}
	// 	round++;
	// }

	// for(lastFreed; i < 6000; i++){
	// 	myFree(c[i],__FILE__,__LINE__);
	// }

	// // Workload D: 
	// randomly choose between (a randomly sized malloc) or (free 6000 times)
	gettimeofday(&t4, NULL);	//Get end time of Workload C & start time for Workload D
	
	// int count = 0;
	// int n;
	// srand(time(NULL));
	// char *c[3000];

	// while(count<3000){
	// 	n = rand() % 2;
	// 	if(n == 0){
	// 		c[count] = (char*) myMalloc(rand(),__FILE__,__LINE__);
	// 		while(c[count] == NULL){
	// 			c[count] = (char*) myMalloc(rand(),__FILE__,__LINE__);
	// 		}
	// 		count++;
	// 	}
	// 	else{

	// 		myFree(c[],__FILE__,__LINE__);

	// 	}
	// }

	// for(int i = 0; i < 3000; i++){
	// 	myFree(c[i],__FILE__,__LINE__);
	// }



	// Workload E
	gettimeofday(&t5, NULL);	// Get end time of Workload D & start time for Workload E



	// Workload F
	gettimeofday(&t6, NULL);	// Get end time of Workload E & start time for Workload F












	gettimeofday(&t7, NULL);

	unsigned int t_a = t2.tv_usec - t1.tv_usec;
	unsigned int t_b = t3.tv_usec - t2.tv_usec;
	unsigned int t_c = t4.tv_usec - t3.tv_usec;
	unsigned int t_d = t5.tv_usec - t4.tv_usec;
	unsigned int t_e = t6.tv_usec - t5.tv_usec;
	unsigned int t_f = t7.tv_usec - t6.tv_usec;

	printf("Run Time Workload A: %d us\n", t_a);
	printf("Run Time Workload B: %d us\n", t_b);
	printf("Run Time Workload C: %d us\n", t_c);
	printf("Run Time Workload D: %d us\n", t_d);
	printf("Run Time Workload E: %d us\n", t_e);
	printf("Run Time Workload F: %d us\n", t_f);




	printf("\n");
	Block *ptr = head;
	int i = 1;
	while(ptr != NULL){
		printf("Size of block %d: %zu\n", i, ptr->size);
		printf("Is the block free: %d\n",ptr->isFree);
		ptr = ptr->next;
		i++;
	}


	ptr = head;
	Block *after;
	while(ptr != NULL){
		after = ptr->next;
		free(ptr);
		ptr = after;
	}


	   // printf("File :%s\n", __FILE__ );
	   // printf("Date :%s\n", __DATE__ );
	   // printf("Time :%s\n", __TIME__ );
	   // printf("Line :%d\n", __LINE__ );
	   // printf("ANSI :%d\n", __STDC__ );


	return 0;


}