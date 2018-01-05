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
// #include "stdafx.h" 
// #include "windows.h"
#include "libnetfiles.h"


int file_mode;
int port = 10302;
char file_name[50];
int file_des;
struct hostent *hp;     		//host information
struct sockaddr_in servaddr;    //server address



typedef struct message {
	int is_error;
	int error_num;
	int command;
	int num_bytes;
} message;


// void paddr(unsigned char *a){

// 	printf("%d.%d.%d.%d\n",a[0],a[1],a[2],a[3]);

// }

int netserverinit(char *hostname, int filemode){

	hp = gethostbyname(hostname);
	if(!hp){
		h_errno = HOST_NOT_FOUND;
		herror(hostname);
		return -1;
	}

	file_mode = filemode;

	return 0;

}




int netopen(const char *pathname, int flags){
	
	// check for proper flag input
	if(flags != O_RDONLY && flags != O_WRONLY && flags != O_RDWR){
		printf("Error: Invalid flag input\n");
		exit(1);
	}

	// make socket
	int fd;
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		errno = EINTR;
		perror("Problem in creating the socket");
		return -1;
	}

	printf("Made socket, fd = %d\n",fd);

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
		// errno = EINTR;
		perror("connect failed");
		return -1;
	}

	char buff[100];
	int numRead = read(fd, buff, sizeof(message));
	printf("Message from server: %s\n", buff);


	return fd;


}

ssize_t netread(int fildes, void *buf, size_t nbyte){
	



	return 0;


}

ssize_t netwrite(int fildes, const void *buf, size_t nbyte){
	



	return 0;


}

int netclose(const char *pathname, int flags){
	



	return 0;


}









int main(int argc, const char *argv[]){

	printf("client\n");

	int x = netserverinit("basic.cs.rutgers.edu", exclusive);
	printf("netserverinit return value = %d\n",x);

	int netfd = netopen("/.autofs/ilab/ilab_users/deymious/test.c", O_RDWR);
	printf("netopen return value = %d\n",netfd);
	
	int netfd2 = netopen("/.autofs/ilab/ilab_users/deymious/test2.c", O_RDWR);
	printf("netopen2 return value = %d\n",netfd2);	



	// printf("File mode: %d\n", file_mode);


	printf("\n");
	return 0;




}
