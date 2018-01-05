#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "count.h"

// inputs char array of hexidecimal input and ouputs decimal integer
// returns -1 if improper input
long hexToDec(char hex[]){

	// if the first 2 characters are not '0' or 'x' respectively, return a '-1' to improper input
	if (hex[0] != '0' || hex[1] != 'x'){
		return -1;
	}
	for(int i = 2; i < strlen(hex); i++){

		if(hex[i] < '0' || (hex[i] > '9' && hex[i] < 'A') || (hex[i] > 'F' && hex[i] < 'a') || hex[i] > 'f'){
			return -1;
		}

	}

	char * start = &hex[2];
	long dec = strtoul(start, NULL, 16);




	return dec;

}


void insert(Hashtable *ht, long decAddress){


	ht->loadFactor = (double) (ht->size)/(ht->capacity);
	if(ht->loadFactor > 0.5){
		rehash(ht);
	}

	long key = decAddress%(ht->capacity);
	Node *ptr = ((ht->table)+key);
	// printf("Key: %lu\n", key);


	while(1){

		// if address is -1 (not initialized) then set it as this address
		if(ptr->address == -1){
			// printf("Not initialized\n");
			ptr->address = decAddress;
			ht->size++;
			return;
		}

		// if address is already there then do nothing and exit
		else if(ptr->address == decAddress){
			// printf("Already there\n");
			return;
		}

		// else if this is the last node
		else if(ptr->next == NULL){
			// printf("Not there, add to the hash table\n");
			ptr->next = (Node*) malloc(sizeof(Node));
			ptr = ptr->next;
			ptr->address = decAddress;
			ptr->next = NULL;
			ht->size++;
			return;
		}

		// otherwise go to next Node (address)
		else{
			// printf("Go to next node\n");
			ptr = ptr->next;
		}
	}

	return;

}

void rehash(Hashtable *ht){

	int oldCapacity = ht->capacity;
	int newCapacity = oldCapacity * 2;

	ht->capacity = newCapacity;
	ht->loadFactor = (double) (ht->size)/(ht->capacity);
	Node *newTable = (Node*) malloc(newCapacity * sizeof(Node));

	for(int i = 0; i < newCapacity; i++){
		(newTable+i)->address = -1;
		(newTable+i)->next = NULL;
	}

	long newKey;
	Node *oldPtr;
	Node *newPtr;

	for(int i = 0; i < oldCapacity; i++){
		oldPtr = (ht->table)+i;

		// if address is -1 (not initialized) then continue
		if(oldPtr->address == -1){
			continue;
		}
		else{

			while(oldPtr != NULL){

				newKey = (oldPtr->address)%newCapacity;
				newPtr = newTable+newKey;

				// if address is -1 (not initialized) then set it as this address
				if(newPtr->address == -1){
					newPtr->address = oldPtr->address;
				}
				// if already has Nodes, then go to last node and then add after it
				else{
					while(newPtr->next != NULL){
						newPtr = newPtr->next;
					}
					newPtr->next = (Node*) malloc(sizeof(Node));
					newPtr = newPtr->next;
					newPtr->address = oldPtr->address;
					newPtr->next = NULL;
				}

				oldPtr = oldPtr->next;

			} // end while loop

		} // end else condition

	} // end for loop

	free(ht->table);
	ht->table = newTable;

}


int getSum(Hashtable *ht){

	int count = 0;
	int indexCount;
	Node *ptr;

	for(int i = 0; i < ht->capacity; i++){
		indexCount = 0;
		ptr = ((ht->table)+i);

		if(ptr->address == -1){
			continue;
		}
		else{
			while(ptr != NULL){
				indexCount++;
				ptr = ptr->next;
			}
		}

		count += indexCount;

	}

	return count;

}


int main(int argc, const char *argv[]){


	FILE *fp;
	fp = fopen(argv[1],"r");

	if(fp == NULL){
		printf("error\n");
		return 1;
	}

	// start reading from file & check if file is empty
	char buff[36];
	if(fscanf(fp, "%s", buff) == EOF){
		printf("0\n");
		return 0;
	}
	// initialize long decimal value
	long decNum;

	// Create ht struct & initialize it
	Hashtable *ht = (Hashtable*) malloc(sizeof(Hashtable));
	ht->capacity = 1000;
	ht->loadFactor = 0;
	ht->size = 0;
	ht->table = (Node*) malloc(ht->capacity*sizeof(Node));
	for(int i = 0; i<1000; i++){
		((ht->table)+i)->address = -1;
		((ht->table)+i)->next = NULL;
	}

	// Insert all the addresses into the hash table
	do{
 		decNum = hexToDec(buff);
 		// printf("Decimal: %lu\n", decNum);

		// If improper input, skip to next line
		if(decNum == -1){
			continue;
		}

		insert(ht, decNum);

	}while(fscanf(fp,"%s",buff) != EOF); // end outer loop

    // Close the file
    fclose(fp);


    int numAddress = getSum(ht);

    printf("%d\n", numAddress);


	free(ht->table);
	free(ht);

    return 0;

}



