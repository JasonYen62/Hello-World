
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
/*
void Reboot(void);
void GetVersion(void);
void Inventory(void);
void GetParameter(char ParmName[]);
void SetParameter(char ParmName[],char ParmValue[]);
*/
void ScreenNearTag(char* EPC,char Result[]);
void WriteLog(char* message);

int main(int argc, char *argv[])
{
	int boothID =0;
	int ret;
	int TagMode=1;	
	
	char result[4096];	//Max Receive data
	char EPC[256];
	char buf[128];
	char URL[128];
	
	if(argc == 1)
	{
		printf("Usage:\n");
		printf("ART3x0 command boothID\n");
		return 0;
	}
	
	RU25Lib art3x0;
	if(art3x0.Open(COM1) != 0)
	{	// UTC-DS31 is use COM1, COM2 is debug port
		printf("Open COM1 fail! \n");
		return 1;
	}
		
	// Read setting from ini file
	memset(buf,'\0', sizeof(buf));	
	//ReadIni(buf,"wpc.cfg","Mode","GetTag");
	//TagMode = atoi(buf);
	//printf("Mode:%d\n",TagMode);
	
	memset(URL,'\0', sizeof(URL));	
	//ReadIni(URL,"wpc.cfg","WebService","URL");
	//printf("URL:%s\n",URL);
	
	if(TagMode == 1)
	{	// set PA Gain
		art3x0.SetParameter("internalpagain","3d");
		art3x0.SetParameter("runmilisecond","200");
	}
	
	while(1)
	{
		memset(result,'\0', sizeof(result));
		memset(EPC,'\0', sizeof(EPC));
		//printf("Inventory:\n");
		//WriteLog("- art3x0.Inventory Start -");
		ret = art3x0.Inventory(result);
		//WriteLog("- art3x0.Inventory End -");
		//printf("	Ret: %d \r\n",ret);
		printf("Result:\n%s\n\n",result);
		
		if(strlen(result) > 6)
		{
			//ScreenNearTag((char*)(&EPC),result);
			if(TagMode == 1)
			{
				art3x0.GetNearestEPC((char*)(&EPC));			
				printf("EPC:%s, ID:%s \n",EPC,argv[1]);			
				ret = SendTagToWPCServer(EPC,argv[1],URL);
			}
			else if(TagMode == 2)
			{
				ret = SendTagListToWPCServer(result,argv[1],URL);
			}
			if(ret == 1)
			{
				printf("Return True.\n");
			}
			else
			{
				printf("Return False.\n");
			}
		}
		usleep(400000);	//400ms
	}
	
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

void WriteLog(char* message)
{
	time_t rawtime;
	struct tm *timeinfo;
	char filename[60];
	FILE *pFile = NULL;
	
	memset(filename,'\0',sizeof(filename));
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	//printf ("Current local time and date: %s", asctime(timeinfo));
	
	sprintf(filename,"A%04d%02d%02d.log",timeinfo->tm_year+1900, timeinfo->tm_mon+1 ,timeinfo->tm_mday);
	pFile = fopen (filename , "a+");
	if (pFile == NULL)
	{
		printf("Error to write log file:%s",filename);
	}
	else
	{
		fprintf(pFile,"%02d:%02d:%02d	%s\n",timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,message);
		fclose (pFile);
	}
}
