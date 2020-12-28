#!/bin/bash

sed -i "/^#define GUI  /s/  .*/  false/" config.h
sed -i "/^#define Analysis  /s/  .*/  true/" config.h
sed -i "/^#define Optimise  /s/  .*/  false/" config.h
sed -i "/^#define logData  /s/  .*/  true/" config.h
sed -i "/^#define Stop  /s/  .*/  true/" config.h
sed -i "/^#define Analyis_Log 1/s/.*/\/\/ #define Analyis_Log 1/" config.h
sed -i "/^#define Noise_Analysis 1/s/.*/\/\/ #define Noise_Analysis 1/" config.h
sed -i "/^#define Post_Eval 1/s/.*/\/\/ #define Post_Eval 1/" config.h

#Calling ls directly using the exec command
make enkiplayground

sed -i "/^No_Of_Trials	=/s/=.*/= 1;/" Parameters.cfg
sed -i "/^mode		=/s/=.*/= $1;/" Parameters.cfg
sed -i "/^noOfShepherd 	=/s/=.*/= $2;/" Parameters.cfg
sed -i "/^noOfSheep 	=/s/=.*/= $3;/" Parameters.cfg
sed -i "/^noOfObjects 	=/s/=.*/= $4;/" Parameters.cfg

if [ $1 -eq 0 ]
then
  echo -e "\e[91mShepherding\e[0m"

elif  [ $1 -eq 1 ]
then
  echo -e "\e[91mObject Clustering\e[0m"

elif  [ $1 -eq 2 ]
then
  echo -e "\e[91mShepherding + Object Clustering\e[0m"

elif  [ $1 -eq 3 ]
then
  echo -e "\e[91mSimplified Controller\e[0m"

fi

./enkiplayground
./plot.py
