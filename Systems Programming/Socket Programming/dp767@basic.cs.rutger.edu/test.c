#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

void paddr(unsigned char *a){

	printf("%d.%d.%d.%d\n",a[0],a[1],a[2],a[3]);
}


int main(int argc, const char* argv[]){

	struct hostent *hp;
	char *host = "indiajijhbjhb.com";
	int i;

	hp = gethostbyname(host);
	if(!hp){
		fprintf(stderr, "could not obtain address of %s\n", host);
		return 0;
	}
	for(i = 0; hp->h_addr_list[i] != 0; i++){
		paddr((unsigned char*) hp->h_addr_list[i]);
	}
	exit(0);


}