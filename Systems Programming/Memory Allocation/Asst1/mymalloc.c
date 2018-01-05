#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "mymalloc.h"



// Initializes Block of memory as 1 free block
void initialize(){

	short *size = (short*) (&myblock[0]);	// creates short pointer to first element of block
	*size = sizeof(myblock)-overheadSize;	// assigns short value of size to the beginning (most likely 2 bytes)

}

// Inputs char pointer to start of metadata
// Outputs size at the location
short getSize(char *metaStart){
	short *ptr = (short*) metaStart;
	return *ptr;
}
// Takes in a size and an array index location and sets the size in that location accordingly
void setSize(short size, short index){
	char *start = &myblock[index];
	short *ptr = (short*) start;
	*ptr = size;
}
// Checks if size is flagged as free or allocated
// Returns 0 if free or 1 if allocated
short isAlloc(short size){
	size = size & 1;
	return size;
}

// Input: size of memory block to allocate
// Return: void pointer to starting of memory block
void* mymalloc(size_t memSize, char *file, int lineNum){

	// If memory size requested is greater than total memory size, return NULL
	if(memSize > sizeof(myblock)){
		printf("%s:%d: Error: Memory request greater than size of total memory.\n",file,lineNum);
		return NULL;
	}

	// To keep track of current byte in block
	short currByte = 0;

	// Get even malloc size if odd
	if((memSize & 1) != 0){
		memSize++;
	}

	// make a char pointer to start of block & gets size of 1st block
	char *cptr = myblock;
	short blockSize = getSize(cptr);


	// While currByte is less than the size of the block
	while(currByte < sizeof(myblock)){

		// if block is free & block size is equal to malloc size
		if(!(isAlloc(blockSize))){

			if(blockSize >= memSize && blockSize < (memSize + 2 + overheadSize)){
			setSize(blockSize+1,currByte);
			void* ptr = &myblock[currByte+overheadSize];
			return ptr;

			}
			else if(blockSize > memSize){

				short *split = (short*) &myblock[currByte+overheadSize+memSize];
				*split = blockSize - memSize - overheadSize;

				setSize(memSize+1,currByte);
				
				void* ptr = &myblock[currByte+overheadSize];
				return ptr;
			}
			// if the free block is too small go to next loop
			else{
				currByte = currByte + overheadSize + blockSize;
				cptr = &myblock[currByte];
				blockSize = getSize(cptr);

			}

		}
		else{
			short realSize = blockSize & ~1;
			currByte = currByte + overheadSize + realSize;
			cptr = &myblock[currByte];
			blockSize = getSize(cptr);

		}

	}

	return NULL;

}


void myMerge(){

	// To keep track of current byte in block
	short currByte = 0;

	// make a char pointer to start of block & gets size of 1st block
	char *cptr = myblock;
	short blockSize = getSize(cptr);


	// While currByte is less than the size of the block
	while(currByte < sizeof(myblock)){

		// if block is free
		if(!(isAlloc(blockSize))){
			short nextByte = currByte + overheadSize + blockSize;

			//if next block is out of bounds, reached last block so exit
			if(nextByte >= sizeof(myblock)){
				return;
			}
			char *nptr = &myblock[nextByte];
			short nextBlockSize = getSize(nptr);

			// if next block is free
			if(!(isAlloc(nextBlockSize))){
				setSize(blockSize+nextBlockSize+overheadSize,currByte);
				*nptr = 0;
				blockSize = getSize(cptr);
				continue;

			}

			// go to next block regardless
			currByte = currByte + overheadSize + blockSize;
			cptr = &myblock[currByte];
			blockSize = getSize(cptr);
	
		}
		// if block is allocated go to next block
		else{
			short realSize = blockSize & ~1;
			currByte = currByte + overheadSize + realSize;
			cptr = &myblock[currByte];
			blockSize = getSize(cptr);
		}

	}

	return;

}


// Input: void pointer to variable initialized by mymalloc
// Return: nothing
void myfree(void* ptr, char *file, int lineNum){

	if(ptr == NULL){

		printf("%s:%d: Error: Cannot Free NULL pointer.\n",file,lineNum);
		return;

	}
	char *check = (char*) ptr;
	if( check < myblock || check > (myblock + sizeof(myblock))){
		printf("%s:%d: Error: Memory not allocated by malloc.\n",file,lineNum);
		return;
	}

	// To keep track of current byte in block
	short currByte = 0;

	// make a char pointer to start of block & gets size of 1st block
	char *cptr = myblock;
	short blockSize = getSize(cptr);


	// While currByte is less than the size of the block
	while(currByte < sizeof(myblock)){

		// if block is allocated
		if((isAlloc(blockSize))){

			if((cptr + overheadSize) == ptr){

				setSize(blockSize-1,currByte);
				myMerge();
				return;

			}

			// if it is not the right block, go to next block
			else{
				short realSize = blockSize & ~1;
				currByte = currByte + overheadSize + realSize;
				cptr = &myblock[currByte];
				blockSize = getSize(cptr);

			}

		}
		// if block is free go to next block
		else{
			currByte = currByte + overheadSize + blockSize;
			cptr = &myblock[currByte];
			blockSize = getSize(cptr);

		}

	}


	printf("%s:%d: Error: Memory not allocated by malloc.\n",file,lineNum);
	
	return;

}

void printList(){

	printf("\n");

	// To keep track of current byte in block
	short currByte = 0;

	// make a char pointer to start of block & gets size of 1st block
	char *cptr = myblock;
	short blockSize = getSize(cptr);

	int i = 0;


	// While currByte is less than the size of the block
	while(currByte < sizeof(myblock)){

		printf("Block %d: isAlloc: %d ",i,isAlloc(blockSize));
		
		if((isAlloc(blockSize))){

			short realSize = blockSize & ~1;
			printf("Size: %d\n",realSize);
			currByte = currByte + overheadSize + realSize;
			cptr = &myblock[currByte];
			blockSize = getSize(cptr);

		}

		else{
			printf("Size: %d\n",blockSize);
			currByte = currByte + overheadSize + blockSize;
			cptr = &myblock[currByte];
			blockSize = getSize(cptr);
		}
		i++;

	}
	printf("\n");

}

