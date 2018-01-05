#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <errno.h>

typedef enum {FIFO, LRU} rep_policy_type;
typedef enum {wt, wb} write_policy_type;

typedef struct {

    int cache_size;
    int block_size;
    int set_size;
    int num_sets;
    int num_lines;
    int block_offset;
    int index_bits;
    int tag_bits;

    rep_policy_type rep_policy;
    write_policy_type write_policy;

    int *rank;
    
    int *valid;
    int *dirty;
    char **tag;

    char *currTag;
    char *currIndex;

    int reads;
    int writes;
    int hits;
    int misses;

} Cache;


//input an integer and it returns 0 if it a power of 2
int isPowerOfTwo (unsigned int x)
{
  return ((x != 0) && ((x & (~x + 1)) == x));
}

int binToDec(char* bin){

    int i;
    int x;
    int power = 1;
    int sum = 0;
    for(i = strlen(bin)-1; i >= 0; i--){
        x = (int) bin[i] - '0';
        sum += x * power;
        power *= 2;
    }
    return sum;
}


void updateBits(Cache *cache, char *addr){
    memcpy(cache->currTag, &addr[cache->index_bits], cache->tag_bits);
    memcpy(cache->currIndex, addr, cache->index_bits);

}

int findBlock(Cache *cache, int set){

    int i;
    int line = -1;
    for(i = set*cache->set_size; i < (set+1)*cache->set_size; i++){
        if(strcmp(cache->tag[i], cache->currTag) == 0 && cache->valid[i] == 1){
            line = i;
        }
    }

    return line;
}



int getBlock(Cache *cache, int set){

    int i;
    int line = set * cache->set_size;
    for(i = set*cache->set_size; i < (set+1)*cache->set_size; i++){
        if(cache->valid[line] == 0){
            line = i;
            return line;
        }
    }
    for(i = set*cache->set_size; i < (set+1)*cache->set_size; i++){
        if(cache->rank[line] < cache->rank[i]){
            line = i;
        }
    }

    return line;
}

void updateRank(Cache *cache, int set, int line){

    int i;
    int r = cache->rank[line];
    if(cache->valid[line] == 0){
        cache->valid[line] = 1;
    }
    if(r == 0 || r == 1){
        cache->rank[line] = 1;
    }
    else{
        for(i = set*cache->set_size; i < (set+1)*cache->set_size; i++){
            if(cache->valid[i] == 1 && cache->rank[i] < r){
                cache->rank[i]++;
            }
        }
        cache->rank[line] = 1;
    }

    return;
}

// Function simulates a read in the cache
void read(Cache *cache, char *addr){

    updateBits(cache, addr);
    printf("currTag = %s\n",cache->currTag);
    printf("currIndex = %s\n",cache->currIndex);
    int index = binToDec(cache->currIndex);
    printf("index = %d\n",index);

    int line = findBlock(cache, index);
    printf("line = %d\n",line);

    // Cache miss
    if(line == -1){
        printf("Cache Miss\n");
        cache->misses++;
        line = getBlock(cache, index);
        // if write-back cache, set dirty bit
        if(cache->write_policy == wb){
            if(cache->dirty[line] == 1){
                cache->writes++;
                cache->dirty[line] = 0;
            }
        }
        cache->reads++;
        strcpy(cache->tag[line], cache->currTag);
        updateRank(cache, index, line);
    }
    // Cache hit
    else{
        printf("Cache Hit\n");
        cache->hits++;
        if(cache->rep_policy == LRU){
            updateRank(cache, index, line);
        }
    }
    printf("\n");

    return;

}

// Function simulates a write in the cache
void write(Cache *cache, char *addr){

    updateBits(cache, addr);
    printf("currTag = %s\n",cache->currTag);
    printf("currIndex = %s\n",cache->currIndex);

    
    int index = binToDec(cache->currIndex);
    printf("index = %d\n",index);

    int line = findBlock(cache, index);
    printf("line = %d\n",line);

    // Cache miss
    if(line == -1){
        printf("Cache Miss\n");
        cache->misses++;
        line = getBlock(cache, index);
        // if write-back cache, set dirty bit
        if(cache->write_policy == wb){
            if(cache->dirty[line] == 1){
                cache->writes++;
                cache->dirty[line] = 0;
            }
        }
        cache->reads++;
        strcpy(cache->tag[line], cache->currTag);
        updateRank(cache, index, line);
        if(cache->write_policy == wt){
            cache->writes++;
        }
    }
    // Cache hit
    else{
        printf("Cache Hit\n");
        cache->hits++;
        if(cache->rep_policy == LRU){
            updateRank(cache, index, line);
        }
        if(cache->write_policy == wt){
            cache->writes++;
        }
        else{
            cache->dirty[line] = 1;
        }
    }
    printf("\n");

    return;



}

// inputs char array of hexidecimal input and ouputs char pointer to binary output
// returns -1 if improper input

char *hexToBin(char hex[], char *bin){

    int i;
    char *error = "-1";
    bin[0] = '\0';

    // if the first 2 characters are not '0' or 'x' respectively, return a '-1' to improper input
    if (hex[0] != '0' || hex[1] != 'x'){
        return error;
    }
    for(i = 2; i < strlen(hex); i++){

        if(hex[i] < '0' || (hex[i] > '9' && hex[i] < 'A') || (hex[i] > 'F' && hex[i] < 'a') || hex[i] > 'f'){
            return error;
        }

    }

    /*
     * Finds binary of each hexadecimal digit
     */
    for(i=2; hex[i]!='\0'; i++)
    {
        switch(hex[i])
        {
            case '0':
                strcat(bin, "0000");
                break;
            case '1':
                strcat(bin, "0001");
                break;
            case '2':
                strcat(bin, "0010");
                break;
            case '3':
                strcat(bin, "0011");
                break;
            case '4':
                strcat(bin, "0100");
                break;
            case '5':
                strcat(bin, "0101");
                break;
            case '6':
                strcat(bin, "0110");
                break;
            case '7':
                strcat(bin, "0111");
                break;
            case '8':
                strcat(bin, "1000");
                break;
            case '9':
                strcat(bin, "1001");
                break;
            case 'a':
            case 'A':
                strcat(bin, "1010");
                break;
            case 'b':
            case 'B':
                strcat(bin, "1011");
                break;
            case 'c':
            case 'C':
                strcat(bin, "1100");
                break;
            case 'd':
            case 'D':
                strcat(bin, "1101");
                break;
            case 'e':
            case 'E':
                strcat(bin, "1110");
                break;
            case 'f':
            case 'F':
                strcat(bin, "1111");
                break;
            default:
                printf("Invalid hexadecimal input.");
        }
    }

    return bin;

}

void printCache(Cache *cache){

    int i;

    for(i = 0; i < cache->num_lines; i++){
        printf("Line: %d Rank: %d  Valid: %d   Tag: %s\n",i,cache->rank[i], cache->valid[i], cache->tag[i]);
    }

}



// main program

int main(int argc, const char *argv[]){
    
    printf("\n");

// Input format:
// ./c-sim <cache size> <associativity> <block size> <replacement_policy> <write_policy> <trace file> 
    
    // Check for proper number of inputs
    if(argc != 7){
        printf("Error: improper input\n");
        return 0;
    }

    // Create a pointer to a Cache structure
    Cache *cache = (Cache*) calloc(1, sizeof(Cache));


    // Copy inputs into variables
    cache->cache_size = atoi(argv[1]);
    char *assoc = (char*) argv[2];
    cache->block_size = atoi(argv[3]);
    char *r_pol = (char*) argv[4];
    char *w_pol = (char*) argv[5];
    char *trace_file = (char*) argv[6];



    // Check for proper cache and block size input
    if(cache->cache_size <=0){
        printf("Error: Cache Size must be greater than 0\nExiting simulation...\n");
        return 1;
    }
    else if(!isPowerOfTwo(cache->cache_size)){
        printf("Error: Cache Size must be a power of 2\nExiting simulation...\n");
        return 1;
    }
    else if(cache->block_size <= 0){
        printf("Error: Block Size must be greater than 0\nExiting simulation...\n");
        return 1;
    }
    else if(!isPowerOfTwo(cache->block_size)){
        printf("Error: Block Size must be a power of 2\nExiting simulation...\n");
        return(1);
    }

    if(strcasecmp(assoc,"direct") == 0){
        cache->set_size = 1;
        cache->num_sets = cache->cache_size / cache->block_size;
        printf("direct association\n");
    }   
    else if(strcasecmp(assoc,"assoc") == 0){
        cache->set_size = cache->cache_size / cache->block_size;
        cache->num_sets = 1;
        printf("fully associative\n");
    }
    else if(strncasecmp(assoc,"assoc:",6) == 0 && isdigit(assoc[6]) !=0){
        cache->set_size = atoi(&assoc[6]);
        if(!isPowerOfTwo(cache->set_size)){
            printf("Error: set-associativity number must be a power of 2\nExiting simulation...\n");
            return(1);
        }
        cache->num_sets = cache->cache_size / (cache->block_size * cache->set_size);
    }
    else{
        printf("Error: improper associativity input\nExiting simulation...\n");
        return(1);
    }
    if(cache->num_sets == 0){
        printf("Error: Cache Size not big enough given block size and associativity\nExiting simulation...\n");
        return(1);
    }

// Check replacement policy input
    if(strcasecmp(r_pol, "FIFO") == 0){
        cache->rep_policy = FIFO;
    }
    else if(strcasecmp(r_pol, "LRU") == 0){
        cache->rep_policy = LRU;
    }
    else{
        printf("Error: improper replacement policy input, enter either (FIFO) or (LRU)\nExiting simulation...\n");
        return(1);
    }

// Check write policy input
    if(strcasecmp(w_pol, "wt") == 0){
        cache->write_policy = wt;
    }
    else if(strcasecmp(w_pol, "wb") == 0){
        cache->write_policy = wb;
    }
    else{
        printf("Error: improper write policy input, enter either (wt) or (wb)\nExiting simulation...\n");
        return(1);
    }

    int i;

    cache->num_lines = cache->cache_size / cache->block_size;
    cache->block_offset = log(cache->block_size) / log(2);
    cache->index_bits = log(cache->num_sets) / log(2);
    cache->tag_bits = 32 - cache->block_offset - cache->index_bits;

    cache->reads = 0;
    cache->writes = 0;
    cache->hits = 0;
    cache->misses = 0;


    printf("set_size = %d\n",cache->set_size);
    printf("num_sets = %d\n",cache->num_sets);
    printf("cache_size = %d\n", cache->cache_size);
    printf("assoc = %s\n", assoc);
    printf("block_size = %d\n", cache->block_size);
    printf("num_lines = %d\n", cache->num_lines);
    printf("r_pol = %s\n", r_pol);
    printf("w_pol = %s\n",w_pol);
    printf("trace_file = %s\n",trace_file);
    printf("block_offset = %d\n",cache->block_offset);
    printf("index_bits = %d\n",cache->index_bits);
    printf("tag_bits = %d\n\n", cache->tag_bits);

    cache->tag = (char**) calloc(1, sizeof(char*) * cache->num_lines);
    for(i = 0; i < cache->num_lines; i++){
        cache->tag[i] = (char*) calloc(1, sizeof(char) * (cache->tag_bits+1));
        cache->tag[i][cache->tag_bits] = '\0';
    }

    cache->currTag = (char*) calloc(1, sizeof(char) * (cache->tag_bits+1));
    cache->currTag[cache->tag_bits] = '\0';
    cache->currIndex = (char*) calloc(1, sizeof(char) * (cache->index_bits+1));
    cache->currIndex[cache->index_bits] = '\0';
    cache->valid = (int*) calloc(1, sizeof(int) * cache->num_lines);
    if(cache->write_policy == wb){
        cache->dirty = (int*) calloc(1, sizeof(int) * cache->num_lines);
    }
    cache->rank = (int*) calloc(1, sizeof(int) * cache->num_lines);
    for(i = 0; i< cache->num_lines; i++){
        cache->rank[i] = cache->set_size;
    }



    // Open file and give an error and return depending on error that occured
    FILE *fp;
    fp = fopen(trace_file,"r");
    if( fp == NULL ){
        printf("Error: %s\n", strerror(errno));
        return(1);
    }

    // start reading from file & check if file is empty
    char buff[36];
    if(fscanf(fp, "%s", buff) == EOF){
        printf("Error: %s file is empty\n",trace_file);
        return 0;
    }
    else if(strcmp(buff, "#eof") == 0){
        printf("Error: %s file is empty\n",trace_file);
        return 0;
    }

    // char pointer to binary buff
    char *binAddr = (char*) calloc(1, sizeof(char) * 33);
    char action[2];


    // PROGRAM LOOP
    do{
        fscanf(fp,"%s",buff);
        strcpy(action,buff);
        fscanf(fp,"%s",buff);

        printf("Address: %s\n",buff);
        binAddr = hexToBin(buff, binAddr);
        printf("Binary: %s\n", binAddr);
        printf("Action: %s\n",action);

        // If improper input, skip to next line
        if(strcmp(binAddr, "-1") == 0){
            continue;
        }

        if(strcasecmp(action, "R") == 0){
            read(cache, binAddr);
        }
        else if(strcasecmp(action, "W") == 0){
            write(cache, binAddr);
        }

        printCache(cache);
        printf("Memory reads: %d\n", cache->reads);
        printf("Memory writes: %d\n", cache->writes);
        printf("Cache hits: %d\n", cache->hits);
        printf("Cache misses: %d\n\n", cache->misses);
        printf("=================================================================\n\n");

    }while((fscanf(fp,"%s",buff) != EOF) && strcmp(buff,"#eof")); // end outer loop



    // Close the file
    fclose(fp);

    // Free dynamically allocated memory
    if(cache->rep_policy == wb){
        free(cache->dirty);
    }
    free(binAddr);
    for (i = 0; i < cache->num_lines; i++){
        free(cache->tag[i]);
    }
    free(cache->currTag);
    free(cache->currIndex);
    free(cache->tag);
    free(cache->valid);
    free(cache->rank);
    free(cache);

    printf("\n");

    printf("Memory reads: %d\n", cache->reads);
    printf("Memory writes: %d\n", cache->writes);
    printf("Cache hits: %d\n", cache->hits);
    printf("Cache misses: %d\n", cache->misses);


}



