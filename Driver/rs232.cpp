/*
  File:  rs232.cpp

  Author(s):  Jason Yen, Advantech Corporation

  Description:
  RS232 for Linux 
  
============================================ Modify Hisory Label ===============================================
Modify      	Date        	Label       Purpose
----------------------------------------------------------------------------------------------------------------
Jason Yen   	2011/08/11  	            Start 
*/

#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <sys/ioctl.h>
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */


#include "rs232.h"

//#define UBCDS31
enum COMPORT SerialPort[9] = {NONE,COM1,COM2,COM3,COM4,COM5,COM6,COM7,COM8};

/* Table of CRC values for high-order byte */ 
static BYTE auchCRCHi[] = { 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40 
}; 
 
/* Table of CRC values for low-order byte */ 
static BYTE auchCRCLo[] = { 
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 
0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 
0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 
0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 
0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 
0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 
0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 
0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 
0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 
0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 
0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 
0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 
0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 
0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 
0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 
0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 
0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 
0x43, 0x83, 0x41, 0x81, 0x80, 0x40 
}; 

RS232::~RS232(void)
{
	PortClose();
}

void RS232::out(BYTE c)
{
    if(m_portHandle == NULL)
        return;
	short n;
	n= write(m_portHandle, &c, 1);	
}

bool RS232::PortRead( short *bytes, char *data, short wait)
{   // wait : msec
	int timeout;
	int nbyte=0;
  
    for (timeout = 0; timeout < wait/10; timeout++) 
	{	
		usleep(10000);
		nbyte = read(m_portHandle, data,1024);
		
		if (nbyte > 1)
			break;	
    }
	*bytes = nbyte;
	
    if(nbyte <1)
		return false;
    return true;
}

bool RS232::PortRead2( int *bytes, char *data, int wait)
{   // wait : msec  // For performance
	int timeout,retry=0;	
	char  c;
	bool bAlreadyRecv = false;
    
	*bytes = 0;
    for (timeout = 0; timeout < wait; timeout++) 
	{
		while( read(m_portHandle, &c,1) == 1)
		{
			data[(*bytes)] = c;
			//printf("<%02x>",c);
			if((*bytes) >1)
			{
				if(c == 0x0a && data[(*bytes)-1]==0x0a) 
				{
					data[(*bytes)-1] = 0x0d;
				}
			}
			(*bytes)++;
			bAlreadyRecv = true;
			retry=0;
		}
		usleep(999);
		if(bAlreadyRecv == true)
		{
			retry++;
			if(retry > 5)
				break;
		}
    }

    if((*bytes) <1)
		return false;
	//tcflush(m_portHandle, TCIOFLUSH);
	//usleep(3000);
    return true;
}

bool RS232::PortRead2CRLF( int *bytes, char *data, int wait)
{   // wait : msec  // For performance
	int timeout,retry=0;	
	char  c;
	bool bAlreadyRecv = false;
	    
	*bytes = 0;
    for (timeout = 0; timeout < wait; timeout++) 
	{
		while( read(m_portHandle, &c,1) == 1)
		{
			data[(*bytes)] = c;
			(*bytes)++;
			//printf("<%02x>",c);
			bAlreadyRecv = true;
			retry = 0;
			if( c == CR || c == LF)
			{
				read(m_portHandle, &c,1); //Read LF
				retry = 6;
				
				data[(*bytes)-1] = 0x0d;
				data[(*bytes)++] = c;
				
				//printf("_%02x_",c);
				data[(*bytes)]=0;
				break;
			}
		}
		usleep(1000); //1 ms
		if(bAlreadyRecv == true)
		{
			retry++;
			if(retry > 5)
				break;
		}
    }

    if((*bytes) <1)
		return false;
	//tcflush(m_portHandle, TCIOFLUSH);
	//usleep(3000);
    return true;
}

void RS232::PortWrite( char *chars ,short nByte) 
{
	int i =0;
    if(m_portHandle == NULL)
        return;
	tcflush(m_portHandle, TCIOFLUSH);
	for(i=0;i<nByte;i++)
	{	
		usleep(m_SendGap);
		write(m_portHandle, &(chars[i]), 1);	
	}
}

void RS232::PortWriteWithCRC16( char *chars ,short nByte) 
{
	int i;
	BYTE uchCRCHi = 0xFF ;                   /* high CRC byte initialized */ 
	BYTE uchCRCLo = 0xFF ;                   /* low CRC byte initialized */ 
	unsigned uIndex ;                                /* will index shorto CRC lookup*/ 

	for(i=0; i<nByte; i++)
	{
		out(chars[i]);		
		uIndex = uchCRCHi ^ chars[i]; /* calculate the CRC */ 
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex]; 
		uchCRCLo = auchCRCLo[uIndex]; 
	}
	out(uchCRCLo);	
	out(uchCRCHi);	
	usleep(100);
}

void RS232::PortWriteWithBCC( char *chars ,short nByte) 
{	//There bcc is xor STX to ETX
	int i;
	unsigned char bcc=0x00;
	//printf("Send:");
	for(i=0; i<nByte; i++)
	{
		out(chars[i]);
		bcc ^= chars[i];
		//printf("%02x ",(unsigned char)chars[i]);
	}	
	out(bcc);
	//printf("%02x \n",(unsigned char)bcc);
	usleep(100);
}

bool RS232::PortInit(enum COMPORT port, int baudrate, unsigned char parity, short databits, short stopbits)
{
	char device[40];
	struct termios options;
	
    if(port>8) return false;
	m_SendGap = (unsigned long)(1.0/(baudrate/8.0) * 1000.0 * 1000.0);
	//printf("gap:[%3ld]\n",m_SendGap);
	
	memset(device,'\0',sizeof(device));
#ifndef UBCDS31	
	sprintf(device,"/dev/ttyS%d",port); // x86 , PCM3530
#else
	sprintf(device,"/dev/ttymxc%d",port); // UBC
#endif	
	//printf("Open %s\n",device);
	m_portHandle = open(device, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK );//
	if (m_portHandle == -1) 
	{
		printf("Open_port: Unable to open [%s]!!",device);
		return false;
	}
	else 
	{
		fcntl(m_portHandle, F_SETFL, 0);
	}
	
	// Get the current options for the port...	
	tcgetattr(m_portHandle, &options);
	// Set the baud rates 
	switch(baudrate)
	{
	default:	
	case 115200:
		//printf("Baud rate: 115200");
		cfsetospeed(&options, B115200);
		cfsetispeed(&options, B115200);
		break;
	case 9600:
		cfsetospeed(&options, B9600);
		cfsetispeed(&options, B9600);
		break;
	case 19200:
		cfsetospeed(&options, B19200);
		cfsetispeed(&options, B19200);
		break;
	case 38400:
		cfsetospeed(&options, B38400);
		cfsetispeed(&options, B38400);
		break;
	case 2400:
		cfsetospeed(&options, B2400);
		cfsetispeed(&options, B2400);
		break;
	case 1200:
		cfsetospeed(&options, B1200);
		cfsetispeed(&options, B1200);
		break;	
	}
	
	switch (parity)
	{
	default:
	case 'N':
        options.c_cflag &= ~PARENB; 
        break; 
	case 'O':
        options.c_cflag |= PARENB;        
        options.c_iflag |= (INPCK | ISTRIP); 
		options.c_cflag |= PARODD; 
        break; 
    case 'E':
		options.c_cflag |= PARENB; 
		options.c_iflag |= (INPCK | ISTRIP);        
        options.c_cflag &= ~PARODD; 
        break;     
	}
	
	switch(databits)
	{
	default:
	case 8:
		options.c_cflag |= CS8;
		break;
	case 7:
		options.c_cflag |= CS7;
		break;
	}
	
	if( stopbits == 1 )
	{
		options.c_cflag &= ~CSTOPB;
	}
    else 
	{
        options.c_cflag |=  CSTOPB;
	}
	// Enable the receiver and set local mode...
	//options.c_cflag |= (CLOCAL | CREAD |CRTSCTS );
	//options.c_cflag &= ~CSIZE;
	
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
	options.c_oflag &= ~OPOST;   /*Output*/
	options.c_cc[VTIME]= 0;// inter-char timer unused //read timeout 
	options.c_cc[VMIN]= 0;// blocking read //wait read n char to return
	// Set the new options for the port...	
	//usleep(20000);
	tcflush(m_portHandle, TCIOFLUSH);
	tcsetattr(m_portHandle, TCSANOW, &options);
	
	return true;
}

void RS232::PortClose()
{
	if (m_portHandle > 0)
    {
        close(m_portHandle);
    }
}

short RS232::GetCRC16(BYTE* puchMsg, short usDataLen)
{
	BYTE uchCRCHi = 0xFF ;                   /* high CRC byte initialized */ 
	BYTE uchCRCLo = 0xFF ;                   /* low CRC byte initialized */ 
	unsigned uIndex ;                                /* will index shorto CRC lookup*/ 
	/* table */ 
	while (usDataLen--)                               /* pass through message buffer */ 
	{ 
		uIndex = uchCRCHi ^ *puchMsg++; /* calculate the CRC */ 
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex]; 
		uchCRCLo = auchCRCLo[uIndex]; 
	} 
	return (uchCRCHi << 8 | uchCRCLo); 
}

void RS232::SetRTS(bool On)
{
	int status;
	
	ioctl(m_portHandle, TIOCMGET, &status); /* get the serial port status */	
	if ( On )      /* set the RTS line */
		status &= ~TIOCM_RTS;
	else
		status |= TIOCM_RTS;
	ioctl(m_portHandle, TIOCMSET, &status); /* set the serial port status */
}

bool RS232::GetCTS(void)
{
	int status;
	bool CTS_ON;
	
	/* get the serial port's status */
	ioctl(m_portHandle, TIOCMGET, &status);
	CTS_ON = (status&TIOCM_CTS ? false:true);
	if(CTS_ON)
	{
		printf("CTS ON\n");
	}
	else
	{
		printf("CTS  Off\n");
	}
	return (CTS_ON);
}

bool RS232::GetDTR(void)
{
	int status;
	bool DTR_ON;
	
	/* get the serial port's status */
	ioctl(m_portHandle, TIOCMGET, &status);
	DTR_ON = (status&TIOCM_DTR ? false:true);
	//if(DTR_ON)
	//	printf("DTR ON\n");
	//else
	//	printf("DTR Off\n");
	return (DTR_ON);
}
