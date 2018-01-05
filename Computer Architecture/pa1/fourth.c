#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "fourth.h"

int main(int argc, const char *argv[]){


	FILE *fp;
	fp = fopen(argv[1],"r");

	if(fp == NULL){
		printf("error\n");
		return 1;
	}

    char buff[15];

 //start reading from file & check if file is empty
    if (fgets(buff, sizeof(buff), fp) == NULL) {
		printf("\n");
		return 0;
	}

	// Create ht struct and 
	Hashtable *ht = (Hashtable*) malloc(sizeof(Hashtable));


	//Begin loop
	do{
		// get the first token in the line
		char* token = strtok(buff, "\t");

		// If insert
		if (*token == 'i'){


	    	token = strtok(NULL, "\t");

	    	//check for improper input
			if(!isdigit(*token)){
				printf("error\n");
				continue;
			}

			//Get number
			int num = atoi(token);

			// Get location of index in hashtable
			int index = num % 1000;

			// Find location is empty
			if(ht->table[index] == 0){
				ht->table[index] = num;
				ht->size++;
				printf("inserted\n");
				continue;
			}
			// If duplicate is found in location
			else if (ht->table[index] == num){
				printf("duplicate\n");
				continue;
			}
			// Linear probing until empty index or number is found
			else{
				while(ht->table[index] != 0 || ht->table[index] != num){
					if(index<1000){
						index++;
					}
					else{
						index = 0;
					}
				}
				if(ht->table[index] == 0){
					ht->table[index] = num;
					ht->size++;
					printf("inserted\n");
					continue;
				}
				else if (ht->table[index] == num){
					printf("duplicate\n");
					continue;
				}
			}

		} // end insert if

		// if search
		else if (*token == 's'){



	    	token = strtok(NULL, "\t");

	    	//check for improper input
			if(!isdigit(*token)){
				printf("error\n");
				continue;
			}
			// Get number
			int num = atoi(token);

			// Get location of index in hashtable
			int index = num % 1000;

			// Find location is empty
			if(ht->table[index] == 0){
				printf("absent\n");
				continue;
			}
			// If searched number is found in location
			else if (ht->table[index] == num){
				printf("present\n");
				continue;
			}
			// Linear probing until empty index or number is found
			else{
				while(ht->table[index] != 0 || ht->table[index] != num){
					if(index<1000){
						index++;
					}
					else{
						index = 0;
					}
				}
				if(ht->table[index] == 0){
					printf("absent\n");
					continue;
				}
				else if (ht->table[index] == num){
					printf("present\n");
					continue;
				}
			}

		} // end search if

		// Print error when improper line structure
		else{
			printf("error\n");
		}

	}while(fgets(buff, sizeof(buff), fp) != NULL); // end outer loop
    
    // Close the file
    fclose(fp);

    //free ht variable
    free(ht);

	return 0;

}

