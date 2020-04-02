#!/bin/bash

#Calling ls directly using the exec command
cd ../enki
cmake .
cd examples/playground/
make enkiplayground

sed -i '/^No_Of_Threads		=/s/=.*/= 10;/' Parameters.cfg

if [ $1 -eq 0 ]
then
  sed -i '/^mode	       	=/s/=.*/= 0;/' Parameters.cfg
  sed -i '/^noOfSheep		  =/s/=.*/= 10;/' Parameters.cfg
  sed -i '/^noOfObjects		=/s/=.*/= 0;/' Parameters.cfg
  echo "Shepherding"
  ./enkiplayground

elif  [ $1 -eq 1 ]
then
  sed -i '/^mode		      =/s/=.*/= 1;/' Parameters.cfg
  sed -i '/^noOfSheep		  =/s/=.*/= 0;/' Parameters.cfg
  sed -i '/^noOfObjects		=/s/=.*/= 10;/' Parameters.cfg
  echo "Object Clustering"
  ./enkiplayground

elif  [ $1 -eq 2 ]
then
  sed -i '/^mode		      =/s/=.*/= 2;/' Parameters.cfg
  sed -i '/^noOfSheep	  	=/s/=.*/= 10;/' Parameters.cfg
  sed -i '/^noOfObjects		=/s/=.*/= 10;/' Parameters.cfg
  echo "Shepherding + Object Clustering"
  ./enkiplayground

fi
