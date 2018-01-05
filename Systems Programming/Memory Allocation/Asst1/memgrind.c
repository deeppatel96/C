#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "mymalloc.h"
#include "mymalloc.c"


// Workload A:
// malloc 1 byte 3000 times, then free 3000 1 byte pointers
void workloadA(){

	char *a[3000];
	for(int i = 0; i < 3000; i++){
		a[i] = (char*) malloc(1);
	}

	for(int i = 0; i < 3000; i++){
		free(a[i]);
	}

}


// Workload B: 
// malloc 1 byte & immediately free it 3000 times in row
void workloadB(){

	char *b;
	for(int i = 0; i < 3000; i++){
		b = (char*) malloc(1);
		free(b);
	}

}

// Workload C: 
// randomly choose between (1 byte malloc or free) 6000 times
void workloadC(){
	int mCount = 0;		// malloc count
	int fCount = 0;		// free count
	int n;				// 0 or 1 random probability factor
	srand(time(NULL));	// initialize random number generator
	char *c[3000];		// initialize char pointer array of 3000 size

	while(mCount < 3000){
		n = rand() % 2;
		if(n == 0){
			c[mCount] = (char*) malloc(1);
			mCount++;
		}
		else{
			if(fCount>=mCount){
				c[mCount] = (char*) malloc(1);
				mCount++;
			}
			free(c[fCount]);
			fCount++;
		}
	}
	for(fCount = fCount; fCount < 3000; fCount++){
		free(c[fCount]);
	}

}


// Workload D: 
// randomly choose between (a randomly sized malloc or free) 6000 times
void workloadD(){

	int mCount = 0;		// malloc count
	int fCount = 0;		// free count
	int n;				// 0 or 1 random probability factor
	int random;
	srand(time(NULL));	// initialize random number generator
	char *c[3000];		// initialize char pointer array of 3000 size

	while(mCount < 3000){
		n = rand() % 2;
		random = rand()/100000;
		if(n == 0){
			c[mCount] = (char*) malloc(random);
			mCount++;
		}
		else{
			if(fCount>=mCount){
				c[mCount] = (char*) malloc(random);
				mCount++;
			}
			free(c[fCount]);
			fCount++;
		}
	}
	for(fCount = fCount; fCount < 3000; fCount++){
		free(c[fCount]);
	}

}

// Workload E
// Mallocs 50 bytes 20 times, then frees random pointers 20 times 
// and then frees all just in case
void workloadE(){

	char *e[100];
	srand(time(NULL));	// initialize random number generator
	int r;

	for(int i = 0; i<100; i++){
		e[i] = (char*) malloc(100);
	}
	for(int i = 0; i<100; i++){
		printf("%D\n",r);
		free(e[r]);
	}
	for(int i = 0; i<100; i++){
		free(e[i]);
	}

}

// Workload F
// Testing expected behavior: I malloc 998 byte blocks 5 times to take up all the memory
// and then (free 3 times and malloc 3 times) 10 times.
void workloadF(){

	char *f[5];
	for(int i = 0; i<5; i++){
		f[i] = malloc(998);
	}
	for(int i = 0; i < 10; i++){
		for(int j = 2; j < 5; j++){
			free(f[j]);
		}
		for(int j = 2; j < 5; j++){
			f[j] = malloc(998);
		}
	}

	for(int j = 2; j < 5; j++){
		free(f[j]);
	}

}


int main(int argc, const char *argv[]){

	// Initializes the block of memory
	initialize();

	// Start running workloads

	clock_t t1 = clock();		// Get start time for Workload A
	for(int i = 0; i<100; i++){
		workloadA();
	}
	clock_t t2 = clock();	// Get end time of Workload A & start time for Workload B	
		
	for(int i = 0; i<100; i++){
		workloadB();
	}
	clock_t t3 = clock();	// Get end time of Workload B & start time for Workload C
		
	for(int i = 0; i<100; i++){
		workloadC();
	}
	clock_t t4 = clock();	// Get end time of Workload C & start time for Workload D
		
	for(int i = 0; i<100; i++){
		workloadD();
	}
	clock_t t5 = clock();	// Get end time of Workload D & start time for Workload E

	for(int i = 0; i<100; i++){
		workloadE();
	}
	clock_t t6 = clock();	// Get end time of Workload E & start time for Workload F

	for(int i = 0; i<100; i++){
		workloadF();
	}
	clock_t t7 = clock();	// get end time of Workload F



	// Get total runtimes for each workload
	clock_t t_a = t2 - t1;
	clock_t t_b = t3 - t2;
	clock_t t_c = t4 - t3;
	clock_t t_d = t5 - t4;
	clock_t t_e = t6 - t5;
	clock_t t_f = t7 - t6;
	

	//Prints average time of each workload
	printf("Avg Run Time Workload A: %lu us\n", t_a * 10000 / CLOCKS_PER_SEC);
	printf("Avg Run Time Workload B: %lu us\n", t_b * 10000 / CLOCKS_PER_SEC);
	printf("Avg Run Time Workload C: %lu us\n", t_c * 10000 / CLOCKS_PER_SEC);
	printf("Avg Run Time Workload D: %lu us\n", t_d * 10000 / CLOCKS_PER_SEC);
	printf("Avg Run Time Workload E: %lu us\n", t_e * 10000 / CLOCKS_PER_SEC);
	printf("Avg Run Time Workload F: %lu us\n", t_f * 10000 / CLOCKS_PER_SEC);


}