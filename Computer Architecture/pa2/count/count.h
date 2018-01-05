#ifndef __COUNT_H__
#define __COUNT_H__

// Linked List Node definition for chaining
typedef struct Node {
	long address;
	struct Node *next;
}Node;

// Hash table definition
typedef struct Hashtable{
	int capacity;
	int size;
	double loadFactor;
	struct Node *table;
}Hashtable;

void insert(Hashtable *ht, long decAddress);
int getSum(Hashtable *ht);
void rehash(Hashtable *ht);



#endif //__COUNT_H__