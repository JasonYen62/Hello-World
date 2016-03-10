export PATH=$PATH:/home/jason/DS31LBV1150/cross_compiler/fsl-linaro-toolchain/bin

#wsdl2h -o soapWPCEventTrackingServiceSoapProxy.h http://172.20.1.132:8005/WPCEventTrackingService.asmx?wsdl
#soapcpp2 -C -L -i -Id:\gsoap-2.8\gsoap\import soapWPCEventTrackingServiceSoapProxy.h

#arm-linux-g++ -o soapTest.bin wpcTest.cpp soapC.cpp soapWPCEventTrackingServiceSoapProxy.cpp stdsoap2.cpp
#arm-linux-g++ -o wpcTest.bin wpcTest.cpp soapC.cpp soapWPCEventTrackingServiceSoapProxy.cpp stdsoap2.cpp 
arm-linux-g++ -shared -fPIC -o libSoapWPC.so wpcTest.cpp soapC.cpp soapWPCEventTrackingServiceSoapProxy.cpp stdsoap2.cpp 