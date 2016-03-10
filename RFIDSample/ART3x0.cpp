
/*
File:
  ART3x0.cpp

  Author(s):
  Jason Yen, Advantech Corporation

  Description:
  SDK Demo and Test Program
  
============================================ Modify Hisory Label ===============================================
Modify      	Date        	Label       Purpose
----------------------------------------------------------------------------------------------------------------
Jason Yen   	2011/08/11  	A.001       Start 

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "RU25Lib.h"


void Reboot(void);
void GetVersion(void);
void TestProcess(void);
void Inventory(void);
void SelectTag(char EPC[]);
void GetParameter(char ParmName[]);
void UpdateFirmware(char filename[]);
void KillTag(char EPC[],char KPassword[]);
void SetParameter(char ParmName[],char ParmValue[]);
void LockTag(char EPC[],char payload[],char APassword[]);
void ReadMemory(char bank[],char nword[],char password[],char start[]);
void WriteMemory(char bank[],char data[],char password[],char start[]);


int main(int argc, char *argv[])
{
	if(argc == 1)
	{
		printf("Usage:\n");
		printf("ART3x0 command parameter1 parameter2 ....\n\n");
		printf("COMMAND:\n");		
		printf("	VERSION:	Get Firmware version.\n");
		printf("	REBOOT: 	Reboot Reader.\n");
		printf("	FWUPDATE: 	Update Firmware\n");
		printf("	INVENTORY:	Inventory Tags.\n");
		printf("	READ:		Read memory data\n");
		printf("	WRITE:		Write memory data\n");
		printf("	SELECT:		Select a Tag.\n");
		printf("	SETPARM:	Set Reader Parameter.\n");
		printf("	GETPARM:	Get Reader Parameter.\n");
		printf("	LOCK:		Set Tag Memory Security.\n");
		printf("	KILL:		Kill a Tag.\n");
		return 0;
	}
	if(strcmp(argv[1],"Test")==0)
	{
		TestProcess();
	}
	else if(strcmp(argv[1],"FWUPDATE")==0)
	{
		if(argc < 3)
		{
			printf("Usage:\n");
			printf("ART3x0 FWUPDATE [file path name]\n");
		}
		else
		{
			UpdateFirmware(argv[2]);
		}
	}
	else if(strcmp(argv[1],"VERSION")==0)
	{
		GetVersion();
	}
	else if(strcmp(argv[1],"REBOOT")==0)
	{
		Reboot();
	}
	else if(strcmp(argv[1],"INVENTORY")==0)
	{
		Inventory();
	}
	else if(strcmp(argv[1],"READ")==0)
	{
		if(argc < 6)
		{
			printf("Usage:\n");
			printf("ART3x0 READ [bank] [read n word] [password] [start word]\n");
		}
		else
		{
			ReadMemory(argv[2],argv[3],argv[4],argv[5]);
		}
	}
	else if(strcmp(argv[1],"WRITE")==0)
	{
		if(argc < 6)
		{
			printf("Usage:\n");
			printf("ART3x0 WRITE [bank] [data] [password] [start word]\n");
		}
		else
		{
			WriteMemory(argv[2],argv[3],argv[4],argv[5]);
		}
	}
	else if(strcmp(argv[1],"SELECT")==0)
	{
		int cmdRet;	
		if(argc < 3)
		{
			printf("Usage:\n");
			printf("ART3x0 SELECT [EPC code]\n");
			
			SetParameter("selectlength","0");
		}
		else
		{
			SelectTag(argv[2]);
		}
	}
	else if(strcmp(argv[1],"SETPARM")==0)
	{
		if(argc < 4)
		{
			printf("Usage:\n");
			printf("ART3x0 SETPARM [parameter name] [parameter value]\n");
		}
		else
		{
			SetParameter(argv[2],argv[3]);
		}
	}
	else if(strcmp(argv[1],"GETPARM")==0)
	{
		if(argc < 3)
		{
			printf("Usage:\n");
			printf("ART3x0 GETPARM [parameter name]\n");
		}
		else
		{
			GetParameter(argv[2]);
		}
	}
	else if(strcmp(argv[1],"LOCK")==0)
	{
		if(argc < 5)
		{
			printf("Usage:\n");
			printf("ART3x0 LOCK [EPC] [payload] [Access Password]\n");
			printf("payload format :\n");
			printf("   Memory:\n");
			printf("	A: Access Password\n");
			printf("	K: Kill Password\n");
			//printf("	T: TID memory\n");
			printf("	E: EPC memory\n");
			printf("	U: User memory\n");
			printf("   Security level:\n");
			printf("	0: writeable\n");
			printf("	1: writeable permanently\n");
			printf("	2: writeable. need password and Password readable need password\n");
			printf("	3: not writeable and Password not readable permanently\n");
			printf("\nExample:  Set Kill password Read/Write need Password, User memory is writeable, others is no change\n");
			printf(" payload => K2U0\n");
		}
		else
		{
			LockTag(argv[2],argv[3],argv[4]);
		}
	}
	else if(strcmp(argv[1],"KILL")==0)
	{
		if(argc < 4)
		{
			printf("Usage:\n");
			printf("ART3x0 KILL [EPC] [Kill password]\n");
		}
		else
		{
			KillTag(argv[2],argv[3]);
		}
	}
	
	return 0;
};

// auto Test Process
void TestProcess(void)
{
	int ret, nTag=0;
	unsigned int Payload = 0;
	char result[5800];	//Max Receive data
	RU25Lib art310;	
	
	if(art310.Open(COM1) == 0)
	//if(art310.Open("172.16.12.103") == 0)
	{		
		printf("Test ART3x0 Programe. \n");
		
		memset(result,'\0', sizeof(result));
		printf("Function0:	Reboot Reader:\n");
		ret = art310.Reboot(result);
		printf("	Ret:%d\r\n",ret);
		printf("	Result:	[%s]\n\n",result);
		
		memset(result,'\0', sizeof(result));
		printf("Function1:	Firmware Version:\n");
		ret = art310.GetFirmwareVersion(result);
		printf("	Ret:%d\r\n",ret);
		printf("	Result:	[%s]\n\n",result);
		
		memset(result,'\0', sizeof(result));
		printf("Function2:	Set Power 27 dBm \n");
		ret = art310.SetPower("3D");
		printf("	Ret:%d\n",ret);
			
		memset(result,'\0', sizeof(result));
		printf("Function3:	Inventory \n");
		ret = art310.Inventory(result,&nTag);
		printf("	Ret:%d\n",ret);
		printf("result:	[%s]\n\n",result);
		
		memset(result,'\0', sizeof(result));
		printf("Function4:	Get Parameter channelnumber \n");
		ret = art310.GetParameter(result,"channelnumber");
		printf("	Ret:%d\n",ret);
		printf("Result:	[%s]\n",result);
		
		//select E2003412DC03011880139086
		memset(result,'\0', sizeof(result));
		printf("Function5:	Select Tag  E2003412DC03011880139086\n");
		ret = art310.SelectTag("E2003412DC03011880139086");
		printf("	Ret:%d\n",ret);
		//printf("result:	[%s]\n",result);
		
		memset(result,'\0', sizeof(result));
		printf("Function6:	Read bank3 2word \n");
		ret = art310.ReadMemory(result,3,4,"00000000",0);
		printf("	Ret:%d\n",ret);
		printf("Result:	[%s]\n",result);
		
		memset(result,'\0', sizeof(result));
		printf("Function7:	Write Bank3 \"ABCD1234\" \n");
		ret = art310.WriteMemory(result,3,"00000000","ABCD1234",0);
		printf("	Ret:%d\n",ret);
				
		memset(result,'\0', sizeof(result));
		printf("Function8:	Read bank3 2word \n");
		ret = art310.ReadMemory(result,3,2,"00000000",0);
		printf("	Ret:%d\n",ret);
		printf("Result:	[%s]\n",result);
		
		memset(result,'\0', sizeof(result));
		printf("Function9:	Set Access Password to 11111111  \n");
		ret = art310.ChangeAccessPassword("00000000","11111111");
		printf("	Ret:%d\n",ret);
				
		memset(result,'\0', sizeof(result));
		printf("Function10:	Lock Tag:\n");
		Payload |= USER_WSECURED;
		ret = art310.LockTag(result,"11111111",Payload);
		printf("	Ret:%d\n",ret);
				
		memset(result,'\0', sizeof(result));
		printf("Function11:	Write Bank3 \"12345678\" \n");
		ret = art310.WriteMemory(result,3,"00000000","12345678",2);
		printf("	Ret:%d\n",ret);
				
		memset(result,'\0', sizeof(result));
		printf("Function12:	Write Bank3 \"98765432\" with correct passowrd \n");
		ret = art310.WriteMemory( result, 3, "11111111", "98765432", 2);
		printf("	Ret:%d\n",ret);
				
		//Kill Tag
		memset(result,'\0', sizeof(result));
		printf("Function13:	Set Kill Password to 44444444 \n");
		ret = art310.ChangeKillPassword("00000000","44444444");
		printf("	Ret:%d\n",ret);
		
		ret = art310.deSelectTag();
	
		art310.CloseReader();
	}
	else
	{
		printf("Connect error! \n");
	}
}

// Update Firmware
void UpdateFirmware(char filename[])
{
	RU25Lib art310;
	int ret;
	char result[256];	//Max Receive data
	
	//if(art310.Open(COM1) == 0)
	if(art310.Open() == 0)
	{
		memset(result,'\0', sizeof(result));
		printf("Firmware Version:\n");
		ret = art310.GetFirmwareVersion(result);
		printf("	Ret:%d\r\n",ret);
		printf("	Result:	[%s]\n\n",result);
		
		memset(result,'\0', sizeof(result));
		printf("Update Firmware \n");
		ret = art310.UpdateFirmware(result,filename);
		printf("	Ret:%d\n",ret);
	}	
}

// Get Firmware Version
void GetVersion()
{
	RU25Lib art310;
	int ret;
	char result[256];	//Max Receive data
	
	//if(art310.Open(COM1) == 0)
	if(art310.Open() == 0)
	{
		memset(result,'\0', sizeof(result));
		printf("Firmware Version:\n");
		ret = art310.GetFirmwareVersion(result);
		printf("	Ret:%d\r\n",ret);
		printf("	Result:	[%s]\n\n",result);	
	}	
}

// Reboot firmware
void Reboot()
{
	RU25Lib art310;
	int ret;
	char result[256];	//Max Receive data
	
	//if(art310.Open(COM1) == 0)
	if(art310.Open() == 0)
	{
		memset(result,'\0', sizeof(result));
		printf("Reboot:\n");
		ret = art310.Reboot(result);
		printf("	Ret:%d\r\n",ret);
		printf("	Result:	[%s]\n\n",result);	
	}
}

// Inventory
void Inventory(void)
{
	int i,ret,nTag=0;
	RU25Lib art310;	
	char result[5800];	//Max Receive data
	
	//if(art310.Open(COM1) == 0)
	if(art310.Open() == 0)
	{
		memset(result,'\0', sizeof(result));
		printf("Inventory:\n");
		ret = art310.Inventory(result,&nTag);
		
		printf("	Ret:%d\r\n",ret);
		if(nTag >0)
		{
			//printf("	Result:	[%s]\n\n",result);	
			for(i=0;i<nTag;i++)
			{
				printf("EPC:%s RSSIavg:%d \n",art310.m_InvResult[i].EPC,art310.m_InvResult[i].avgRSSI);
			}
		}
	}
}

// Read Tag bank Memory
void ReadMemory(char bank[],char nword[],char password[],char start[])
{
	RU25Lib art310;
	int ret, iBank, iNword, iStart;
	char result[256];	//Max Receive data

	memset(result,'\0', sizeof(result));
	//if(art310.Open(COM1) == 0)
	if(art310.Open() == 0)
	{
		iBank = atoi(bank);
		iNword = atoi(nword);
		iStart = atoi(start);
		printf("Bank:%d Start:%d nWord:%d passowrd:%s\n",iBank,iNword,iStart,password);
		ret = art310.ReadMemory(result,iBank,iNword,password,iStart);
		printf("	Ret:%d\n",ret);
		printf("Result:	[%s]\n",result);
	}		
}

// Write Tag
void WriteMemory(char bank[],char data[],char password[],char start[])
{
	RU25Lib art310;
	int ret, iBank, iStart;
	char result[256];	//Max Receive data

	memset(result,'\0', sizeof(result));
	//if(art310.Open(COM1) == 0)
	if(art310.Open() == 0)
	{
		iBank = atoi(bank);
		iStart = atoi(start);
		//printf("Bank:%d  nWord:%d  data:[%s] passowrd:[%s] \n",iBank,iStart, data,password);
		ret = art310.WriteMemory(result,iBank,password,data,iStart);
		printf("	Ret:%d\n",ret);
	}
}

// Select Tag
void SelectTag(char EPC[])
{
	RU25Lib art310;
	int ret;
	char result[256];	//Max Receive data

	memset(result,'\0', sizeof(result));
	//if(art310.Open(COM1) == 0)
	if(art310.Open() == 0)
	{
		printf("Select:[%s]\n",EPC);
		ret = art310.SelectTag(EPC);
		printf("	Ret:%d\n",ret);
	}
}

void UnSelectTag()
{
	RU25Lib art310;
	int ret;
	
	//if(art310.Open(COM1) == 0)
	if(art310.Open() == 0)
	{
		ret = art310.deSelectTag();
		printf("	Ret:%d\n",ret);
	}
}

// Set Reader Parameter
void SetParameter(char ParmName[],char ParmValue[])
{
	RU25Lib art310;
	int ret = 0;

	//if(art310.Open(COM1) == 0)
	if(art310.Open() == 0)
	{
		ret = art310.SetParameter(ParmName,ParmValue);
		printf("	Ret:%d\n",ret);		
	}
}

// Get Reader Parameter
void GetParameter(char ParmName[])
{
	RU25Lib art310;
	int ret = 0;
	char result[256];	//Max Receive data
	
	//if(art310.Open(COM1) == 0)
	if(art310.Open() == 0)
	{
		memset(result,'\0', sizeof(result));
	
		ret = art310.GetParameter(result,ParmName);
		printf("	Ret:%d\n",ret);
		printf("Result:	[%s]\n",result);	
	}
}

void LockTag(char EPC[],char payload[],char APassword[])
{
	RU25Lib art310;
	int ret = 0;
	int i,plLen = 0;
	unsigned int mask,level, payloadValue = 0;
	char result[256];	//Max Receive data
	char c;
	
	//if(art310.Open(COM1) == 0)
	if(art310.Open() == 0)
	{
		memset(result,'\0', sizeof(result));
		ret = art310.SelectTag(EPC);
		printf("Select EPC:%s\n",EPC);
		//parse payload
		plLen = strlen(payload);
		if( (plLen % 2) != 0)
		{
			printf("Payload format error!\n");
			return;
		}
		for(i=0; i<plLen; i += 2)
		{
			if(payload[i+1] < 0x30 || payload[i+1] > 0x39)
			{
				printf("Invalid payload level! (%c%c)!\n",payload[i],payload[i+1]);
			}
			mask = 0x00C00;
			c = payload[i+1];
			level = atoi(&c);
			if(level>3)
			{
				printf("payload level too large! (%c%c)!\n",payload[i],payload[i+1]);
			}
			switch(payload[i])
			{
				case 'A':	// Access Password
					mask <<= 6;
					level <<= 6;
					break;
				case 'K':	// Kill Password
					mask <<= 8;
					level <<= 8;
					break;				
				case 'E':	// EPC Bank
					mask <<= 4;
					level <<= 4;
					break;
				case 'T':	// TID Bank
					mask <<= 2;
					level <<= 2;
					break;	
				case 'U':	// User Bank
					break;
				default :
					printf("Invalid payload memory!! (%c%c)\n",payload[i],payload[i+1]);
					return;
			}
			payloadValue |= (mask | level);
		}
		// lock
		
		//printf("payload:%d\n",payloadValue); //debug
		ret = art310.LockTag( result,APassword,payloadValue);
		printf("	Ret:%d\n",ret);
		printf("Result:	[%s]\n",result);
	}
}

void KillTag(char EPC[],char KPassword[])
{
	RU25Lib art310;
	int ret = 0;
	char result[256];	//Max Receive data
	
	//if(art310.Open(COM1) == 0)
	if(art310.Open() == 0)
	{
		memset(result,'\0', sizeof(result));
	
		ret = art310.SelectTag(EPC);
		ret = art310.KillTag( result,KPassword);
		printf("	Ret:%d\n",ret);
		printf("Result:	[%s]\n",result);
	}
}


