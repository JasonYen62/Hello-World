#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 		10001
#define DIRSIZE 	8192

int main(void)
{
    char     dir[DIRSIZE];  /* used for incomming dir name, and
					outgoing data */
	int 	 sd, sd_current, cc, fromlen, tolen;
	size_t 	 addrlen;
	struct   sockaddr_in sin;
	struct   sockaddr_in pin;
 
	/* get an internet domain socket */
	printf("Socket init.\n");
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("Socket error\n");
		return 1;
	}

	/* complete the socket structure */
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(PORT);

	/* bind the socket to the port number */
	printf("Bind\n");
	if (bind(sd, (struct sockaddr *) &sin, sizeof(sin)) == -1) {
		printf("bind error\n");
		return 1;
	}

	/* show that we are willing to listen */
	printf("Listen\n");
	if (listen(sd, 5) == -1) {
		printf("listen error\n");
		return 1;
	}
	/* wait for a client to talk to us */
	printf("Accept\n");
    addrlen = sizeof(pin); 
	if ((sd_current = accept(sd, (struct sockaddr *)  &pin, &addrlen)) == -1) {
		printf("accept error\n");
		return 1;
	}
/* if you want to see the ip address and port of the client, uncomment the 
    next two lines */

       /*
printf("Hi there, from  %s#\n",inet_ntoa(pin.sin_addr));
printf("Coming from port %d\n",ntohs(pin.sin_port));
        */
	printf("wait recv:\n");
	/* get a message from the client */
	if (recv(sd_current, dir, sizeof(dir), 0) == -1) {
		printf("Recv error\n");
		return 1;
	}
	printf("recv:[%s]\n",dir);
        /* get the directory contents */
        // read_dir(dir);
    
    /* strcat (dir," DUDE"); */
	/* acknowledge the message, reply w/ the file names */
	sprintf(dir,"1102");
	if (send(sd_current, dir, strlen(dir), 0) == -1) {
		printf("Send error\n");
		return 1;
	}

    /* close up both sockets */
	close(sd_current); 
	close(sd);
        
    /* give client a chance to properly shutdown */
    usleep(1000);
}
