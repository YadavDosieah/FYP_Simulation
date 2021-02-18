#!/bin/bash

cd "$(dirname "$0")"

#Calling ls directly using the exec command
cd ../../
cmake .
cd examples/playground/

# sed -ri 's/(.*)(\Evolution	= )([0-9]+)(.*)/echo "\1\2$((\3+1))\4"/ge' Parameters.cfg

sed -i "/^#define GUI  /s/  .*/  false/" config.h
sed -i "/^#define Analysis  /s/  .*/  true/" config.h
sed -i "/^#define Optimise  /s/  .*/  false/" config.h
sed -i "/^#define logData  /s/  .*/  false/" config.h
sed -i "/^#define Stop  /s/  .*/  false/" config.h
sed -i "/^\/\/ #define Analyis_Log 1/s/.*/#define Analyis_Log 1/" config.h
sed -i "/^#define Noise_Analysis 1/s/.*/\/\/ #define Noise_Analysis 1/" config.h
sed -i "/^\/\/ #define Post_Eval 1/s/.*/#define Post_Eval 1/" config.h

make enkiplayground

sed -i '/^No_Of_Threads	=/s/=.*/= 20;/' Parameters.cfg
sed -i "/^No_Of_Trials	=/s/=.*/= 100;/" Parameters.cfg

sed -i "/^noOfShepherd 	=/s/=.*/= 6;/" Parameters.cfg

if [ $1 -eq 0 ]
then
  sed -i '/^mode		=/s/=.*/= 0;/' Parameters.cfg
  sed -i '/^noOfSheep 	=/s/=.*/= 25;/' Parameters.cfg
  sed -i '/^noOfObjects 	=/s/=.*/= 25;/' Parameters.cfg
  echo "Shepherding"
  ./Post_Eval.py 0

elif  [ $1 -eq 1 ]
then
  sed -i '/^mode		=/s/=.*/= 1;/' Parameters.cfg
  sed -i '/^noOfSheep 	=/s/=.*/= 25;/' Parameters.cfg
  sed -i '/^noOfObjects 	=/s/=.*/= 25;/' Parameters.cfg
  echo "Object Clustering"
  ./Post_Eval.py 1

elif  [ $1 -eq 2 ]
then
  sed -i '/^mode		=/s/=.*/= 2;/' Parameters.cfg
  sed -i '/^noOfSheep 	=/s/=.*/= 25;/' Parameters.cfg
  sed -i '/^noOfObjects 	=/s/=.*/= 25;/' Parameters.cfg
  echo "Shepherding + Object Clustering"
  ./Post_Eval.py 2

elif  [ $1 -eq 3 ]
then
  sed -i '/^mode		=/s/=.*/= 3;/' Parameters.cfg
  sed -i '/^noOfSheep 	=/s/=.*/= 50;/' Parameters.cfg
  sed -i '/^noOfObjects 	=/s/=.*/= 0;/' Parameters.cfg
  echo "Simplified Controller"
  ./Post_Eval.py 3

elif  [ $1 -eq 6 ]
then
  sed -i '/^mode		=/s/=.*/= 6;/' Parameters.cfg
  sed -i '/^noOfSheep 	=/s/=.*/= 10;/' Parameters.cfg
  sed -i '/^noOfObjects 	=/s/=.*/= 10;/' Parameters.cfg
  echo "Heterogenous Controller"
  ./Post_Eval.py 6

fi
