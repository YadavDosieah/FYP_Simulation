#!/bin/bash

cd "$(dirname "$0")"

#Calling ls directly using the exec command
cd ../../
cmake .
cd examples/playground/

# sed -ri 's/(.*)(\Evolution	= )([0-9]+)(.*)/echo "\1\2$((\3+1))\4"/ge' Parameters.cfg

sed -i "/^#define GUI  /s/  .*/  false/" config.h
sed -i "/^#define Analysis  /s/  .*/  false/" config.h
sed -i "/^#define Optimise  /s/  .*/  true/" config.h
sed -i "/^#define logData  /s/  .*/  false/" config.h
sed -i "/^#define Stop  /s/  .*/  false/" config.h
sed -i "/^#define Analyis_Log 1/s/.*/\/\/ #define Analyis_Log 1/" config.h
sed -i "/^#define Noise_Analysis 1/s/.*/\/\/ #define Noise_Analysis 1/" config.h
sed -i "/^#define Post_Eval 1/s/.*/\/\/ #define Post_Eval 1/" config.h
make enkiplayground

sed -i '/^No_Of_Threads	=/s/=.*/= 20;/' Parameters.cfg
sed -i "/^No_Of_Trials	=/s/=.*/= 50;/" Parameters.cfg

sed -i '/^mode  =/s/=.*/= 4;/' Parameters.cfg

sed -i "/^noOfShepherd =/s/=.*/= ({Value = 4;},{Value = 4;});/" Parameters.cfg
sed -i '/^noOfSheep 	=/s/=.*/= 25;/' Parameters.cfg
sed -i '/^noOfObjects 	=/s/=.*/= 0;/' Parameters.cfg
echo "2 Groups, 4 4"
./enkiplayground
