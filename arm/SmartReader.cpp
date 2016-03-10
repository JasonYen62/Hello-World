#include <stdio.h>
#include <sys/types.h> 
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "Network.h"
#include "RU25Lib.h"


int main( int argc, char *argv[] )
{
    char buffer[MAX_COMMAND_SIZE];
	char Response[2048];    
    int  i, n;
	int  flags;

    TCPSocket ARTGate;
    
	if(ARTGate.InitialServer(10001) == false)
	{
		printf("Initial Server Error\n");
		return 1;
	};

	RU25Lib art3x0;
	if(art3x0.Open(COM1) != 0)
	{
		printf("COM1 Can't connect. \n");
		ARTGate.tcpClose();
		return 1;
	}
	
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
			/* If connection is established then start communicating */
			//Read
			bzero(buffer,MAX_COMMAND_SIZE);
			if(ARTGate.Read(&n,buffer) == false)
			{	// Client is closed
				break;
			};
			if(n == 0)
			{
				continue;
			}
			//printf("Command: [%s], Len:%d \n",buffer,n);		
			art3x0.SendCommand(buffer,n);
		
			bzero(Response,2048);			
			n = art3x0.ReadResponse((char*)(&Response));
			//printf("Response: [%s] Len:%d",Response,n);
			
			ARTGate.SendtoClient(Response,n);
		}
	}
	art3x0.CloseReader();
	ARTGate.tcpClose();
    return 0; 
}
