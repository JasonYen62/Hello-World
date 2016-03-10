/* rs232.h */

#ifndef ____RS232H____
#define ____RS232H____

typedef unsigned char BYTE;
#define SOH    0x01
#define STX    0x02
#define ETX    0x03
#define EOT    0x04
#define ENQ    0x05
#define ACK    0x06
#define BS     0x08
#define HT     0x09
#define LF     0x0A
#define FF     0x0C
#define CR     0x0D
#define SO     0x0E
#define SI     0x0F
#define DLE    0x10
#define DC2    0x12
#define DC3    0x13
#define DC4    0x14
#define NAK    0x15
#define CAN    0x18
#define ESC    0x1B
#define US     0x1F
#define SPACE  0x20


enum COMPORT
{
	COM1,
	COM2,
	COM3,
	COM4,
	COM5,
	COM6,
	COM7,
	COM8,
	NONE = 99
};

extern enum COMPORT SerialPort[9];

class RS232
{
public:	
	RS232(void) { m_portHandle = NULL;};
	~RS232(void);
	
	void PortClose(void);
	void out(BYTE c);
	void PortWrite(char *datas,short nbyte);
	void PortWriteWithCRC16(char *datas,short nbyte);
	void PortWriteWithBCC(char *datas,short nbyte);
	bool PortRead(short *nByte, char *data, short wait);
	bool PortRead2( int *bytes, char *data, int wait);
	bool PortRead2CRLF( int *bytes, char *data, int wait);
	bool PortInit(enum COMPORT port, int baudrate, unsigned char parity, short databits, short stopbits);
	void SetRTS(bool On);
	bool GetCTS(void);
	bool GetDTR(void);
private:
	short GetCRC16(BYTE* puchMsg,  short usDataLen);	
	char Port[20];
	int m_portHandle;
	unsigned long m_SendGap;
};

#endif

