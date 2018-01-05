#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>



int compressR_LOLS(char *fname, int numParts){


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



    //make array to pass in execvp
    char * args[6];
    char passLength[10];
    char passStart[10];
    char *exec = "./compressR_worker_LOLS.c\0";



    //make array of PIDs to wait on
    pid_t pids[numParts];
    pid_t pid, parent;

    // variables for each process
    int status;
    int seg_length = size / numParts;
    int slack = size % numParts;
    int i, position = 0;
    char * out_file;


    // copy file name, append _LOLS to the end and add %d for segment number
        char *outF = malloc(strlen(fname) + 10);
        strcpy(outF, fname);
        *(outF + (strlen(outF) - 4)) = '_';
    	strcat(outF, "_LOLS%d\0");


    //make processes and make children run worker file
    for (i; i<numParts; i++){
        sprintf(passStart, "%d", position);
        if(slack>0){
        	sprintf(passLength, "%d", seg_length+1);
        	slack--;
        	position += seg_length+1;
        }
        else{
        	sprintf(passLength, "%d", seg_length);
        	position += seg_length;

        }


        out_file = malloc(strlen(fname) + 15);
        sprintf(out_file, outF, i);

        if (numParts == 1) {
            // if we are only processing 1 segment, then we remove the "0" at the end of the file name
            out_file[strlen(out_file) - 1] = '\0';
        }

        args[0] = exec;
	    args[1] = passStart;
	    args[2] = passLength;
	    args[3] = fname;
	    args[4] = out_file;
	    args[5] = NULL;


    //if the parent then put the PID in array and wait
        pid = fork();
        if(pid == -1){
        	printf("Fork has failed... exitings from all processes\n");
        	return -1;
        }
        else if (pid != 0){
            pids[i] = pid;
            parent = waitpid(pid, &status, 0);
        }
        else {
           	//if child then get metadata and pass to execvp
          	int error = execvp(args[0], args);
          	if(error == -1){
                printf("Exec gave error value: %d\n",error);
          	     printf("%s\n", strerror(errno));
            }

        }
        
        free(out_file);
    }

        free(outF);
        return size;

}


