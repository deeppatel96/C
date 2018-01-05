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
#include "netfileserver.h"
#include <pthread.h>

// use -pthread to link pr -lpthread


extern int h_errno;
extern int errno;

typedef struct message {
	int is_error;
	int error_num;
	int command;
	int num_bytes;
} message;



int main(int argc, char *argv[]){

	printf("server\n");

	int port = 10302;				// port number
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

            message status = {0,0,0,0};
            write(rqst, &status, sizeof(message));

         }
		// the socket for this accepted connection is rqst

	}












}











