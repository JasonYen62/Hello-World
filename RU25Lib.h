

#ifndef _____RU25LIB_____
#define _____RU25LIB_____

//Connect Device
#define NO_CONNECT      0
#define UART_CONNECT    1
#define RJ45_CONNECT    2

// Payload mask value
//Kill Password Payload Setting
#define KPSW_RWOPEN		0xC0000
#define KPSW_NOLOCK     0xC0100
#define KPSW_RWSECURED  0xC0200
#define KPSW_RWDISABE	0xC0300
//Access Password Payload Setting
#define APSW_RWOPEN		0x30000
#define APSW_NOLOCK     0x30040
#define APSW_RWSECURED  0x30080
#define APSW_RWDISABE	0x300C0
//EPC bank Payload Setting
#define EPC_RWOPEN		0x0C000
#define EPC_NOLOCK     	0x0C010
#define EPC_WSECURED  	0x0C020
#define EPC_WDISABE		0x0C030
//TID bank Payload Setting
#define TID_RWOPEN		0x03000
#define TID_NOLOCK     	0x03004
#define TID_WSECURED  	0x03008
#define TID_WDISABE		0x0300C
//USER bank Payload Setting
#define USER_RWOPEN		0x00C00
#define USER_NOLOCK     0x00C01
#define USER_WSECURED  	0x00C02
#define USER_WDISABE	0x00C03
#include "Driver/rs232.h"
#include "Driver/Network.h"

typedef struct strInventoryResult
{
	char ProtocolControl[5];	//PC
	char EPC[512];
	int  Attenna;
	int minRSSI;
	int maxRSSI;
	int avgRSSI;
}invResult;

#define MAX_INVENTORYTAGS 	64

class RU25Lib
{
private :
	int ConnectDevice;
	bool ReadOnePacket(char* datas);
	int TxFirmware(char Filename[]);
	unsigned short CRC16(char buf[],int len);
	void ParseInvData(int index, char* InvData);        // Parse and save to variable:m_InvResult
	RS232 serial;
	TCPSocket *rj45;
	
public:
	invResult m_InvResult[MAX_INVENTORYTAGS];
	int m_iTags;
	RU25Lib(void);
	~RU25Lib(void);

	int Open(enum COMPORT comport);	
	int Open(char IpAddress[]);
	int Open();
	void CloseReader();
	
	int SendCommand(char cmd[],int len);
	int ReadResponse(char* datas);
	int ActiveReceive(char* datas);
	int Inventory(char* result,int *nTag);
	int GetFirmwareVersion(char* result);
	int GetParameter(char* result, char ParamName[]);
	int ReadMemory(char* result,int bank,int nWord,char APassowrd[],int start);
	int WriteMemory(char* result,int bank,char APassowrd[],char wData[],int startWord=0);

	int ChangeAccessPassword(char APassowrd[],char NewAPassowrd[]);
	int ChangeKillPassword(char APassword[],char KPassowrd[]);
	int LockTag(char* result,char APassword[], unsigned int Payload);
	int KillTag(char* result,char KPassword[]);
	int SelectTag(char TagEPC[]);
	int deSelectTag();
	int SetPower(char value[]);
	int SetFrequencyEnable(char Freq[],bool Enable);
	int SetHoppingMode(char Mode[]);
	int SetParameter(char ParmName[],char ParaValue[]);
	int SaveParameter(char* result);
	
	int UpdateFirmware(char* result,char Filename[]);
	int Reboot(char* result);
	
	bool GetNearestEPC(char* EPC, int validRSSI);
	void WriteLog(char* message);
};

#endif


