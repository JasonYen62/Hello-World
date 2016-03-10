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

class TCPSocket
{
public:
	TCPSocket(void){ sockfd = NULL;};
	~TCPSocket(void){ tcpClose();};

	bool  Initial(char ip[],short port,bool bBlock = false);
	void  Send(char data[],int nbyte);
	bool  ReadtoCRLF(int *nByte,char* data,int msWait);
	void  tcpClose();

private:
	int 	sockfd;
	//struct 	sockaddr_in address;  // remote address
	//struct 	hostent *hostinfo;
	//struct 	servent *servinfo;
};

#endif

