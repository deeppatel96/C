#ifndef __MYMALLOC_H__
#define __MYMALLOC_H__

#define malloc( x ) mymalloc( x, __FILE__, __LINE__ )
#define free( x ) myfree( x, __FILE__, __LINE__ )


// initialize block of memory to simulate main memory
static char myblock[5000];

// declare static sizes so the code is portable on different OS's
static short overheadSize = sizeof(short);



void initialize();
short getSize(char*);
void setSize(short, short);
short isAlloc(short);
void* mymalloc(size_t, char*, int lineNum);
void myMerge();
void myfree(void* ptr, char*, int lineNum);
void printList();



#endif // __MYMALLOC_H__