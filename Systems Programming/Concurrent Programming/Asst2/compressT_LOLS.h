#include "compress_LOLS.h"

#ifndef _COMPRESST_LOLS_H
# define _COMPRESST_LOLS_H

typedef struct {
    char *in_file;
    char *out_file;
    int start;
    int length;
} Segment;

int compressT_LOLS(char *fname, int segcount);

#endif
