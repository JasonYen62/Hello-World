/* TCP/IP Transaction function Header file */
#ifndef _____NETWORK_____
#define _____NETWORK_____

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <map>
#include <utility> // make_pair

using namespace std;
#define MAX_COMMAND_SIZE  	128

class TCPSocket
{
public:
	TCPSocket(void);
	~TCPSocket(void){ tcpClose();};

	bool  Initial(char ip[],short port,bool bBlock = false);
	bool  InitialServer(short port);
	void  Send(char data[],int nbyte);
	void  SendtoClient(char data[],int nbyte);
	bool  ReadtoCRLF(int *nByte,char* data,int msWait);
	bool  Read(int *nByte,char* data);
	bool  Accept(void);
	void  tcpClose();

private:
	int 	sockfd;
	int     newsockfd;
	struct 	sockaddr_in cli_addr;  // client address
	//struct 	hostent *hostinfo;
	//struct 	servent *servinfo;
};

#endif

