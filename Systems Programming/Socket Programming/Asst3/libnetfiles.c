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
#include "libnetfiles.h"


int file_mode;
int port = 10302;
char file_name[50];
int server_fd;
int isHostSet;
struct hostent *hp; 	   		//host information
struct sockaddr_in servaddr;    //server address


int netserverinit(char *hostname, int filemode){

	if(filemode != unrestricted && filemode != exclusive && filemode != transaction){
		errno = INVALID_FILE_MODE;
		perror("bad filemode");
		return -1;
	}

	hp = gethostbyname(hostname);
	if(!hp){
		h_errno = HOST_NOT_FOUND;
		herror("hostname");
		return -1;
	}
	isHostSet = 1;

	// Set up open files Global Linked List
	int i = 0;
	for(i = 0; i < MAX_OPEN_FILES; i++){
		open_files[i].fd = 0;
		open_files[i].canRead = 0;
		open_files[i].canWrite = 0;
	}
	file_mode = filemode;

	return 0;

}



int netopen(const char *pathname, int flags){

	if(isHostSet == 0){
		h_errno = HOST_NOT_FOUND;
		herror("hostname");
		return -1;
	}
	
	// check for proper flag input
	if(flags != O_RDONLY && flags != O_WRONLY && flags != O_RDWR){
		exit(1);
	}

	// make socket connection
	int fd = makeSocket(port);

	// send command to server
	sendCommand(fd, threadopen);

	open_message *o_message = (open_message*) malloc(sizeof(open_message));
	o_message->file_mode = file_mode;
	strcpy(o_message->file_path, pathname);
	o_message->flag = flags;

	// Send server open message (includes file mode, path, and flags)
	if(write(fd, o_message, sizeof(open_message)) == -1){
		errno = EINTR;
		perror("netopen");
		close(fd);
		return -1;
	}


	// Allocate return message memory
	open_return_message *or_message = (open_return_message*) malloc(sizeof(open_return_message));

	// Wait for open return message
	if(read(fd, or_message, sizeof(open_return_message)) == -1){
		errno = EINTR;
		perror("netopen");
		close(fd);
		return -1;
	}


	if(or_message->fd == -1){
		errno = or_message->errorNum;
		perror("netopen");
		free(o_message);
		free(or_message);
		close(fd);
		return -1;
	}
	else{
		int netfd = or_message->fd;
		addFile(netfd, flags);
		free(o_message);
		free(or_message);
		close(fd);
		return netfd;
	}

}



ssize_t netread(int fildes, void *buf, size_t nbyte){

	if(isHostSet == 0){
		h_errno = HOST_NOT_FOUND;
		herror("hostname");
		return -1;
	}

	if(!canRead(fildes)){
		errno = EBADF;
		perror("netread");
		return -1;
	}

	// make socket connection
	int fd = makeSocket(port);


	// send command to server
	sendCommand(fd, threadread);


	// Initialize read message
	read_message *r_message = (read_message*) calloc(1, sizeof(read_message));
	r_message->fd = fildes;
	r_message->size = nbyte;

	// Send server read message (includes file descriptor and size of read)
	if(write(fd, r_message, sizeof(read_message)) < 0){
		perror("message write");
		return -1;
	}

	// Allocate return message memory
	read_return_message *rr_message = (read_return_message*) calloc(1, sizeof(read_return_message));

	// Wait for read return message
	if(read(fd, rr_message, sizeof(read_return_message)) < 0){
		perror("return message read");
		return -1;
	}


	// if multiplexing is on, get multiplex message
	if(rr_message->multiplexOn == 1){
		buf = (char*)'\0';
		// Allocate memory for multiplex  message
		multiplex_message *m_message = (multiplex_message*) calloc(1, sizeof(multiplex_message));
		if(read(fd, m_message, sizeof(multiplex_message)) < 0){
			perror("multiplex message read");
			return -1;
		}
		char *buff = malloc(nbyte);
		int n = 0;
		read_message **x_message = (read_message**) calloc(1,10* sizeof(read_message));
		read_return_message **mr_message = (read_return_message**) calloc(1,10* sizeof(read_return_message));
		for(n = 0; n < m_message->numPorts; n++){

			// make socket connection
			int msock_fd = makeSocket(m_message->port[n]);

			// send command to server
			sendCommand(fd, threadread);


			x_message[n]->fd = fildes;
			x_message[n]->offset = 0;
			if(m_message->chunkSize[n] > 2000){
				int c = m_message->chunkSize[n];
				while(c > 0){
					if(c > 2000){
						x_message[n]->size = 2000;
						write(msock_fd,x_message[n], sizeof(read_message));
						read(msock_fd, mr_message[n], sizeof(read_return_message));
						strcat(buf, mr_message[n]->buff);
						x_message[n]->offset+= 2000;
					}
					else{
						x_message[n]->size = c;
						write(msock_fd,x_message[n], sizeof(read_message));
						read(msock_fd, mr_message[n], c);
						strcat(buf, mr_message[n]->buff);
					}
					c-=2000;
				}
			}
			else{
				read(msock_fd, mr_message[n]->buff, m_message->chunkSize[n]);
				strcat(buf, mr_message[n]->buff);
			}

		}
		free(buff);
		free(mr_message);
		return nbyte;
	}

	if(rr_message->errorNum != 0){
		errno = rr_message->errorNum;
		perror("netread");
		free(r_message);
		free(rr_message);
		close(fd);
		return -1;
	}
	else{
		int numRead = rr_message->numCharRead;
		strcpy(buf, rr_message->buff);
		free(r_message);
		free(rr_message);
		close(fd);
		return numRead;
	}

}


ssize_t netwrite(int fildes, const void *buf, size_t nbyte){

	if(isHostSet == 0){
		h_errno = HOST_NOT_FOUND;
		herror("hostname");
		return -1;
	}

	if(!canWrite(fildes)){
		errno = EBADF;
		perror("netwrite");
		return -1;
	}

	// make socket connection
	int fd = makeSocket(port);

	// send command to server
	sendCommand(fd, threadwrite);

	// Initialize write message
	write_message *w_message = (write_message*) calloc(1, sizeof(write_message));
	w_message->fd = fildes;
	strcpy(w_message->buff, buf);
	w_message->size = nbyte;

	// Send server write message (includes file descriptor, buffer, and size of read)
	if(write(fd, w_message, sizeof(write_message)) < 0){
		perror("message write");
		return -1;
	}

	// Allocate return message memory
	write_return_message *wr_message = (write_return_message*) calloc(1, sizeof(write_return_message));

	// Wait for read return message
	if(read(fd, wr_message, sizeof(write_return_message)) < 0){
		perror("return message read");
		return -1;
	}

	if(wr_message->errorNum != 0){
		errno = wr_message->errorNum;
		perror("netwrite");
		free(w_message);
		free(wr_message);
		close(fd);
		return -1;
	}
	else{
		int numWrote = wr_message->numCharWrote;
		free(w_message);
		free(wr_message);
		close(fd);
		return numWrote;
	}
}

int netclose(int fd){
	
	if(isHostSet == 0){
		h_errno = HOST_NOT_FOUND;
		herror("hostname");
		return -1;
	}

	// 
	if(canRead(fd) == -1){
		errno = EBADF;
		perror("netclose");
		return -1;
	}

	// make socket connection
	int sock_fd = makeSocket(port);

	// send command to server
	sendCommand(sock_fd, threadclose);

	// Initialize close message
	close_message *c_message = (close_message*) calloc(1, sizeof(close_message));
	c_message->fd = fd;
	c_message->mode = file_mode;
	c_message->flag = getFlag(fd);

	// Send server close message (includes file descriptor)
	write(sock_fd, c_message, sizeof(close_message));

	// Allocate return message memory
	close_return_message *cr_message = (close_return_message*) calloc(1, sizeof(close_return_message));

	// Wait for close return message
	read(sock_fd, cr_message, sizeof(close_return_message));


	if(cr_message->errorNum != 0){
		errno = cr_message->errorNum;
		perror("netclose");
		free(c_message);
		free(cr_message);
		close(sock_fd);
		return -1;
	}
	else{
		free(c_message);
		free(cr_message);
		close(sock_fd);
		removeFile(fd);
		return 0;
	}
}

int makeSocket(int port){

	// make socket
	int fd;
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Problem in creating the socket");
		return -1;
	}

	/* fill in the server's address and data */
	struct sockaddr_in myaddr;
	memset((char *) &myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(port);

	/* put the host's address into the server address structure */
	memcpy( (void *) &servaddr.sin_addr, hp->h_addr_list[0], hp->h_length);
	servaddr.sin_port = htons(port);

	/* connect to server */
	if (connect(fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		herror("connect failed");
		return -1;
	}
	return fd;
}

// Send given command to given socket
void sendCommand(int fd, int command){

	accept_message *a_message = (accept_message*) calloc(1, sizeof(accept_message));
	a_message->command = command;

	// Send server open message (includes file mode, path, and flags)
	write(fd, a_message, sizeof(accept_message));
	free(a_message);
	return;

}

int addFile(int fd, int flag){

	int i = 0;
	for(i = 0; i < MAX_OPEN_FILES; i++){
		if(open_files[i].fd == 0){
			open_files[i].fd = fd;
			if(flag == O_RDONLY){
				open_files[i].canRead = 1;
				open_files[i].canWrite = 0;
			}
			else if(flag == O_WRONLY){
				open_files[i].canRead = 0;
				open_files[i].canWrite = 1;
			}
			else if(flag == O_RDWR){
				open_files[i].canRead = 1;
				open_files[i].canWrite = 1;
			}
			return i;
		}
	}

	return -1;

}

int removeFile(int fd){

	int i = 0;
	for(i = 0; i < MAX_OPEN_FILES; i++){
		if(open_files[i].fd == fd){
			open_files[i].fd = 0;
			open_files[i].canRead = 0;
			open_files[i].canWrite = 0;
		}
	}
	return -1;
}

int canRead(int fd){
	int i = 0;
	for(i = 0; i < MAX_OPEN_FILES; i++){
		if(open_files[i].fd == fd){
			return open_files[i].canRead;
		}
	}
	return -1;
}

int canWrite(int fd){
	int i = 0;
	for(i = 0; i < MAX_OPEN_FILES; i++){
		if(open_files[i].fd == fd){
			return open_files[i].canWrite;
		}
	}
	return -1;
}

int getFlag(int fd){
	int i = 0;
	for(i = 0; i < MAX_OPEN_FILES; i++){
		if(open_files[i].fd == fd){
			if(open_files[i].canRead == 1 && open_files[i].canWrite == 0){
				return O_RDONLY;
			}
			else if(open_files[i].canRead == 0 && open_files[i].canWrite == 1){
				return O_WRONLY;
			}
			else if(open_files[i].canRead == 1 && open_files[i].canWrite == 1){
				return O_RDWR;
			}
		}
	}
	return -1;
}

int main(){
  printf(" test 1 ======= 10pts===========\n");
  printf(" netserverinit(128.6.13.234)3pts\n");
  netserverinit("128.6.13.234", 2);
  getchar();/*
  printf(" netserverinit(derp.cs.rutgers.edu)2pts\n");
  netserverinit("derp.cs.rutgers.edu", 2);
  getchar();
  printf(" netserverinit(   )2pts\n");
  netserverinit("", 2);
  getchar();*/
  printf(" netserverinit(basic.cs.rutgers.edu)3pts\n");
  netserverinit("basic.cs.rutgers.edu", 2);
  getchar();

  printf(" test 2 ======= 10pts===========\n");
  printf("netopen\n");
  int netfd = netopen("./file.txt", O_RDWR);
  printf("netfd is %d\n", netfd);
  getchar();

  printf(" test 3 ======= 20pts===========\n");

  char buf[200]= "This is a test! From test!";
  netwrite(netfd, buf, 25);
  printf("Write is buf = %s\n", buf);
  getchar();

  bzero(buf,200);
  netread(netfd, buf, 25);
  printf("Read is buf = %s\n", buf);
  getchar();

  return 0;
}
