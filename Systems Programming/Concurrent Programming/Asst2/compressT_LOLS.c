#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>


typedef struct Segment{
    char *inFile;
    char *outFile;
    int start;
    int length;
} Segment;




/**
 *
 * Core compression method. If the string is not long enough to be compressed then null is returned.
 * If the string is long enough, then a new malloc()ed string is returned with the compressed data.
 * Returning a new string does not ensure that the string was compressed at all, only that it was long
 * enough to compress.
 *
 * This compression method follws the Length of Long Sequence compression model. It shortens runs of the same 
 * character. If there is either 1 or 2 of the same character, then no change is made. If more than 2 are found, 
 * then it is replaced with the count followed by the character to replace.
 *
 */
char *compress(char *section, int length) {

    // if less than or equal to 2 characters, return null
    if (length <= 2) return NULL;

    // declare output array, the current 'run' character, count of character, input index and output index
    char *output = malloc(sizeof(char) * length);
    char running = section[0], current;
    int count, inIndex, outIndex;

    // running is set to the first char (or zero-th) so set count to 1 and in index to 1
    for (count=1, inIndex=1, outIndex=0; inIndex<length; inIndex++) {
        current = section[inIndex];

        if (isdigit(current)) {
                        printf("\n***Got unexpected number '%c'. Ignoring***\n\n", current);
                        continue;
                }

        if (current == running) {
            count++;
        } 
        else {
            // if there was a change in running char, we write the output
            // writes the compressed information to the output string
		    if (count == 2) {
		        // if count == 2, we write the running char once. It will be written again later, effectively no change.
		        output[outIndex++] = running;
		    } 
		    else if (count > 2) {
		        // if we have more than two, we write the count, and set out index to the end of the string
		        sprintf(output+outIndex, "%d\0", count);
		        outIndex = strlen(output);
		    }
		    // finally, write the running char no matter what, and set the new running char
		    output[outIndex++] = running;
		    running = current;
		    count = 1;
        }
    }
    // write remainIndexing output and terminate string
    // writes the compressed information to the output string
    if (count == 2) {
        // if count == 2, we write the running char once. It will be written again later, effectively no change.
        output[outIndex++] = running;
    } else if (count > 2) {
        // if we have more than two, we write the count, and set out index to the end of the string
        sprintf(output+outIndex, "%d\0", count);
        outIndex = strlen(output);
    }
    
    // finally, write the running char no matter what, and set the new running char
    output[outIndex++] = running;
    running = current;
    count = 1;
    output[outIndex++] = '\0';
    return output;
}


/**
 * compress_segment takes a file name, an output file name, a starting pos, and length.
 *
 * This method compresses a specific segment of a file and writes it to the
 * specified output file. This is the method called by both thread and process versions
 * of the LOLS compression project.
 */
void compress_segment(char *inFile, char *outFile, int start, int length) {


    int fd = open(inFile, O_RDONLY);
    char *original = malloc(sizeof(char) * length);
    if (lseek(fd, start, SEEK_SET) > -1) {
        // we successfully moved to our starting position
        read(fd, original, length);
        close(fd);
        // compress our specific result
        char *new = compress(original, length);
		fd = open(outFile, O_RDWR | O_APPEND | O_CREAT, 0666);



		if(fd == -1){
			printf("Thread #%d: Couldn't create a new file\n",(int) pthread_self());
			return;
		}

        if (new == NULL) {
            // if no changes were made, write the old sequence
            write(fd, original, length);
        }
        else {
            // otherwise write the new result
            write(fd, new, strlen(new));
        }

        close(fd);
    }
    free(original);



}




/**
 * method used when spawning each thread
 */
void *compress_segment_thread(void *segment) {
    Segment *s = (Segment *) segment;
    compress_segment(s->inFile, s->outFile, s->start, s->length);
    return NULL;
}



int compressT_LOLS(char* fname, int numParts){


    if (numParts <= 0) {
       printf("Not a valid number of segment/part requests\n");
       return -1;
    }

    int len = strlen(fname);
    printf("Length of file name = %d\n",len);
    if(len<5){
    	printf("Improper file name\n");
       return -1;
    }

    if(memcmp(fname,"./",2) != 0){
    	char str[250];
    	char *end = "/0";
    	strcpy(str,"./");
    	strcat(str,fname);
    	fname = str;
    }

	int fd = open(fname, O_RDONLY);

	if(fd == -1){
		printf("Error opening file: %s\n",strerror(errno));
		close(fd);		
		return -1;
	}
	// get number of characters in file
	int size = lseek(fd, (size_t)0, SEEK_END);


	// if file size is 0
	if(size == 0){
		printf("Error: empty file\n");
		close(fd);		
		return -1;
	}
	// If there are more segments requested then are characters
	else if(size < numParts){
		printf("Error: more number of segments than characters in file\n");
		close(fd);
		return -1;
	}
	
	close(fd);		
	char *outName = malloc(strlen(fname)+10);

// if the output file already exists
    strcpy(outName, fname);
    *(outName + (strlen(fname) - 4)) = '_';
    strcat(outName, "_LOLS");
    int fd2 = open(outName, O_RDONLY);
	if(fd2 != -1){
		printf("Error: compressed file already exists:\n");
		close(fd2);
		free(outName);
		return -1;
	}
	close(fd2);



    int temp = strlen(outName);
    *(outName + (temp)) = '0';
    *(outName + temp+1) = '\0';
    int fd3 = open(outName, O_RDONLY);
	if(fd3 != -1){
		printf("Error: compressed file already exists:\n");
		close(fd3);
		free(outName);
		return -1;
	}
	free(outName);
	close(fd3);



	// Floored segment length
	int segLen = size / numParts;

	// Additional slack for when segments are not of equal length
	int slack = size % numParts;
	
	// length of file name
	int fLen = strlen(fname);

    // temp object for current segment
    Segment *seg;

    // segment array for all the parts
	Segment *segments = (Segment*) malloc(sizeof(Segment) * numParts);
	
	// Allocate memory for threads
	pthread_t *threads = malloc(sizeof(pthread_t) * numParts);

	char *outF = (char*) malloc(fLen + 10);
    strcpy(outF, fname);
    *(outF + (strlen(outF) - 4)) = '_';
    strcat(outF, "_LOLS%d\0");


    printf("\n");
    int i = 0;
    int tcount = 0;
    int position = 0;

    for(i = 0; i < numParts; i++){

    	// go to current segment
    	seg = &segments[i];

    	// initialize files and start & end positions in file
    	seg->start = position;
    	seg->length = segLen;
    	if(slack > 0){
    		seg->length++;
    	}
    	seg->inFile = fname;
    	seg->outFile = malloc(fLen +15);
    	sprintf(seg->outFile, outF, i);

    	if (numParts == 1){
    		seg->outFile[strlen(seg->outFile) -1] = '\0';
    	}

        // attempt to create a new thread
        if (pthread_create(&threads[i], NULL, (void*) &compress_segment_thread,(void*) seg)) {
            
            // if fails, print error, kill all running threads, and return
            printf("Error creating threads: output not valid.\n");
            
            for (i=0; i<tcount; i++) {
                pthread_cancel(threads[i]);
			}
            break;
        } 
        else {
            // otherwise, we move our position, increment thread count, and decrease slack
            position += seg->length;
            tcount++;
            slack--;
        }
    }


    if (tcount == numParts) {
        for (i=0; i<tcount; i++) {
            pthread_join(threads[i], NULL);
        }
    }

    free(segments);
    free(threads);
    free(outF);
    return size;

}


