#!/bin/bash

#Calling ls directly using the exec command
make enkiplayground

sed -i "/^GUI		=/s/=.*/= false;/" Parameters.cfg
sed -i "/^Analysis	=/s/=.*/= true;/" Parameters.cfg
sed -i "/^logData		=/s/=.*/= true;/" Parameters.cfg
sed -i "/^No_Of_Trials	=/s/=.*/= 1;/" Parameters.cfg
sed -i "/^Stop		=/s/=.*/= true;/" Parameters.cfg

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

fi

./enkiplayground
./plot.py
