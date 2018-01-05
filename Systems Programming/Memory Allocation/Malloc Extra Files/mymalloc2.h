#include <stdio.h>
#include <stddef.h>

static char myblock[10000];

typedef struct Node{

	size_t size;
	int free;
	struct Node* next;

}Node;

Node* memList = (void*)myblock;

void initialize();
void split(Node* freeNode,size_t size);
void* myMalloc(size_t byteInput);
void merge();

void MyFree(void* ptr);
