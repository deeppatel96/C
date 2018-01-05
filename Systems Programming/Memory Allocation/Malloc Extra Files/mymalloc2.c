#include <stdio.h>
#include <stddef.h>
#include "mymalloc2.h"


//Initialize LL. Points memList to the start of myblock.
void initialize(){
memList->size = 10000 - sizeof(Node);
memList->free=1;
memList->next=NULL;
}

//Takes a Node to split and size of new malloc. Splits a node into free and !free node. 
void split(Node* freeNode, size_t size){
Node* new = (void*)((void*)freeNode+size+sizeof(Node));		//Creates new free node from the input freeNode
new->size=(freeNode->size)-size-sizeof(Node);
new->free=1;
new->next=freeNode->next;		//New.next points to freeNode.next

freeNode->size = size;			//freeNode becomes !free node.
freeNode->free = 0;
freeNode->next = new;			//freeNode.next points to new
}


//====================Malloc Function==================== 
//Takes byte size to malloc.
void* myMalloc(size_t byteInput){

Node* curr;
Node* prev;
void* result;

//If the LL has not been initialized yet, initialize.



curr=memList;

while((((curr->size)<byteInput)||((curr->free)==0))&&(curr->next!=NULL)){	//Find a Node that has room for the malloc. Save node in curr and previous in prev.
prev=curr;
curr=curr->next;
}

if((curr->size)==byteInput){							//If the bytes are equal, no further action required.
curr->free=0;									//Set curr node to !free
result=(void*)(++curr);								
return result;
}
else if((curr->size)>(byteInput+sizeof(Node))){					//If the node has extra space we need to split.
split(curr, byteInput);								//Split curr
result=(void*)(++curr);								//Return Pointer to allocated memory
return result;
}
else{
result=NULL;									//If we reach here, we have the last node in the LL and there is no room for allocation.
printf("Not enough memory for allocation!!");
return result;									//Return Null

}
}

//Merge Function. After freeing a pointer, we must check to see if we can merge nodes.
void merge(){
Node* curr; 
Node* prev;

curr = memList;
while(curr!=NULL&&curr->next!=NULL){				//Cycle through LL
if((curr->free)&&(curr->next->free)){			//If two nodes are both free, merge.
curr->size += (curr->next->size)+sizeof(Node);		
curr->next=curr->next->next;
}
prev=curr;
curr=curr->next;
}
}


//Free Function. Takes ptr to free. Check to see if pointer is from MyMalloc. If yes then free, if no then print error.

void myFree(void* ptr){
if(((void*)myblock<=ptr)&&(ptr<=(void*)(myblock+10000))){	
Node* curr = ptr;
curr--;
curr->free=1;
merge(); 					
}
else printf("Please provide a vaild pointer allocated by MyMalloc\n");
}


//Print LL with fields. Takes in Head of LL
void printLL(Node* head){
Node* tmp = head;

while(tmp!=NULL){
printf("Free: %d\n",tmp->free);
printf("Address: %p\n", tmp);
printf("Size: %zd\n",tmp->size);
printf("-----\n");

tmp=tmp->next;
}
printf("--------------------------\n");
return;
}

int main(){

initialize();

Node* head = memList;

char* p = (char*)myMalloc(9800);
printLL(head);

}






