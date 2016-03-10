export PATH=$PATH:/home/jason/DS31LBV1150/cross_compiler/fsl-linaro-toolchain/bin

arm-linux-g++ -c RU25Lib.cpp rs232_ubc.cpp Network.cpp Datafile.cpp
arm-linux-g++ -shared -fPIC -o libART3x0risk.so RU25Lib.o rs232_ubc.o Network.o Datafile.o
arm-linux-g++ ART3x0.cpp -o ARTrisk.bin -L. -lART3x0risk
