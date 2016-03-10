# Make file for ART3x0 Reader SDK 

all: wpcDemo
wpc: wpcDemo
smartreader: SmartReader
art: ART3x0

CC = g++
DRIVER_PATH = Driver
WEBSRV_PATH = Soap
UTIL_PATH = Util
CGICCINC =  /arm-linux/include
CGICCARMLIB = /arm-linux/lib
SQLLIB = /usr/local/lib
SQLINC = /usr/local/include

CFLAGS = -O2 -Wall -march=i686
LIBS   = -lpthread 
#-lSDL -ldl -lsqlite3

wpcDemo: rs232.o Network.o Datafile.o RU25Lib.o wpcTest.o soapC.o soapWPCEventTrackingServiceSoapProxy.o stdsoap2.o wpcDemo.o 
	# compile a static Execution file wpcDemo
	$(CC) -O3 -march=i486 -o wpcDemo.bin rs232.o Network.o RU25Lib.o soapC.o soapWPCEventTrackingServiceSoapProxy.o stdsoap2.o wpcTest.o wpcDemo.o Datafile.o
SmartReader: rs232.o Network.o RU25Lib.o SmartReader.o
ART3x0: rs232.o Network.o RU25Lib.o ART3x0.o	
	# compile a static Execution file
	$(CC) -O3 -march=i486 -o ART3x0.bin rs232.o Network.o RU25Lib.o ART3x0.o
	
	# compile a shared library
	#$(CC) -fPIC -shared -o libART3x0.so rs232.o Network.o RU25Lib.o
	# compile a Execution file with shared library
	#$(CC) -O3 -march=i486 -L. -lART3x0 -o ART3x0 ART3x0.o 

soapC.o: $(WEBSRV_PATH)/soapC.cpp
	$(CC) -I$(WEBSRV_PATH) $(CFLAGS) -c $(WEBSRV_PATH)/soapC.cpp
soapWPCEventTrackingServiceSoapProxy.o: $(WEBSRV_PATH)/soapWPCEventTrackingServiceSoapProxy.cpp
	$(CC) -I$(WEBSRV_PATH) $(CFLAGS) -c $(WEBSRV_PATH)/soapWPCEventTrackingServiceSoapProxy.cpp
stdsoap2.o: $(WEBSRV_PATH)/stdsoap2.cpp
	$(CC) -I$(WEBSRV_PATH) $(CFLAGS) -c $(WEBSRV_PATH)/stdsoap2.cpp
wpcTest.o: $(WEBSRV_PATH)/wpcTest.cpp
	$(CC) -I$(WEBSRV_PATH) $(CFLAGS) -c $(WEBSRV_PATH)/wpcTest.cpp
Datafile.o: $(UTIL_PATH)/Datafile.cpp
	$(CC) -I$(UTIL_PATH) $(CFLAGS) -c $(UTIL_PATH)/Datafile.cpp	
rs232.o: $(DRIVER_PATH)/rs232.cpp
	$(CC) -I$(DRIVER_PATH) $(CFLAGS) -c $(DRIVER_PATH)/rs232.cpp
Network.o: $(DRIVER_PATH)/Network.cpp
	$(CC) -I$(DRIVER_PATH) $(CFLAGS) -c $(DRIVER_PATH)/Network.cpp
RU25Lib.o: RU25Lib.cpp
	$(CC) -I$(DRIVER_PATH) $(CFLAGS) -c RU25Lib.cpp
ART3x0.o: ART3x0.cpp
	$(CC) -I$(DRIVER_PATH) $(CFLAGS) -c ART3x0.cpp
SmartReader.o: SmartReader.cpp
	$(CC) -I$(DRIVER_PATH) $(CFLAGS) -c SmartReader.cpp	
wpcDemo.o: wpcDemo.cpp
	$(CC) -I$(DRIVER_PATH) $(CFLAGS) -c wpcDemo.cpp		
clean: 
	-rm ART3x0.o rs232.o ART3x0 RU25Lib.o Network.o libART3x0.so SmartReader.o soapC.o soapWPCEventTrackingServiceSoapProxy.o stdsoap2.o wpcDemo.o Datafile.o wpcTest.o 
	