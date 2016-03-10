#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cgicc/CgiDefs.h> 
#include <cgicc/Cgicc.h> 
#include <cgicc/HTTPHTMLHeader.h> 
#include <cgicc/HTMLClasses.h>  

char HTTP_EVNS[][64] = {
    "PATH",
    "LD_LIBRARY_PATH",
    "SERVER_SOFTWARE",
    "SERVER_NAME",
    "GATEWAY_INTERFACE",
    "SERVER_PROTOCOL",
    "SERVER_PORT",
    "REQUEST_METHOD",
    "PATH_INFO",
    "PATH_TRANSLATED",
    "SCRIPT_NAME",
    "QUERY_STRING",
    "REMOTE_ADDR",
    "HTTP_REFERER",
    "HTTP_USER_AGENT",
    "HTTP_ACCEPT",
    "HTTP_ACCEPT_ENCODING",
    "HTTP_ACCEPT_LANGUAGE",
    "HTTP_COOKIE",
    "CONTENT_TYPE",
    "HTTP_HOST",
    "CONTENT_LENGTH",
    "REMOTE_USER",
    "AUTH_TYPE",
    "TZ",
    "CGI_PATTERN",
};
using namespace cgicc;
using namespace std;

int main( int argc, char* argv[] )
{
    int i, total;
	Cgicc formData;
	
    printf( "Content-type: text/html\nStatus: 200/html\n\n<HTML><HEAD><TITLE>EVN List</TITLE></HEAD>\n");
    total = sizeof(HTTP_EVNS)/sizeof(HTTP_EVNS[0]);
    for(i = 0; i < total; ++i )
    {
        printf("%s=%s<BR>\n", HTTP_EVNS[i], getenv(HTTP_EVNS[i]));
    }
	
	form_iterator fi = formData.getElement("first_name");

		if( !fi->isEmpty() && fi != (*formData).end())
		{
			cout << "First name: " << **fi << endl;  
		}
		else
		{
			cout << "No text entered for first name" << endl;  
		}
	
	cout << "</BR>\n";
    printf( "<BODY>\n</BODY></HTML>\n" );
    exit( 0 );
}
