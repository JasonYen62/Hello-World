
/*
  File:  Network.cpp

  Author(s):  Jason Yen, Advantech Corporation

  Description:
  ART3x0 Reader TCP/IP Function for Linux 
  
============================================ Modify Hisory Label ===============================================
Modify      	Date        	Label       Purpose
----------------------------------------------------------------------------------------------------------------
Jason Yen   	2011/08/11  	A.001       Start 

*/

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include "Network.h"


TCPSocket::TCPSocket(void)
{
	sockfd = NULL;
	newsockfd = NULL;
}

void TCPSocket::tcpClose()
{
    if(sockfd != NULL)
    {
        close(sockfd);
    }
	
	if(newsockfd != NULL)
    {
        close(newsockfd);
    }
}

bool TCPSocket::Initial(char ip[],short port,bool bBlock)
{
    int flags;
    int portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server = NULL;
	fd_set fdset;
	struct timeval tv;
	char cData[1024];
  
    portno = 10001;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("ERROR opening socket. \n");
		return false;
    }

	printf("IP:%s port:%d\n",ip,port);
    server = gethostbyname(ip);
    if (server == NULL) {
        printf("ERROR, no such host\n");
        return false;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(ip);
	serv_addr.sin_port = htons(portno);;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    //set non block
    if(bBlock != true)
    {
        flags = fcntl(sockfd, F_GETFL,0);
        flags |= (O_NONBLOCK);
    //    fcntl(sockfd, F_SETFL, flags);
	//	fcntl(sockfd, F_SETFL, O_NONBLOCK); 
    }

    serv_addr.sin_port = htons(portno);

    tv.tv_sec = 0;
    tv.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,  sizeof tv))
    {
		perror("setsockopt");
        return -1;
	}

	n = connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr));
	
    if (n < 0)
    {
        printf("ERROR connecting. ret:%d\n",n);
		return false;
    }
	//printf("sockfd= %d\n",sockfd);
	//read(sockfd, cData,1024);
	//printf("TEMP:%s.\n",cData);
    return true;
}

void TCPSocket::Send(char data[],int nbyte)
{
	if(sockfd == NULL)
	{
		printf("No connect!!\n");
		return ;
	}
	printf("write [%s]\n",data);
    int n = write(sockfd,data,nbyte);
    if (n < 0)
    {
        //error("ERROR writing to socket");
		printf("ERROR writing to socket");
    }
}


bool TCPSocket::ReadtoCRLF(int *nByte,char* data,int msWait)
{
    int   timeout=0, retry = 0;	
    char  cData[2];
    bool  bAlreadyRecv = false;

	if(sockfd == NULL)
		return false;
	
    *nByte = 0;
	memset(cData,'\0',sizeof(cData));
    while(1)
    {
		if(read(sockfd, cData,1) >0)
		{
			data[(*nByte)] = cData[0];
			
			if( cData[0] == 0x0D || cData[0] == 0x0A)
			{
				data[(*nByte)] = '\n';
				read(sockfd, cData,1);
				(*nByte)++;
				data[(*nByte)] = 0;
				break;
			}
			else
			{
				(*nByte)++;
			}
		}
		else
		{
			usleep(1);
			timeout++;
			if(timeout > 5000)
			{
				printf("Read rj45 timeout!\n");
				break;
			}
		}
    }

    if((*nByte) <1)
        return false;

    return true;
}

bool TCPSocket::Read(int *nByte,char* data)
{
	int n=0;
	
	*nByte=0;

	n = read( newsockfd, data, MAX_COMMAND_SIZE-1 );
	//printf("n=%d ",n);
	if (n < 0)
	{
		perror("ERROR reading from socket");
		return false;
	}
	if(n == 0) 
	{	// Client is closed
		return false;
	}
	*nByte = n;
	return true;
}

/* 
	====== Server Code ======
	
*/
bool TCPSocket::InitialServer(short port)
{
	struct sockaddr_in serv_addr;
	/* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        perror("ERROR opening socket");
        return false;
    }
    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    //portno = port;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
 

    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR on binding");
		if(sockfd != NULL)
		{
			close(sockfd);
		}
        return false;
    }

    /* Now start listening for the clients, here process will
    * go in sleep mode and will wait for the incoming connection
    */
    if(listen(sockfd,5) != 0)
	{
		perror("ERROR on Listen");
		if(sockfd != NULL)
		{
			close(sockfd);
		}
		return false;
	};
	return true;
}

bool TCPSocket::Accept(void)
{
	int clilen = sizeof(cli_addr);
	
	newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (size_t *)&clilen);
	if (newsockfd < 0) 
	{
		perror("ERROR on accept");
		return false;
	}
	return true;
}

void TCPSocket::SendtoClient(char data[],int nbyte)
{
	if(newsockfd == NULL)
	{
		printf("No connect!!\n");
		return ;
	}
	//printf("write [%s]\n",data);
    int n = write(newsockfd,data,nbyte);
    if (n < 0)
    {
        //error("ERROR writing to socket");
		printf("ERROR writing to socket");
    }
}