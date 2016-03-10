/*

File:
  RU25Lib.cpp

  Author(s):
  Jason Yen, Advantech Corporation

  Description:
  ART-3x0 UHF Reader Library
  
============================================ Modify Hisory Label ===============================================
Modify      	Date        	Label       Purpose
----------------------------------------------------------------------------------------------------------------
Jason Yen   	2011/08/11  	A.001       Start 


*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "RU25Lib.h"

RU25Lib::RU25Lib(void)
{
	ConnectDevice = NO_CONNECT;
	rj45 = NULL;
	//m_InvResult = NULL;
}

RU25Lib::~RU25Lib(void)
{
	//if(m_InvResult != NULL)
	//	delete m_InvResult;
	CloseReader();
}

int RU25Lib::Open()
{	// From file
	int ret = 0;	
	long lSize;
	char c;
	char *buffer = NULL;	
	FILE *pFile = NULL;
	size_t result;
		
	pFile = fopen("ART3x0.cfg","rb");
	
	if(pFile != NULL)
	{	// Read config
		fseek (pFile , 0 , SEEK_END);
		lSize = ftell (pFile);
		rewind (pFile);
		
		buffer = (char*) malloc (sizeof(char)*lSize);
		memset(buffer,'\0',lSize);
		if (buffer == NULL) 
		{
			fputs ("Memory error",stderr);
			return -1;
		}

		// copy the file into the buffer:
		result = fread (buffer,1,lSize,pFile);
		if (result != lSize) 
		{
			fputs ("Reading error",stderr);
			return -1;
		}
		// terminate
		fclose (pFile);		
	}
	if(strlen(buffer) < 7)
	{	// UART: COM1,COM2.....
		if(strncmp(buffer,"COM",3)==0)
		{
			c = buffer[3];
			ret = Open(SerialPort[atoi(&c)]);
		}
		else
		{
			printf("Unknow COM port setting !\n");
			ret = -1;
		}
	}
	else
	{	// IP address
		ret = Open(buffer);
	}
	free (buffer);
	return ret;
}

int RU25Lib::Open(enum COMPORT comport)
{	// UART
	bool bSuccess = true;
    // Setup the serial port (115200,N81)
    bSuccess = serial.PortInit(comport,115200,'N',8,1);
	if (!bSuccess )
	{
		return -2;
	}
	ConnectDevice = UART_CONNECT;
	return 0;
}

int RU25Lib::Open(char IpAddress[])
{	//  TCP/IP
	bool bSuccess = false;
    // Setup the tcpip
	rj45 = new TCPSocket();
    bSuccess = rj45->Initial(IpAddress,10001,false);
	if (!bSuccess )
	{
		return -2;
	}
	ConnectDevice = RJ45_CONNECT;
	
	return 0;
}
	
int RU25Lib::SendCommand(char cmd[],int len)
{
	if(ConnectDevice == UART_CONNECT)
	{				
		serial.PortWrite(cmd,len);
	}
	else if(ConnectDevice == RJ45_CONNECT)
	{				
		rj45->Send(cmd,len);
	}
}

int RU25Lib::ReadResponse(char* datas)
{
	int nbyte = 0,totalByte=0;
	char *str = NULL;
	char CompareStr[128];
	
	while(1)
	{		
		if(ConnectDevice == UART_CONNECT)
		{
			//printf(".%d.",totalByte);
			if (serial.PortRead2CRLF(&nbyte, datas + totalByte , 3000) == true)	//4s timeout
			{
				memset(CompareStr,'\0',sizeof(CompareStr));
				memcpy(CompareStr,datas + totalByte ,nbyte);
				totalByte += (nbyte);				
			}
		}	
		else if(ConnectDevice == RJ45_CONNECT)
		{
			if(rj45->ReadtoCRLF(&nbyte, datas + totalByte +1, 3000) == true)
			{
				memset(CompareStr,'\0',sizeof(CompareStr));
				memcpy(CompareStr,datas + totalByte,nbyte);
				totalByte += nbyte;
			}
		}
		else
		{
			printf("No connection\n");
			break;
		}
		
		//printf("compare:[%s]\n",CompareStr);
		str = strstr(CompareStr,"1102\r");
		if(str != NULL || nbyte==0)
		{			
			//printf("[%d End]\n",nbyte);
			break;
		}
	}
	
	return totalByte;
}

int RU25Lib::ActiveReceive(char* datas)
{
	int nbyte = 0,totalByte=0;
	char *str = NULL;
	char CompareStr[128];
	

	if(ConnectDevice == UART_CONNECT)
	{
		//printf(".%d.",totalByte);
		if (serial.PortRead2CRLF(&nbyte, datas + totalByte , 3000) == true)	//4s timeout
		{
			memset(CompareStr,'\0',sizeof(CompareStr));
			memcpy(CompareStr,datas + totalByte ,nbyte);
			totalByte += (nbyte);				
		}
	}	
	else if(ConnectDevice == RJ45_CONNECT)
	{
		if(rj45->ReadtoCRLF(&nbyte, datas + totalByte +1, 3000) == true)
		{
			memset(CompareStr,'\0',sizeof(CompareStr));
			memcpy(CompareStr,datas + totalByte,nbyte);
			totalByte += nbyte;
		}
	}
	else
	{
		printf("No connection\n");
	}
	
	return totalByte;
}

bool RU25Lib::ReadOnePacket(char* datas)
{
	int nbyte = 0;
	
	while(1)
	{
		switch(ConnectDevice)
		{
			case UART_CONNECT:		
				if (serial.PortRead2CRLF(&nbyte,datas,4000) == true)	//4s timeout
				{
					return true;
				}
				break;
			case RJ45_CONNECT:			
				if(rj45->ReadtoCRLF(&nbyte,datas,3000) == true)
				{
					return true;
				}
				break;
			default:
				printf("No connection\n");
				break;
		}
	}
	
	return false;
}

int RU25Lib::Inventory(char* result,int *nTag)
{
	int  i,ret = 0;
	char packet[2048];
	int  index = 0, len = 0;

	if(ConnectDevice == UART_CONNECT)
	{
		//printf("COM port write\n");
		serial.PortWrite("inventory\r",10);
	}
	else if(ConnectDevice == RJ45_CONNECT)
	{
		rj45->Send("inventory\r",10);
	}
	else
	{
		return -1;
	}
	
	memset(packet,'\0', sizeof(packet));
	//WriteLog("- ReadOnePacket 1101 -");
	if(ReadOnePacket((char*)&packet) == true)
	{
		//printf("Packet:%s\n",packet);
		if(strncmp(packet,"1101",4) == 0)
		{
		}
	
	}
	m_iTags =0;
	memset(&m_InvResult,'\0', sizeof(strInventoryResult) * MAX_INVENTORYTAGS);
	//if(m_InvResult != NULL)
	//{
	//	delete m_InvResult;
	//}
	
	index = 0;
	for (int maxTag = 0; maxTag < MAX_INVENTORYTAGS; maxTag++) 
	{
		memset(packet,'\0', sizeof(packet));
		//WriteLog("- ReadOnePacket EPC -");
		if(ReadOnePacket((char*)&packet) == true)
		{
			len = strlen(packet);
			
			if( len < 6 && len > 1)
			{	//return code
				ret = atoi(packet);
				break;
			}
			memcpy(result + index, packet, len);
			index += len;
			// Parse and save to variable:m_InvResult			
			//printf("m_iTags = %d\n",m_iTags);
			ParseInvData(m_iTags,packet);
			m_iTags++;
		}
		else
		{
			break;
		}
	}
	*nTag = m_iTags;
	//WriteLog("- ReadOnePacket 1102 -");
	memset(packet,'\0', sizeof(packet));
	if(ReadOnePacket((char*)&packet) == true)
	{
		//printf("Packet:%s\n",packet);
		if(strncmp(packet,"1102",4) == 0)
		{
			//m_InvResult = new invResult[m_iTags];
		}
		else
		{
			//for(i=0; i<strlen(packet); i++)
			//{
			//	printf("2:%x ",packet[i]);
			//}
		}
	}

	return ret;
}

int RU25Lib::GetFirmwareVersion(char* result)
{
	int ret =0;
	char packet[64];
	int index = 0, len = 0;
	
	if(ConnectDevice == UART_CONNECT)
	{
		serial.PortWrite("version\r",8);
	}
	else if(ConnectDevice == RJ45_CONNECT)
	{
		rj45->Send("version\r",8);
	}
	else
	{
		return -1;
	}
	
	for (int timeout = 0; timeout < 4; timeout++)
	{
		memset(packet,'\0', sizeof(packet));
	
		if(ReadOnePacket((char*)&packet) == true)
		{
			//printf("Packet%d:%s\n",timeout,packet);
			if(strncmp(packet,"1102",4) == 0)
			{
				break;
			}
			if(strcmp(packet,"no command") == 0)
			{
				break;
			}
			if(strncmp(packet,"1101",4) == 0)
			{
				continue;
			}
			len = strlen(packet);
			
			if( len < 5)
			{	//return code
				ret = atoi(packet);
				continue;
			}
			memcpy(result + index, packet, len);
			index += len;			
		}
		else
		{
			break;
		}
		//usleep(100);		
	}
	return ret;
}

int RU25Lib::SetParameter(char ParamName[],char ParaValue[])
{
	int index = 0, len = 0,ret = 0;
	int packetIndex = 0;
	char packet[128];
	
	//printf("Set parameter:\n");
	memset(packet,'\0', sizeof(packet));
	if(ConnectDevice == UART_CONNECT)
	{
		sprintf(packet,"set %s %s",ParamName,ParaValue);
		//printf("u : %s=%s Send:%s len:%d \n",ParamName,ParaValue,packet,strlen(packet));//debug
		serial.PortWrite(packet,strlen(packet));		
		serial.PortWrite("\r",1);
	}
	else if(ConnectDevice == RJ45_CONNECT)
	{
		sprintf(packet,"set %s %s",ParamName,ParaValue);
		//printf("n  : %s=%s Send:%s len:%d\n",ParamName,ParaValue,packet,strlen(packet));//debug
		rj45->Send(packet,strlen(packet));
		rj45->Send("\r",1);
	}
	else
	{
		return -1;
	}
	
	//usleep(1000);	
	for (int timeout = 0; timeout < 200; timeout++) 
	{
		memset(packet,'\0', sizeof(packet));
		if(ReadOnePacket((char*)&packet) == true)
		{
			//printf("write Packet2:%s\n",packet);
			if(strncmp(packet,"1102",4) == 0)
			{
				break;
			}
	
			if(strncmp(packet,"1101",4) == 0)
			{
				continue;
			}
			len = strlen(packet);
			if( len < 5 && len>2)
			{	//return code
				ret = atoi(packet);		
				continue;
			}

			index += len;			
		}
	}
	return ret;
}

int RU25Lib::GetParameter(char* result, char ParamName[])
{
	int index = 0, len = 0,ret = 0,i;
	char packet[64];

	if(ConnectDevice == UART_CONNECT)
	{
		sprintf(packet,"get %s",ParamName);
		serial.PortWrite(packet,strlen(packet));
		serial.PortWrite("\r",1);
	}
	else if(ConnectDevice == RJ45_CONNECT)
	{
		rj45->Send("get ",2);
		rj45->Send(ParamName,strlen(ParamName));
		rj45->Send("\r",1);
	}
	else
	{
		return -1;
	}
	
	memset(packet,'\0', sizeof(packet));
	if(ReadOnePacket((char*)&packet) == true)
	{
		//printf("Packet:%s\n",packet);
		if(strncmp(packet,"1101",4) == 0)
		{			
		}
		else
		{	
			for( i=0; i<strlen(packet); i++)
			{
				printf("%x ",packet[i]);
			}
		}
	}
	for (int timeout = 0; timeout < 2; timeout++) 
	{
		memset(packet,'\0', sizeof(packet));
		if(ReadOnePacket((char*)&packet) == true)
		{
			//printf("Packet2:%s\n",packet);
			len = strlen(packet);
			if( len < 5 && len>3)
			{	//return code
				ret = atoi(packet);		
				break;
			}
			
			memcpy(result + index, packet, len);
			index += len;			
		}
		else
		{
			break;
		}
		usleep(1000);		
	}
	
	//for (int timeout2 = 0; timeout2 < 2; timeout2++) 
	{
		memset(packet,'\0', sizeof(packet));
		if(ReadOnePacket((char*)&packet) == true)
		{
			//printf("Packet3:%s\n",packet);
			if(strncmp(packet,"1102",4) == 0)
			{
			}
			else
			{	
				//for(i=0;i<strlen(packet);i++)
				//{
				//	printf("%x ",packet[i]);
				//}
			}
		}
	}
	return ret;
}

int RU25Lib::SaveParameter(char* result)
{
	int ret =0;
	char packet[64];
	int index = 0, len = 0;
	
	if(ConnectDevice == UART_CONNECT)
	{
		serial.PortWrite("saveparameter\r",14);
	}
	else if(ConnectDevice == RJ45_CONNECT)
	{	
		rj45->Send("saveparameter\r",14);
	}
	else
	{
		return -1;
	}
	//usleep(100);
	
	for (int timeout = 0; timeout < 4; timeout++) 
	{
		memset(packet,'\0', sizeof(packet));
	
		if(ReadOnePacket((char*)&packet) == true)
		{
			printf("Packet%d:%s\n",timeout,packet);
			if(strncmp(packet,"1102",4) == 0)
			{
				break;
			}
		
			if(strncmp(packet,"1101",4) == 0)
			{
				continue;
			}
			len = strlen(packet);
			
			if( len < 5 && len>1)
			{	//return code
				ret = atoi(packet);
				continue;
			}
			memcpy(result + index, packet, len);
			index += len;			
		}
		else
		{
			break;
		}
		//usleep(100);		
	}
	return ret;
}

int RU25Lib::ReadMemory(char* result,int bank,int nWord,char APassowrd[],int start)
{
	int cmdRet;
	int i,index = 0, len = 0, ret = 0;
	char packet[128], buf[8];
	int packetIndex=0;

	memset(buf,'\0', sizeof(buf));

	sprintf(buf,"%d",bank);
	cmdRet = SetParameter("membank",buf);

	sprintf(buf,"%d",nWord);
	cmdRet = SetParameter("wordcount",buf);

	sprintf(buf,"%d",start);
	cmdRet = SetParameter("wordptr",buf);

	cmdRet = SetParameter("apassword",APassowrd);

	if(ConnectDevice == UART_CONNECT)
	{
		serial.PortWrite("read\r",5);
	}
	else if(ConnectDevice == RJ45_CONNECT)
	{
		rj45->Send("read\r",5);
	}
	else
	{
		return -1;
	}
	
	usleep(1000);
	memset(packet,'\0', sizeof(packet));
	if(ReadOnePacket((char*)&packet) == true)
	{	//Receive "1101"
		//printf("Packet:%s\n",packet);
		if(strncmp(packet,"1101",4) == 0)
		{
			//printf("START");
		}
		else
		{	
			for(i=0; i<strlen(packet); i++)
			{
				printf("%x ",packet[i]);
			}
		}
	}
	
	for (int timeout = 0; timeout < 2; timeout++) 
	{
		memset(packet,'\0', sizeof(packet));
		if(ReadOnePacket((char*)&packet) == true)
		{
			//printf("Packet:%s\n",packet);
			len = strlen(packet);
			//printf("len:%d",len);
			if( len < 6  && len >1)
			{	//return code
				ret = atoi(packet);		
				break;
			}		
			memcpy(result + index, packet, len);
			index += len;			
		}	
		usleep(1000);		
	}
	
	memset(packet,'\0', sizeof(packet));
	if(ReadOnePacket((char*)&packet) == true)
	{	//Receive "1102"
		//printf("Packet:%s\n",packet);
		if(strncmp(packet,"1102",4) == 0)
		{
		}
		else
		{	
			printf("End Recv:%s \n",packet);
		}
	}
	
	return ret;
}

int RU25Lib::WriteMemory(char* result,int bank,char APassowrd[],char wData[],int startWord)
{
	int cmdRet;
	int index = 0, len = 0,ret = 0;
	char packet[64], buf[8];	
	int packetIndex=0;
	
	memset(buf,'\0', sizeof(buf));

	sprintf(buf,"%1d",bank);
	cmdRet = SetParameter("membank",buf);
	if(cmdRet < 0)
	{
		printf("Set Membank. Ret:%d \n",cmdRet);
		return -2;
	}
	
	cmdRet = SetParameter("writedata",wData);
	if(cmdRet < 0)
	{
		printf("Set Writedata . Ret:%d \n",cmdRet);
		return -2;
	}
	
	sprintf(buf,"%d",startWord);
	cmdRet = SetParameter("wordptr",buf);
	if(cmdRet < 0)
	{
		printf("Set Wordptr. Ret:%d \n",cmdRet);
		return -2;
	}
	
	cmdRet = SetParameter("apassword",APassowrd);
	if(cmdRet < 0)
	{
		printf("Set Access password. Ret:%d \n",cmdRet);
		return -2;
	}
	
	if(ConnectDevice == UART_CONNECT)
	{
		serial.PortWrite("write\r",6);
	}
	else if(ConnectDevice == RJ45_CONNECT)
	{
		rj45->Send("write\r",6);
	}
	else
	{
		return -1;
	}
	
	usleep(1000);	
	for (int timeout = 0; timeout < 4; timeout++) 
	{
		memset(packet,'\0', sizeof(packet));
		if(ReadOnePacket((char*)&packet) == true)
		{
			//printf("Packet%2d:%s \n",timeout,packet);
			if(strncmp(packet,"1102",4) == 0)
			{
				break;
			}
			
			if(strncmp(packet,"1101",4) == 0)
			{
				continue;
			}

			len = strlen(packet);
			if( len < 6  && len >1)
			{	//return code
				ret = atoi(packet);	
				continue;
			}		
			memcpy(result + index, packet, len);
			index += len;			
		}	
		usleep(1000);		
	}
	
	return ret;
}

int RU25Lib::ChangeAccessPassword(char APassword[],char NewAPassowrd[])
{
	int ret;
	char retData[64];

	memset(retData,'\0', sizeof(retData));
	ret = WriteMemory((char*)&retData,0,APassword,NewAPassowrd,2);
	return ret;
}

int RU25Lib::ChangeKillPassword(char APassword[],char KPassowrd[])
{
	int ret;
	char retData[64];
	
	memset(retData,'\0', sizeof(retData));
	ret = WriteMemory((char*)&retData,0,APassword,KPassowrd);
	return ret;
}

int RU25Lib::LockTag(char* result,char APassword[], unsigned int Payload)
{
	char packet[64],buf[32];
	int i,cmdRet;
	int len = 0,ret = 0,index =0;
	int packetIndex = 0;
	unsigned int iTemp =0;
	
	memset(buf,'\0', sizeof(buf) );
	for(i=0; i < 20; i++)
	{
		iTemp = (Payload << i) & 0x00080000;

		if( iTemp == 0)
		{
			buf[i] = '0';
		}
		else
		{
			buf[i] = '1';
		}
	}

	cmdRet = SetParameter("apassword",APassword);
	cmdRet = SetParameter("payload",buf);
	
	if(ConnectDevice == UART_CONNECT)
	{
		serial.PortWrite("lock\r",5);
	}
	else if(ConnectDevice == RJ45_CONNECT)
	{
		rj45->Send("lock\r",5);
	}
	else
	{
		return -1;
	}
	
	usleep(1000);	
	for (int timeout = 0; timeout < 4; timeout++) 
	{		
		if(ReadOnePacket((char*)&packet) == true)
		{
			//printf("Packet%d:%s\n",timeout,packet);
			if(strncmp(packet,"1102",4) == 0)
			{
				break;
			}
			
			if(strncmp(packet,"1101",4) == 0)
			{
				continue;
			}
			
			len = strlen(packet);
			if( len < 6  && len >1)
			{	//return code
				//printf("len:%s",packet); //debug
				ret = atoi(packet);
				continue;
			}
	
			memcpy(result + index, packet, len);
			index += len;			
		}	
		usleep(1000);		
	}	
	return ret;
}

int RU25Lib::KillTag(char* result,char KPassword[])
{
	int cmdRet;
	char packet[64];
	int len = 0,ret = 0,index =0;
	int packetIndex=0;
	
	cmdRet = SetParameter("kpassword",KPassword);
	
	if(ConnectDevice == UART_CONNECT)
	{
		serial.PortWrite("kill\r",5);
		usleep(1000);	
	}
	else if(ConnectDevice == RJ45_CONNECT)
	{
		rj45->Send("kill\r",5);
	}
	else
	{
		return -1;
	}
	
	for (int timeout = 0; timeout < 4; timeout++) 
	{
		memset(packet,'\0', sizeof(packet));
		if(ReadOnePacket((char*)&packet) == true)
		{
			printf("Packet:[%s]\n",packet);
			if(strncmp(packet,"1102",4) == 0)
			{
				break;
			}
		
			if(strncmp(packet,"1101",4) == 0)
			{
				continue;
			}
			len = strlen(packet);
			if( len<6  && len>1)
			{	//return code
				ret = atoi(packet);	
				continue;
			}

			memcpy(result + index, packet, len);
			index += len;			
		}	
		usleep(1000);		
	}	
	return ret;
}

int RU25Lib::SelectTag(char TagEPC[])
{
	int cmdRet=0;
	char buf[8];
	int len = 0,ret = 0;
	int packetIndex=0;

	memset(buf,'\0', sizeof(buf));

	cmdRet = SetParameter("selectmask",TagEPC);
	if(cmdRet < 0)
	{
		printf("selectmask : return %d\n",cmdRet);
		return cmdRet;
	}
	
	sprintf(buf,"1");	
	cmdRet = SetParameter("selectmembank",buf);
	if(cmdRet < 0)
	{
		printf("selectmembank : return %d\n",cmdRet);
		return cmdRet;
	}
	sprintf(buf,"32");
	cmdRet = SetParameter("selectpointer",buf);
	if(cmdRet < 0)
	{
		printf("selectpointer : return %d\n",cmdRet);
		return cmdRet;
	}
	memset(buf,'\0', sizeof(buf));

	len = strlen(TagEPC) * 4;
	sprintf(buf,"%d",len);
	
	//printf("Sel Len:%s\n",buf);
	cmdRet = SetParameter("selectlength",buf);	
	return cmdRet;
}

int RU25Lib::deSelectTag()
{
	int cmdRet;
	
	cmdRet = SetParameter("selectlength","0");
	return cmdRet;
}

// PA­È ¹ï DBmªºÃö«Y
// 30dbm  29dbm , 28dbm, 27dbm, 26dbm, 25dbm, 24dbm, 23dbm, 22dbm, 21dbm, 20dbm, 19dbm, 18dbm, 17dbm, 16dbm, 15dbm
// 0x90,  0x50    0x45   0x3d   0x38   0x35   0x31   0x2E   0x2c   0x2a   0x28   0x26   0x24   0x23   0x21   0x20
int RU25Lib::SetPower(char value[])
{
	int cmdRet = SetParameter("internalpagain",value);	
	return cmdRet;
}

int RU25Lib::SetFrequencyEnable(char Freq[],bool Enable)
{
	int cmdRet;

	if(Enable)
	{
		cmdRet = SetParameter("setchannelenable",Freq);
	}
	else
	{
		cmdRet = SetParameter("setchanneldisable",Freq);
	}
	return 0;
}

int RU25Lib::SetHoppingMode(char Mode[])
{
	int cmdRet = SetParameter("hoppingmode",Mode);	
	return cmdRet;
}


void RU25Lib::CloseReader(void)
{
	if(ConnectDevice == UART_CONNECT)
	{
		serial.PortClose();
	}
	else if(ConnectDevice == RJ45_CONNECT)
	{
		rj45->tcpClose();
	}
	
	ConnectDevice = NO_CONNECT;
}


int RU25Lib::UpdateFirmware(char* result,char Filename[])
{
	int ret =0;
	char packet[64];
	int index = 0, len = 0;
		
	if(SendCommand("updatefirmware\r",15) < 0)
	{
		return -1;
	}

	for (int timeout = 0; timeout < 4; timeout++) 
	{
		memset(packet,'\0', sizeof(packet));
	
		if(ReadOnePacket((char*)&packet) == true)
		{
			//printf("Packet%d:%s\n",timeout,packet);
			if(strncmp(packet,"1102",4) == 0)
			{
				break;
			}
	
			if(strncmp(packet,"1101",4) == 0)
			{
				//Tx Firmware file
				//printf("Tx file.....\n"); //	debug
				ret = TxFirmware(Filename);
				//if(ret < 0)
				//	printf("Update Firmware Error! Please reboot Reader!\n");
				continue;
			}
			len = strlen(packet);
			
			if( len < 5 && len>1)
			{	//return code
				ret = atoi(packet);
				continue;
			}
			memcpy(result + index, packet, len);
			index += len;			
		}
		else
		{
			break;
		}
	}
	return ret;
}

int RU25Lib::Reboot(char* result)
{
	int ret =0;
	char packet[64];
	int index = 0, len = 0;
	
	if(ConnectDevice == UART_CONNECT)
	{
		serial.PortWrite("reboot\r",7);
	}
	else if(ConnectDevice == RJ45_CONNECT)
	{	
		rj45->Send("reboot\r",7);
	}
	else
	{
		return -1;
	}

	for (int timeout = 0; timeout < 3; timeout++) 
	{
		memset(packet,'\0', sizeof(packet));
	
		if(ReadOnePacket((char*)&packet) == true)
		{
			//printf("Packet%d:%s\n",timeout,packet);
			if(strncmp(packet,"RESET",5) == 0)
			{
				continue;
			}
			
			len = strlen(packet);
			
			if( len < 5 && len>1)
			{	//return code
				ret = atoi(packet);
				continue;
			}
			memcpy(result + index, packet, len);
			index += len;			
		}
		else
		{
			break;
		}
	}
	return ret;
}

int RU25Lib::TxFirmware(char Filename[])
{
	unsigned char c;
	unsigned short crc16=0;
	char wBuff[512], wData[512], buf[32];
	char packet[64];
	long size = 0,updateLen = 0;
	long i, index = 0;
	size_t len = 0;
	FILE *fh = NULL;
	
	memset(wBuff,'\0',sizeof(wBuff));
	memset(packet,'\0',sizeof(packet));
	fh = fopen(Filename,"rb");
	if(fh != NULL)
	{
		fseek(fh,0,SEEK_END);
		size = ftell(fh);
		fseek(fh,0,SEEK_SET);
		
		//printf("Tx data size=%ld \n",size);
		do
		{	//Tx Data
			memset(wData,'\0',sizeof(wData));
			len = fread (wData,1,256,fh);
			if(len == EOF)
			{
				break;
			}
			memcpy( wBuff,wData,len);
			for(i=0; i<256 && updateLen < size; i++, updateLen++)
			{
				c = wBuff[i];
				sprintf(buf,"%02X",c);		
				SendCommand(buf,2);				
			}
		
			//Tx CRC		
			memset(buf,'\0',sizeof(buf));
			crc16 = (unsigned short)~CRC16(wBuff,(unsigned short)i<<3);
			sprintf(buf,"%04X\r",crc16);			
			SendCommand(buf,5);
			
			//printf("%d CRC:%04x \n",updateLen,crc16);
			// Read response to "\r\n"			
			if(ReadOnePacket((char*)&packet) == true)
			{
				//printf("resp1:%s \n",packet);
				if(strncmp(packet,"1701",4)==0)
				{	//success
					continue;
				}	
				else if(strncmp(packet,"-1701",5)==0 || strncmp(packet,"-1704",5))
				{
					SendCommand("ready\r",6);
					if(ReadOnePacket((char*)&packet) == true)
					{
						if(strncmp(packet,"1702",4)==0)
						{
							break;
						}
					}
					fclose(fh);
					return -1;
				}
				else if(strncmp(packet,"-1702",5)==0 || strncmp(packet,"-1703",5))
				{
					fclose(fh);
					return -2;
				}			
			}			
		} while(updateLen < size);
		fclose(fh);
		
		SendCommand("end\r",4);
		do
		{
			if(ReadOnePacket((char*)&packet) == true)
			{
				//printf("resp2:%s \n",packet);
				if(strncmp(packet,"-1701",5)==0 || strncmp(packet,"-1704",5))
				{
					SendCommand("ready\r",6);
					if(ReadOnePacket((char*)&packet) == true)
					{
						if(strncmp(packet,"1702",4)==0)
						{
							break;
						}
					}
				
					return -1;
				}
				else if(strncmp(packet,"-1702",5)==0 || strncmp(packet,"-1703",5))
				{				
					return -2;
				}
				else if(strncmp(packet,"1703",4)==0)
				{	//update firmware End packet					
					return 0;
				}				
			}
		}while(true);
	}
	
	return 0;
}

unsigned short RU25Lib::CRC16(char buf[],int len)
{
	const int POLY = 0x1021;
	unsigned short shift = 0xFFFF, data = 0, val=0;
	long i, j;
	
	for(i=0, j=0; i<len; i++)
	{
		if((i & 0x00000007) == 0)  // (i & 0x00000007) = (i % 8)
		{
			data = (unsigned short) (buf[j++]<<8);
		}
		val = (unsigned short)(shift^data);
		shift = (unsigned short)(shift << 1);
		data = (unsigned short)(data << 1);
		
		if( (val & 0x8000) > 0)
		{
			shift = (unsigned short)(shift^POLY);
		}
	}
	
	return shift;
}


/* Parse string like : "3400E2003412DC03011880139086,0001,4B,59,54" */
void RU25Lib::ParseInvData(int index, char* InvData)
{
	char *pch = NULL;
	char *frontpch = NULL;
	char buf[8];
		
	pch = strchr(InvData,',');
	strncpy((m_InvResult[index].ProtocolControl),InvData,4);
	strncpy((m_InvResult[index].EPC),InvData+4,pch-(InvData+4));
	//printf("PC:%s,EPC:%s\n",m_InvResult[index].ProtocolControl,m_InvResult[index].EPC);
	
	memset(buf,'\0', sizeof(buf));
	memcpy(buf,pch+1,4);	
	m_InvResult[index].Attenna = strtol (buf,NULL,16);
		
	pch = strchr(pch+1,',');
	memset(buf,'\0', sizeof(buf));
	memcpy(buf,pch+1,2);
	m_InvResult[index].minRSSI = strtol (buf,NULL,16);
	
	pch = strchr(pch+1,',');
	memset(buf,'\0', sizeof(buf));
	memcpy(buf,pch+1,2);
	m_InvResult[index].maxRSSI = strtol (buf,NULL,16);
	
	pch = strchr(pch+1,',');
	memset(buf,'\0', sizeof(buf));
	memcpy(buf,pch+1,2);
	m_InvResult[index].avgRSSI = strtol (buf,NULL,16);
	//printf("Attenna:%d rssimin:%d rssimax:%d rssiavg:%d \n",m_InvResult[index].Attenna,
	//	m_InvResult[index].minRSSI,m_InvResult[index].maxRSSI, m_InvResult[index].avgRSSI);
}

bool RU25Lib::GetNearestEPC(char* EPC, int validRSSI)
{
	int i;
	int MaxRssiIndex = 0;
	
	for( i=1; i<m_iTags; i++)
	{
		if(m_InvResult[i].avgRSSI > m_InvResult[MaxRssiIndex].avgRSSI)
		{
			MaxRssiIndex = i;
		}
	}
	if(m_InvResult[MaxRssiIndex].avgRSSI > validRSSI)
	{
		sprintf(EPC,"%s%s", m_InvResult[MaxRssiIndex].ProtocolControl, m_InvResult[MaxRssiIndex].EPC);
		return true;
	}
	return false;
}

void RU25Lib::WriteLog(char* message)
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