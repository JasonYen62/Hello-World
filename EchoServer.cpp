#include <stdio.h>
#include <sys/types.h> 
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <ctime>
#include "Network.h"
#include "RU25Lib.h"


int main( int argc, char *argv[] )
{
    char buffer[MAX_COMMAND_SIZE];
	char Response[2048];    
    int  i, n;
	int  flags;
	clock_t start, finish;
    TCPSocket ARTGate;
    
	if(ARTGate.InitialServer(10001) == false)
	{
		printf("Initial Server Error\n");
		return 1;
	};
	printf("Echo Server Start....\n");
	while(1)
	{		
		/* Accept actual connection from the client */
		if(ARTGate.Accept() == false)
		{
			printf("accept error\n");
			break;
		}
		
		while(1)
		{	
			start = clock();
			/* If connection is established then start communicating */
			//Read
			bzero(buffer,MAX_COMMAND_SIZE);
			if(ARTGate.Read(&n,buffer) == false)
			{	// Client is closed
				if(n==0)
				{
					break;
				}
			};
			if(n>0)
			{
				printf("[%s], Len:%d \n",buffer,n);
				ARTGate.SendtoClient(buffer,n);
			}
			finish = clock();
	
		}
		printf("Client Closed.\n");
	}

	ARTGate.tcpClose();
    return 0; 
}

//arm-linux-g++ -o EchoServer.bin -lpthread  rs232_ubc.o Network.o RU25Lib.o EchoServer.o