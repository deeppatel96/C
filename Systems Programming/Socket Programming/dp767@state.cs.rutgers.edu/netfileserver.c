#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <pthread.h>
#include "netfileserver.h"


extern int h_errno;
extern int errno;
bin_sema *s;
bin_sema *m;
int port = 10302;	// port number


int main(int argc, char *argv[]){

	// Set up open files Global Open Files Array
	int i = 0;
	for(i = 0; i < MAX_OPEN_FILES; i++){
		open_files[i].fd = 0;
		open_files[i].numClients = 0;
		open_files[i].numReadClients = 0;
		open_files[i].numWriteClients = 0;
		open_files[i].canRead = 0;
		open_files[i].canWrite = 0;
		open_files[i].path[0] = '\0';
	}

	for(i = 0; i < 10; i++){
		multiplexPorts[i].port = 0;
	}

	// Initialize semaphore
	BinSemaInit(&s);
	BinSemaInit(&m);

	int rqst;						// socket accepting the request
	socklen_t alen;       			// length of address structure
	struct sockaddr_in my_addr;   	// address of this service
	struct sockaddr_in client_addr; // client's address
	alen = sizeof(client_addr);
	int sockoptval = 1;
	int svc;

	/* create a TCP/IP socket */
	if ((svc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("cannot create socket");
		exit(1);
	}

	/* allow immediate reuse of the port */
	setsockopt(svc, SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof(int));

	/* bind the socket to our source address */
	memset((char*)&my_addr, 0, sizeof(my_addr));  	// 0 out the structure
	my_addr.sin_family = AF_INET;   				// address family
	my_addr.sin_port = htons(port);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(svc, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) {
		perror("bind failed");
		exit(1);
	}

	// set the socket for listening (queue backlog of 10)
	if (listen(svc, 10) < 0) {
		perror("listen failed");
		exit(1);
	}

	// loop, accepting connection requests
	while(1) {
		while ((rqst = accept(svc, (struct sockaddr *) &client_addr, &alen)) < 0) {
			/* we may break out of accept if the system call */
            /* was interrupted. In this case, loop back and */
            /* try again */
            if ((errno != ECHILD) && (errno != EINTR)) {
                    perror("accept failed");
                    exit(1);
            }

         }
		// the socket for this accepted connection is rqst
        pthread_t thread;
        int ret;
        if((ret = pthread_create(&thread, NULL,(void*) &acceptThread, (void*) &rqst)) != 0){
        	errno = ret;
        	perror("pthread_create");
        	exit(1);
        }

	}

	free(s);

}

void acceptThread(int *client_fd){
	int fd = *client_fd;

	accept_message *a_message = (accept_message*) calloc(1, sizeof(accept_message));

	// Accept command message
	read(fd, a_message, sizeof(accept_message));
	if(a_message->command == threadopen){
		free(a_message);
		threadOpen(fd);
	}
	else if(a_message->command == threadread){
		free(a_message);
		threadRead(fd);
	}
	else if(a_message->command == threadwrite){
		free(a_message);
		threadWrite(fd);
	}
	else if(a_message->command == threadclose){
		free(a_message);
		threadClose(fd);
	}
}



void threadOpen(int fd){


	// Allocate memory for messages
	open_message *o_message = (open_message*) calloc(1, sizeof(open_message));
	open_return_message *or_message = (open_return_message*) calloc(1, sizeof(open_return_message));

	int bytesRead = read(fd, o_message, sizeof(open_message));
	if(bytesRead < 0){
		perror("read");
		or_message->errorNum = errno;
		or_message->fd = -1;

		write(fd, or_message, sizeof(open_return_message));
	    free(or_message);
	    free(o_message);
	    close(fd);
	   	pthread_exit(NULL);
	}

	// check if file exists
	if (0 == access(o_message->file_path, F_OK)) {
	}
	else {
		or_message->errorNum = errno;
		or_message->fd = -1;

		write(fd, or_message, sizeof(open_return_message));
	    free(or_message);
	    free(o_message);
	    close(fd);
	   	pthread_exit(NULL);
	}

	if(o_message->flag == O_RDONLY || o_message->flag == O_RDWR){
		// check if file can be read into
		if (0 == access(o_message->file_path, R_OK)) {
		}
		else {
			or_message->errorNum = errno;
			or_message->fd = -1;

			write(fd, or_message, sizeof(open_return_message));
		    free(or_message);
		    free(o_message);
		    close(fd);
		   	pthread_exit(NULL);
		}
	}

	if(o_message->flag == O_WRONLY || o_message->flag == O_RDWR){
		// check if file can be written into
		if (0 == access(o_message->file_path, W_OK)) {
		}
		else {
			or_message->errorNum = errno;
			or_message->fd = -1;

			write(fd, or_message, sizeof(open_return_message));
		    free(or_message);
		    free(o_message);
		    close(fd);
		   	pthread_exit(NULL);
		}
	}

	// Set semaphore
	BinSemaP(s);

		// check if file is open
		int isfid = isOpen(o_message->file_path);

		// If file is not open
		if(isfid == -1){
			// Open the file and check for error
			int fid = 0;
			if((fid = open(o_message->file_path, o_message->flag)) < 0){
				perror("open file failed");
				or_message->errorNum = errno;
				or_message->fd = -1;

				write(fd, or_message, sizeof(open_return_message));
			    free(or_message);
			    free(o_message);
			    close(fd);
			    BinSemaV(s);
			    pthread_exit(NULL);
			}

			// Add the file
			int ret = addFile(fid, o_message->file_path, o_message->file_mode, o_message->flag);

			// if error in adding file (max number of files opened)
			if(ret == -1){
				perror("adding file failed");
				or_message->errorNum = ENFILE;
				or_message->fd = -1;

				write(fd, or_message, sizeof(open_return_message));
			    free(or_message);
			    free(o_message);
			   	close(fd);
			   	BinSemaV(s);
			    pthread_exit(NULL);
			}
			// Success, send message with local file descriptor
			else{
				or_message->errorNum = 0;
				or_message->fd = fid;

				write(fd, or_message, sizeof(open_return_message));
			    free(or_message);
			    free(o_message);
			   	close(fd);
			   	BinSemaV(s);
			    pthread_exit(NULL);
			}
		}

		// If file is open
		else{

			int op = canOpen(isfid, o_message->file_mode, o_message->flag);
			// if file is busy send error
			if(op == -1){
				perror("openFile");
				or_message->errorNum = errno;
				or_message->fd = -1;

				write(fd, or_message, sizeof(open_return_message));
			    free(or_message);
			    free(o_message);
			   	close(fd);
			   	BinSemaV(s);
			    pthread_exit(NULL);
			}
			// Success, send message with local file descriptor
			else{
				or_message->errorNum = 0;
				or_message->fd = isfid;

				write(fd, or_message, sizeof(open_return_message));
			    free(or_message);
			    free(o_message);
			   	close(fd);
			   	BinSemaV(s);
			    pthread_exit(NULL);
			}
		}

	// Close socket, and terminate thread
	close(fd);
	pthread_exit(NULL);
}




void threadRead(int fd){


	// Allocate memory for return message
	read_message *r_message = (read_message*) calloc(1, sizeof(read_message));
	read_return_message *rr_message = (read_return_message*) calloc(1, sizeof(read_return_message));

	// get read message from client
	int bytesRead = read(fd, r_message, sizeof(read_message));
	if(bytesRead < 0){
		perror("read");
		rr_message->errorNum = errno;

		write(fd, rr_message, sizeof(read_return_message));
	    free(rr_message);
	    free(r_message);
	    close(fd);
	   	pthread_exit(NULL);
	}

	int size = r_message->size;
	if(size > 2000){
		BinSemaP(m);
		int n;
		if((n = getNumPorts()) > 0){
			// send read return message telling it multiplexing is available
			rr_message->multiplexOn = 1;
			write(fd, rr_message, sizeof(read_return_message));

			int sizeLeft = size;
			int chunk = 0;
			multiplex_message *m_message = (multiplex_message*) calloc(1, sizeof(multiplex_message));
			m_message->numPorts = 0;

			// Assign Multiplex Message
			while(sizeLeft > 0 || n != 0){
				m_message->numPorts++;

				int p = getAndSetPort();

				m_message->port[chunk] = p;
				if(sizeLeft - 2000 > 0 && n == 1){
					m_message->chunkSize[chunk] = sizeLeft;
				}
				else if(sizeLeft - 2000 < 0){
					m_message->chunkSize[chunk] = sizeLeft;
				}
				else{
					m_message->chunkSize[chunk] = 2000;
				}

				sizeLeft -= m_message->chunkSize[chunk];
				n--;
				chunk++;
			}

			BinSemaV(m);
			// write multiplex message to client
			write(fd, m_message, sizeof(multiplex_message));

			for(n = 0; n < m_message->numPorts; n++){
				makeMultiplex(m_message->port[n]);
			}
			free(m_message);

		}
		BinSemaV(m);
	}


	// read from file
	lseek(r_message->fd, r_message->offset, SEEK_SET);
	int r = 0;
	if((r = read(r_message->fd, rr_message->buff, r_message->size)) < 0){
		perror("read");
		rr_message->errorNum = errno;

		write(fd, rr_message, sizeof(read_return_message));
	    free(rr_message);
	    free(r_message);
	    close(fd);
	   	pthread_exit(NULL);
	}
	else{
		rr_message->errorNum = 0;
		rr_message->numCharRead = r;
		write(fd, rr_message, sizeof(read_return_message));
	    free(rr_message);
	    free(r_message);
	    close(fd);
	   	pthread_exit(NULL);
	}

	// Close socket, and terminate thread
	close(fd);
	pthread_exit(NULL);
}


void threadWrite(int fd){


	// Allocate memory for return message
	write_message *w_message = (write_message*) calloc(1, sizeof(write_message));
	write_return_message *wr_message = (write_return_message*) calloc(1, sizeof(write_return_message));

	// get read message from client
	int bytesRead = read(fd, w_message, sizeof(write_message));
	if(bytesRead < 0){
		perror("write");
		wr_message->errorNum = errno;

		write(fd, wr_message, sizeof(write_return_message));
	    free(wr_message);
	    free(w_message);
	    close(fd);
	   	pthread_exit(NULL);
	}

	// write to file
	lseek(w_message->fd, 0, SEEK_SET);
	int w = 0;
	if((w = write(w_message->fd, w_message->buff, w_message->size)) < 0){
		perror("write");
		wr_message->errorNum = errno;

		write(fd, wr_message, sizeof(write_return_message));
	    free(wr_message);
	    free(w_message);
	    close(fd);
	   	pthread_exit(NULL);
	}
	// Success
	else{
		wr_message->errorNum = 0;
		wr_message->numCharWrote = w;
		write(fd, wr_message, sizeof(write_return_message));
	    free(wr_message);
	    free(w_message);
	    close(fd);
	   	pthread_exit(NULL);
	}

	// Close socket, and terminate thread
	close(fd);
	pthread_exit(NULL);
}


void threadClose(int fd){


	// Allocate memory for return message
	close_message *c_message = (close_message*) calloc(1, sizeof(close_message));
	close_return_message *cr_message = (close_return_message*) calloc(1, sizeof(close_return_message));

	// get read message from client
	int bytesRead = read(fd, c_message, sizeof(close_message));
	if(bytesRead < 0){
		perror("close");
		cr_message->errorNum = errno;

		write(c_message->fd, cr_message, sizeof(close_return_message));
	    free(cr_message);
	    free(c_message);
	    close(fd);
	   	pthread_exit(NULL);
	}

	// Set semaphore
	BinSemaP(s);

		// close file
		int i = getFileIndex(c_message->fd);
		// if the only client using the file
		if(open_files[i].numClients == 1){
			// remove from open files list
			open_files[i].fd = 0;
			open_files[i].numClients = 0;
			open_files[i].numReadClients = 0;
			open_files[i].numWriteClients = 0;
			open_files[i].canRead = 0;
			open_files[i].canWrite = 0;
			open_files[i].path[0] = '\0';
			// close the actual file
			if(close(c_message->fd) < 0){
				perror("close");
				cr_message->errorNum = errno;

				write(fd, cr_message, sizeof(close_return_message));
			    free(cr_message);
			    free(c_message);
			    close(fd);
			    BinSemaV(s);
			   	pthread_exit(NULL);
			}
		}
		// if not the only client using the file
		else{

			open_files[i].numClients--;

			if(c_message->mode == unrestricted){
				if(c_message->flag == O_RDONLY){
					open_files[i].numReadClients--;
				}
				else if(c_message->flag == O_WRONLY){
					open_files[i].numWriteClients--;
				}
				else if(c_message->flag == O_RDWR){
					open_files[i].numReadClients--;
					open_files[i].numWriteClients--;
				}
			}
			else if(c_message->mode == exclusive){
				setWrite(i,1);
				if(c_message->flag == O_RDONLY){
					open_files[i].numReadClients--;
				}
				else if(c_message->flag == O_WRONLY){
					open_files[i].numWriteClients--;
				}
				else if(c_message->flag == O_RDWR){
					open_files[i].numReadClients--;
					open_files[i].numWriteClients--;
				}
			}
			else if(c_message->mode == transaction){
				setRead(i,1);
				setWrite(i,1);
				if(c_message->flag == O_RDONLY){
					open_files[i].numReadClients--;
				}
				else if(c_message->flag == O_WRONLY){
					open_files[i].numWriteClients--;
				}
				else if(c_message->flag == O_RDWR){
					open_files[i].numReadClients--;
					open_files[i].numWriteClients--;
				}
			}
		}
	// Release semaphore
	BinSemaV(s);

	cr_message->errorNum = 0;
	write(fd, cr_message, sizeof(close_return_message));
    free(cr_message);
    free(c_message);

	// Close socket, and terminate thread
	close(fd);
	pthread_exit(NULL);
}


int isOpen(char *path){
	int i = 0;
	for(i = 0; i < MAX_OPEN_FILES; i++){
		if(strcmp(path, open_files[i].path) == 0){
			return open_files[i].fd;
		}
	}
	return -1;
}


int addFile(int fd, char* path, int mode, int flag){
	
	int i = 0;
	for(i = 0; i < MAX_OPEN_FILES; i++){
		if(open_files[i].fd == 0){
			open_files[i].fd = fd;
			strcpy(open_files[i].path, path);
			if(mode == unrestricted){
				if(flag == O_RDONLY){
					open_files[i].numReadClients++;
				}
				else if(flag == O_WRONLY){
					open_files[i].numWriteClients++;
				}
				else{
					open_files[i].numReadClients++;
					open_files[i].numWriteClients++;
				}
				open_files[i].canRead = 1;
				open_files[i].canWrite = 1;
			}
			else if(mode == exclusive){
				if(flag == O_RDONLY){
					open_files[i].numReadClients++;
				}
				else if(flag == O_WRONLY){
					open_files[i].numWriteClients++;
				}
				else{
					open_files[i].numReadClients++;
					open_files[i].numWriteClients++;
				}
				open_files[i].canRead = 1;
				open_files[i].canWrite = 0;
			}
			else if(mode == transaction){
				if(flag == O_RDONLY){
					open_files[i].numReadClients++;
				}
				else if(flag == O_WRONLY){
					open_files[i].numWriteClients++;
				}
				else{
					open_files[i].numReadClients++;
					open_files[i].numWriteClients++;
				}
				open_files[i].canRead = 0;
				open_files[i].canWrite = 0;
			}
			open_files[i].numClients++;
			return i;
		}
	}
	return -1;
}

int canOpen(int fd, int mode, int flag){

	int i = 0;
	for(i = 0; i < MAX_OPEN_FILES; i++){
		if(open_files[i].fd == fd){
			break;
		}
	}

	// read only client
	if(flag == O_RDONLY){
		// if read is closed, set errno and return error
		if(!canRead(i)){
			errno = EWOULDBLOCK;
			return -1;
		}
		// if client is exclusive
		else if(mode == exclusive){
			// if write is open, close it
			if(canWrite(i)){
				setWrite(i, 0);
				open_files[i].numClients++;
				open_files[i].numReadClients++;
				return 0;
			}
			// if write is closed, set errno and return error
			else{
				errno = EWOULDBLOCK;
				return -1;		
			}
		}
		else if(mode == transaction){
			// if write is open, close it and close read
			if(canWrite(i) && open_files[i].numReadClients == 0 && open_files[i].numWriteClients == 0){
				setRead(i, 0);
				setWrite(i, 0);
				open_files[i].numClients++;
				open_files[i].numReadClients++;
				return 0;
			}
			// if write is closed, set errno and return error
			else{
				errno = EWOULDBLOCK;
				return -1;		
			}
		}
		else{
			open_files[i].numClients++;
			open_files[i].numReadClients++;
			return 0;
		}

	}
	// write only client
	else if(flag == O_WRONLY){
		// if write is closed, set errno and return error
		if(!canWrite(i)){
			errno = EWOULDBLOCK;
			return -1;
		}
		// if client is exclusive
		else if(mode == exclusive){
			// if write is open, close it
			if(open_files[i].numWriteClients != 0){
				errno = EWOULDBLOCK;
				return -1;
			}
			setWrite(i, 0);
			open_files[i].numClients++;
			open_files[i].numWriteClients++;
			return 0;
		}
		// if client is transaction
		else if(mode == transaction){
			// if write is open, close it and close read
			if(canRead(i) && open_files[i].numReadClients == 0 && open_files[i].numWriteClients == 0){
				setRead(i, 0);
				setWrite(i, 0);
				open_files[i].numClients++;
				open_files[i].numWriteClients++;
				return 0;
			}
			// if not
			else{
				errno = EWOULDBLOCK;
				return -1;		
			}
		}
		else{
			open_files[i].numClients++;
			open_files[i].numWriteClients++;
			return 0;
		}
	}

	// read/write client
	else if(flag == O_RDWR){
		// if write is closed, set errno and return error
		if(!canWrite(i) || !canRead(i)){
			errno = EWOULDBLOCK;
			return -1;
		}
		// if client is exclusive
		else if(mode == exclusive){
			// check that there are no write clients
			if(open_files[i].numWriteClients != 0){
				errno = EWOULDBLOCK;
				return -1;
			}
			setWrite(i, 0);
			open_files[i].numClients++;
			open_files[i].numReadClients++;
			open_files[i].numWriteClients++;
			return 0;

		}
		// if client is transaction
		else if(mode == transaction){
			// if write is open, close it and close read
			if(open_files[i].numReadClients == 0 && open_files[i].numWriteClients == 0){
				setRead(i, 0);
				setWrite(i, 0);
				open_files[i].numClients++;
				open_files[i].numReadClients++;
				open_files[i].numWriteClients++;
				return 0;
			}
			// if not
			else{
				errno = EWOULDBLOCK;
				return -1;		
			}
		}
		else{
			open_files[i].numClients++;
			open_files[i].numReadClients++;
			open_files[i].numWriteClients++;
			return 0;
		}
	}

	return -1;
}

int getFileIndex(int fd){

	int i = 0;
	for(i = 0; i < MAX_OPEN_FILES; i++){
		if(open_files[i].fd == fd){
			return i;
		}
	}

	return -1;
}

int canRead(int i){
	return open_files[i].canRead;
}

int canWrite(int i){
	return open_files[i].canWrite;
}

void setRead(int i, int state){
	open_files[i].canRead = state;
}

void setWrite(int i, int state){
	open_files[i].canWrite = state;
}

void BinSemaInit(bin_sema **s){
     /* Allocate space for bin_sema */
     *s = (bin_sema *) malloc(sizeof(bin_sema));     

     /* Init mutex */
     pthread_mutex_init(&((*s)->mutex), NULL); 

     /* Init cond. variable */
     pthread_cond_init(&((*s)->cv), NULL);     

     /* Set flag value */
     (*s)->flag = 1;            
  }

void BinSemaP(bin_sema *s){ 
     /* Try to get exclusive access to s->flag */
     pthread_mutex_lock(&(s->mutex));     

     /* Success - no other thread can get here unless
	the current thread unlock "mutex"              */

     /* Examine the flag and wait until flag == 1 */
     while (s->flag == 0)
     {
        pthread_cond_wait(&(s->cv), &(s->mutex) );
            /* When the current thread execute this
	       pthread_cond_wait() statement, the
	       current thread will be block on s->cv
	       and (atomically) unlocks s->mutex !!! 
	       Unlocking s->mutex will let other thread
	       in to test s->flag.... */
     }

     /* -----------------------------------------
        If the program arrive here, we know that 
	s->flag == 1 and this thread has now
	successfully pass the semaphore !!!
      ------------------------------------------- */
     s->flag = 0;  /* This will cause all other threads
		      that executes a P() call to wait 
		      in the (above) while-loop !!! */

     /* Release exclusive access to s->flag */
     pthread_mutex_unlock(&(s->mutex)); 
}

void BinSemaV(bin_sema *s){ 
     /* Try to get exclusive access to s->flag */
     pthread_mutex_lock(&(s->mutex));  

     pthread_cond_signal(&(s->cv));
	    /* This call may restart some thread that
	       was blocked on s->cv (in the P() call)
		- if there was not thread blocked on
		- cv, this operation does absolutely
		- nothing...                          */    

     /* Update semaphore state to Up */
     s->flag = 1;

     /* Release exclusive access to s->flag */
     pthread_mutex_unlock(&(s->mutex)); 
}

int getNumPorts(){
	int i = 0;
	int n = 0;
	for(i = 0; i < 10; i++){
		if (multiplexPorts[i].port == 0){
			n++;
		}
	}
	return n;
}

int getAndSetPort(){
	int i = 0;
	for(i = 0; i < 10; i++){
		if (multiplexPorts[i].port == 0){
			multiplexPorts[i].port = port + i;
			return multiplexPorts[i].port;
		}
	}
	return -1;
}


void makeMultiplex(int nport){

	int rqst;						// socket accepting the request
	socklen_t alen;       			// length of address structure
	struct sockaddr_in my_addr;   	// address of this service
	struct sockaddr_in client_addr; // client's address
	alen = sizeof(client_addr);
	int sockoptval = 1;
	int svc;


	/* create a TCP/IP socket */
	if ((svc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("cannot create socket");
		exit(1);
	}

	/* allow immediate reuse of the port */
	setsockopt(svc, SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof(int));

	/* bind the socket to our source address */
	memset((char*)&my_addr, 0, sizeof(my_addr));  	// 0 out the structure
	my_addr.sin_family = AF_INET;   				// address family
	my_addr.sin_port = htons(nport);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(svc, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) {
		perror("bind failed");
		exit(1);
	}

	// set the socket for listening (queue backlog of 10)
	if (listen(svc, 1) < 0) {
		perror("listen failed");
		exit(1);
	}

	// loop, accepting connection requests
		while ((rqst = accept(svc, (struct sockaddr *) &client_addr, &alen)) < 0) {
			/* we may break out of accept if the system call */
            /* was interrupted. In this case, loop back and */
            /* try again */
            if ((errno != ECHILD) && (errno != EINTR)) {
                    perror("accept failed");
                    exit(1);
            }

        }
		// the socket for this accepted connection is rqst
        pthread_t thread;
        int ret;
        if((ret = pthread_create(&thread, NULL,(void*) &acceptThread, (void*) &rqst)) != 0){
        	errno = ret;
        	perror("pthread_create");
        	exit(1);
        }

	return;
}







