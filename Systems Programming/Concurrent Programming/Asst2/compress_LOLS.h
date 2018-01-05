#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#ifndef _COMPRESS_LOLS_H
# define _COMPRESS_LOLS_H

int check_file(char * infile, int segments);
void compress_segment(char *fname, char *outname, int start, int length);

#endif

