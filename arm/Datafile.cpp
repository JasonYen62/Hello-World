/* Datafile.cpp 
    All read/write file function(include SQLite db).
*/
#include <stdio.h>   /* Standard input/output definitions */
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "Datafile.h"
#define LineWordMax 64
#define RETRY 3


bool INIReadLine(char* lineData,FILE* fh)
{
	bool bComment = false;
	short index = 0;
	char c = 0x00;

	do
	{
		while(true)
		{
			c = fgetc(fh);
			if(c == EOF)
				return true;
			if(c == 0x0d || c == 0x20)
				continue;
			if(c == 0x0a )
			{
				lineData[index]=0;
				break;
			}
			if(bComment == true)
				continue;
			if(c == ';' || c == '#')
			{
				bComment = true;
				continue;
			}
			lineData[index] = c;
			index ++;
			if(index >= LineWordMax)
			{
				lineData[LineWordMax] = 0;
				break;
			}
		}
		if(strlen(lineData) == 0)
		{
			index = 0;
			bComment = false;
		}
	}while(strlen(lineData) == 0);
	return false;
}

// Read Section and key value from INI file to data
// variable input/output	comment
// data 	output			key value
// filename input			ini file name
// Section  input			Section name
// key 		input			Key name
bool ReadIni(char* data,char* filename,char* Section,char* key)
{
	bool  bFindKey = false;
	FILE* fh = NULL;
	char* SectionPoint = NULL;
	char* keyPoint = NULL;
	char  LineData[LineWordMax+1];
	char  buffer[LineWordMax+1];

	fh = fopen(filename,"rb");
	if(fh == NULL)
	{
		return false;
	}

	memset(LineData,0,sizeof(LineData));
	memset(buffer,0,sizeof(buffer));
	fseek ( fh , 0L , SEEK_SET );
	while(true)
	{
		if(INIReadLine(LineData,fh)==true) //true :is EOF
			break;
		
		sprintf(buffer,"[%s]",Section);

		if( SectionPoint == NULL)
		{
			SectionPoint = strstr(LineData,buffer);
		}
		else
		{
			sprintf(buffer,"%s=",key);
			keyPoint = strstr(LineData,buffer);
			if(keyPoint != NULL)
			{
				bFindKey = true;
				strcpy(data,LineData+strlen(buffer));
			}
		}
	}
	fclose(fh);
	return bFindKey;
}

void GetKeyName(char *key,char *value,char LineString[])
{
	char *pEqu = NULL;
	short len = 0;

	pEqu = strchr(LineString,'=');
	if(pEqu)
	{
		memcpy(key,LineString,pEqu-LineString);
		len = strlen(LineString) - (pEqu+1-LineString);
		memcpy(value,pEqu+1,len);
	}

	//printf("%s = %s  \n",key,value);
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
