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
#include <pthread.h>

bool volatile bStopThread = false;
void *print_message_function( void *msg );
bool volatile bActiveMode = false;

TCPSocket ARTGate;
RU25Lib art3x0;


int main( int argc, char *argv[] )
{
    char buffer[MAX_COMMAND_SIZE];	
    int  i, n;
	int  flags;	
    pthread_t thread_Reader;
	int  iret1;
    
	if(ARTGate.InitialServer(10001) == false)
	{
		printf("Initial Server Error\n");
		return 1;
	};

	
	if(art3x0.Open(COM1) != 0)
	{
		printf("COM1 Can't connect. \n");
		ARTGate.tcpClose();
		return 1;
	}
	
	iret1 = pthread_create( &thread_Reader, NULL, print_message_function, NULL);
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
				if(n==0)
				{
					break;
				}
			};
			if(n>0)
			{
				if(strstr(buffer,"set readmode 2") >= 0)
				{
					bActiveMode = true;
				}
				else if(strstr(buffer,"set readmode 0") >= 0)
				{
					bActiveMode = false;
				}
				printf("Command: [%s], Len:%d \n",buffer,n);		
				art3x0.SendCommand(buffer,n);				
			}
			usleep(5000);
			
		}
		printf("Client Closed.\n");
	}
	bStopThread = true;
	pthread_join( thread_Reader, NULL);

	art3x0.CloseReader();
	ARTGate.tcpClose();
    return 0; 
}

void *print_message_function( void *msg)
{
	char Response[2048];
	int n;
	
	while(bStopThread == false)
	{
		bzero(Response,2048);
		if(bActiveMode == true)
		{
			n = art3x0.ActiveReceive((char*)(&Response));
		}
		else
		{
			n = art3x0.ReadResponse((char*)(&Response));
		}
		
		if(n > 0)
		{
			printf("Response: [%s] Len:%d\n",Response,n);			
			ARTGate.SendtoClient(Response,n);
		}
		usleep(1000);
	}
}
