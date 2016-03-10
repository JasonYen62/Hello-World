

#ifndef _____RU25LIB_____
#define _____RU25LIB_____

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

//Connect Device
#define NO_CONNECT      0
#define UART_CONNECT    1
#define RJ45_CONNECT    2

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
	RS232 serial;										// RS232 component
	TCPSocket *rj45;									// TCP/IP component
	
public:
	invResult m_InvResult[MAX_INVENTORYTAGS];			// Inventory rResult
	int m_iTags;										// Inventory Tag number
	
	RU25Lib(void);
	~RU25Lib(void);

	int Open(enum COMPORT comport);						// Connect Reader With RS232
	int Open(char IpAddress[]);							// Connect Reader with TCP/IP
	int Open();	// open by config file "ART3x0.cfg"
	void CloseReader();
	
	int SendCommand(char cmd[],int len);
	int ReadResponse(char* datas);
	
	int Inventory(char* result,int *nTag);				// Inventory Tags
	int GetFirmwareVersion(char* result);				// Get Firmware Version Infomation
	int GetParameter(char* result, char ParamName[]);	// Get Parameter value
	int ReadMemory(char* result,int bank,int nWord,char APassowrd[],int start);	// Read User Memory from a Tag
	int WriteMemory(char* result,int bank,char APassowrd[],char wData[],int startWord=0); 	// write User memory to a Tag

	int ChangeAccessPassword(char APassowrd[],char NewAPassowrd[]);	// Change Access Password
	int ChangeKillPassword(char APassword[],char KPassowrd[]);		// Change Kill Password
	int LockTag(char* result,char APassword[], unsigned int Payload);	// Lock Tag memory
	int KillTag(char* result,char KPassword[]);							// Kill a Tag
	int SelectTag(char TagEPC[]);										// Select a Tag by EPC
	int deSelectTag();													// cancel Select
	int SetPower(char value[]);											// Set Internal RF Power output
	int SetFrequencyEnable(char Freq[],bool Enable);					// Enable/Disable a Frequency
	int SetHoppingMode(char Mode[]);		
	int SetParameter(char ParmName[],char ParaValue[]);					// Set Parameter
	int SaveParameter(char* result);									// Save Parameter on Reader's flash memory	
	
	int UpdateFirmware(char* result,char Filename[]);
	int Reboot(char* result);
	
	void GetNearestEPC(char* EPC);
	void WriteLog(char* message);
};

#endif


