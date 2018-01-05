#ifndef __LIBNETFILES_H__
#define __LIBNETFILES_H__

#define INVALID_FILE_MODE 100
#define MAX_OPEN_FILES 100

#define unrestricted 0
#define exclusive 1
#define transaction 2

#define threadopen 0
#define threadread 1
#define threadwrite 2
#define threadclose 3

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

typedef struct openFile_t{
	int fd;
	int canRead;
	int canWrite;
} openFile_t;

openFile_t open_files[MAX_OPEN_FILES];

extern int netserverinit(char *hostname, int filemode);
extern int netopen(const char *pathname, int flags);
extern ssize_t netread(int fildes, void *buf, size_t nbyte);
extern ssize_t multiplexRead(int fildes, void *buf, size_t nbyte, int portNum);
extern ssize_t netwrite(int fildes, const void *buf, size_t nbyte);
extern int netclose(int fd);
extern int makeSocket(int port);
extern void sendCommand(int fd, int command);
extern int addFile(int fd, int flag);
extern int removeFile(int fd);
extern int canRead(int fd);
extern int canWrite(int fd);
extern int getFlag(int fd);


#endif // __LIBNETFILES_H__