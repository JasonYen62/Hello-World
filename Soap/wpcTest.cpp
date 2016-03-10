


#include "soapWPCEventTrackingServiceSoapProxy.h"
#include "WPCEventTrackingServiceSoap.nsmap"

#include <string>  
#include <iostream>

int SendTagToWPCServer(char *usrID,char *tagID, char* url);
int SendTagListToWPCServer(char *usrID,char *tagID, char* url);

/*
int main()
{
  
    int ret = SendTagToWPCServer("999","E2003412Dc03011880139212","http://172.20.1.132:8005/WPCEventTrackingService.asmx");
	printf("ret:%d\n",ret);
    return 0;
}

*/

int SendTagToWPCServer(char *usrID,char *tagID, char* url)
{
	WPCEventTrackingServiceSoapProxy proxy(url);

    _ns1__addEventLog addEventLog;
    addEventLog.soap = &proxy;
	
    std::string userID(usrID);
	std::string locationTagID(tagID);
    addEventLog.userID = &userID;
	addEventLog.locationTagID = &locationTagID;
	
	//printf("Send:%s\n",usrID);
    _ns1__addEventLogResponse EventLogResponse;
    proxy.addEventLog(&addEventLog, &EventLogResponse );
	
	return EventLogResponse.addEventLogResult;
}

int SendTagListToWPCServer(char *usrID,char *tagID, char* url)
{
	WPCEventTrackingServiceSoapProxy proxy(url);

    _ns1__addEventLogList addEventLogList;
    addEventLogList.soap = &proxy;
	
    std::string userID(usrID);
	std::string locationTagID(tagID);
    addEventLogList.userID = &userID;
	addEventLogList.locationTagID = &locationTagID;
	
    _ns1__addEventLogListResponse EventLogListResponse;
    proxy.addEventLogList(&addEventLogList, &EventLogListResponse );
	
	return EventLogListResponse.addEventLogListResult;
}
