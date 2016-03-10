
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
Jason Yen   	2013/07/12  	A.001       Create 

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "RU25Lib.h"
#include "./Soap/wpcTest.h"
#include "./Util/Datafile.h"
#include "./Driver/watchDog.h"

/*
void Reboot(void);
void GetVersion(void);
void Inventory(void);
void GetParameter(char ParmName[]);
void SetParameter(char ParmName[],char ParmValue[]);
*/

void ScreenNearTag(char* EPC,char Result[]);

int main(int argc, char *argv[])
{
	int boothID =0,nTag=0;
	int ret;
	int TagMode=1;	
	int HostType = 1;
	char result[4096];	//Max Receive data
	char EPC[300];
	char oldEPC[300];
	char buf[256];
	char URL[256];
	char PAValue[6];
	char Rssi[6];
	LinuxWatchDog watchDog(60);
	
	if(argc == 1)
	{
		printf("Usage:\n");
		printf("ART3x0 command boothID\n");
		return 0;
	}
	
	// Read setting from ini file
	memset(oldEPC,'\0', sizeof(oldEPC));	
	ReadIni(buf,"wpc.cfg","Mode","GetTag");
	TagMode = atoi(buf);
	printf("Mode:%d\n",TagMode);
	
	memset(URL,'\0', sizeof(URL));	
	ReadIni(URL,"wpc.cfg","WebService","URL");
	printf("URL:%s\n",URL);

	memset(PAValue,'\0', sizeof(PAValue));
	ReadIni(PAValue,"wpc.cfg","RFIDPOWER","PAValue");
	printf("PAValue:%s\n",PAValue);
	
	memset(Rssi,'\0', sizeof(Rssi));
	ReadIni(Rssi,"wpc.cfg","RFIDPOWER","ValidRSSI");
	printf("Rssi:%s\n",Rssi);
	
	memset(buf,'\0', sizeof(buf));
	ReadIni(buf,"wpc.cfg","Mode","HostType");
	printf("HostType:%s\n",buf);
	HostType = atoi(buf);
	
	RU25Lib art3x0;
	printf("Reader connect...\n");
	
	if(art3x0.Open(COM1) != 0)
	{	// UTC-DS31 is use COM1, COM2 is debug port
		//printf("Open COM1 fail! \n");
		return 1;
	}
	
	if(TagMode == 1)
	{	// set PA Gain
		printf("Set internal PA.\n");
		art3x0.SetParameter("internalpagain",PAValue);
		printf("Set run milisecond.\n");
		art3x0.SetParameter("runmilisecond","400");
	}
	else if(TagMode == 2)
	{	// set PA Gain	
		printf("Set internal PA\n");
		art3x0.SetParameter("internalpagain",PAValue);
		printf("Set antennaswitchmode PA.\n");
		art3x0.SetParameter("antennaswitchmode","0");	// 使用2支天線 Attena1,2輪流
	}
	// set frequency 915.xx Enable
	printf("Set setchanneldisable PA.\n");
	art3x0.SetParameter("setchannel Disable","927.75");
	//art3x0.SetParameter("setchanneldisable","924.75");
	art3x0.SetParameter("setchanneldisable","927.25");
	//art3x0.SetParameter("setchanneldisable","924.25");
	art3x0.SetParameter("setchanneldisable","926.75");
	art3x0.SetParameter("setchanneldisable","923.75");
	art3x0.SetParameter("setchanneldisable","926.25");
	art3x0.SetParameter("setchanneldisable","923.25");
	//art3x0.SetParameter("setchanneldisable","925.75");
	art3x0.SetParameter("setchanneldisable","922.75");
	//art3x0.SetParameter("setchanneldisable","925.25");
	art3x0.SetParameter("setchanneldisable","922.25");
	
	printf("Set setchannel Enable PA\n");
	art3x0.SetParameter("setchannelenable","915.25");
	art3x0.SetParameter("setchannelenable","915.75");
	//art3x0.SetParameter("setchannelenable","916.25");
	art3x0.SetParameter("setchannelenable","914.75");
	printf("Set run milisecond\n");
	art3x0.SetParameter("runmilisecond","400");
	
	watchDog.Start();
	while(1)
	{
		memset(result,'\0', sizeof(result));
		memset(EPC,'\0', sizeof(EPC));
		//printf("Inventory:\n");
		//WriteLog("- art3x0.Inventory Start -");
		ret = art3x0.Inventory(result, &nTag);
		//WriteLog("- art3x0.Inventory End -");
		//printf("	Ret: %d \r\n",ret);
		if(nTag > 0)
		{
			printf("Result:\n%s\n\n",result);
		}
		
		if(strlen(result) > 6)
		{
			//ScreenNearTag((char*)(&EPC),result);
			if(TagMode == 1)
			{
				if(art3x0.GetNearestEPC((char*)(&EPC),atoi(Rssi)) == true)
				{					
					//if(strcmp(EPC,oldEPC) != 0) // Filter same Tag
					//{
						printf("EPC:%s, ID:%s \n",EPC,argv[1]);
						if(HostType == 1)
						{
							ret = SendTagToWPCServer(EPC,argv[1],URL);
							//test ret = SendTagToWPCServer("3400E2003412013C100009380D9E",argv[1],URL);
							if(ret == 1)
							{
								printf("Return True.\n");
							}
							else
							{
								printf("Return False.\n");
							}
						}
						//else if(HostType == 2)
						//{
						//
						//	system("/www/SendStrings 'this is first line' 'this is second line' ");
						//}
					//}
				}
				//else
				//{
				//	memset(oldEPC,'\0', sizeof(oldEPC));
				//}
			}
			else if(TagMode == 2)
			{
				ret = SendTagListToWPCServer(result,argv[1],URL);
				if(ret == 1)
				{
					printf("Return True.\n");
				}
				else
				{
					printf("Return False.\n");
				}
			}
		}
		watchDog.KeepAlive();
		if(TagMode == 1)
		{
			usleep(200000);	//200ms
		}
		else
		{
			usleep(100000);	//100ms
		}
	}
	watchDog.Stop();
	return 0;
};

/*
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
		printf("	Ret: %d\r\n",ret);
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
	int ret;
	RU25Lib art310;	
	char result[5800];	//Max Receive data
	
	//if(art310.Open(COM1) == 0)
	if(art310.Open() == 0)
	{
		memset(result,'\0', sizeof(result));
		printf("Inventory:\n");
		ret = art310.Inventory(result);
		printf("	Ret:%d\r\n",ret);
		printf("	Result:	[%s]\n\n",result);	
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
*/
void ScreenNearTag(char *EPC,char Result[])
{
	char *pch = NULL;
	char buf[10];
	int i,maxRssi,n = 0;
	int epcTail[128];
	int RSSI[128];
		
	WriteLog("- ScreenNearTag start -");
	pch = strchr(Result,0x0d);
	//printf("ScreenNearTag\n");
	while (pch != NULL)
	{
		memset(buf,'\0', sizeof(buf));
		epcTail[n] = (pch-Result);
		
		//printf("ScreenNearTag2\n");
		memcpy(buf,pch-2,2);
		//printf("rssi[%d]:%s\n",n,buf);
		RSSI[n] = strtol (buf,NULL,16);
		
		pch = strchr(pch + 1,0x0d);
		n++;
		if(pch == NULL)
		{
			break;		
		}
	}
	
	if(n > 1) // Tag more than 1
	{	// search RSSI largest 
		maxRssi = 0;
		for(i=1; i<n; i++)
		{
			if(RSSI[maxRssi] < RSSI[i])
			{
				maxRssi = i;
			}
		}
		//printf("max:%d\n",maxRssi);
		if(maxRssi >0)
		{
			strncpy(EPC,Result+epcTail[maxRssi-1]+2,28);
		}
		else
		{
			strncpy(EPC,Result,28);
		}
	}
	else
	{	// only one tag
		strncpy(EPC,Result,28);
	}
	WriteLog("- ScreenNearTag End -");
}

