export PATH=$PATH:/home/jason/DS31LBV1150/cross_compiler/fsl-linaro-toolchain/bin

arm-linux-g++ -o wpcDemoArm.bin wpcDemo.cpp soapC.cpp soapWPCEventTrackingServiceSoapProxy.cpp stdsoap2.cpp
arm-linux-g++ wpcDemo.cpp -o wpcDemoArm.bin -L. -lART3x0risk -lSoapWPC