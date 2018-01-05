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
			printf("Couldn't create a new file\n");
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


int main(int argc, char *argv[]){

	compress_segment((char*) argv[3], (char*) argv[4], atoi(argv[1]), atoi(argv[2]));


return 0;

}
