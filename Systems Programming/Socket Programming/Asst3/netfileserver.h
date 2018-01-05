#ifndef __NETFILESERVER_H__
#define __NETFILESERVER_H__

#define MAX_OPEN_FILES 100

#define unrestricted 0
#define exclusive 1
#define transaction 2

#define threadopen 0
#define threadread 1
#define threadwrite 2
#define threadclose 3

typedef struct BIN_SEMA
  { 
     pthread_cond_t  cv;    /* cond. variable - used to block threads */
     pthread_mutex_t mutex; /* mutex variable - used to prevents concurrent access to the variable "flag" */
     int flag;          	/* Semaphore state: 0 = down, 1 = up */
} bin_sema;

typedef struct accept_message {
	int command;
} accept_message;

typedef struct open_message {
	int file_mode;
	char file_path[142];
	int flag;
} open_message;

typedef struct open_return_message {
	int errorNum;
	int fd;
} open_return_message;

typedef struct read_message {
	int fd;
	size_t size;
	int offset;
} read_message;

typedef struct read_return_message {
	int errorNum;
	int numCharRead;
	char buff[2000];
	int multiplexOn;
} read_return_message;

typedef struct write_message {
	int fd;
	char buff[2000];
	size_t size;
	int offset;
} write_message;

typedef struct write_return_message {
	int errorNum;
	int numCharWrote;
} write_return_message;

typedef struct close_message {
	int fd;
	int mode;
	int flag;
} close_message;

typedef struct close_return_message {
	int errorNum;
} close_return_message;

typedef struct multiplex_message {
	int numPorts;
	int port[10];
	int chunkSize[10];
} multiplex_message;

// if port is 0, it means it is available for multiplexing
typedef struct multiplexPort_t{
	int port;
} multiplexPort_t;

multiplexPort_t multiplexPorts[10];

typedef struct openFile_t{
	int fd;
	int numClients;
	int numReadClients;
	int numWriteClients;
	int canRead;
	int canWrite;
	char path[150];
} openFile_t;

openFile_t open_files[MAX_OPEN_FILES];

void acceptThread(int *client_fd);
void threadOpen(int fd);
void threadRead(int fd);
void threadWrite(int fd);
void threadClose(int fd);
int isOpen(char *path);
int addFile(int fd, char* path, int mode, int flag);
int canOpen(int fd, int mode, int flag);
int getFileIndex(int fd);
int canRead(int i);
int canWrite(int i);
void setRead(int i, int state);
void setWrite(int i, int state);
void BinSemaInit(bin_sema **s);
void BinSemaP(bin_sema *s);
void BinSemaV(bin_sema *s);
int getNumPorts();
int getAndSetPort();
void makeMultiplex(int nport);



#endif // __NETFILESERVER_H__





