#!/bin/bash

cd "$(dirname "$0")"

sed -i "/^#define GUI  /s/  .*/  false/" config.h
sed -i "/^#define Analysis  /s/  .*/  true/" config.h
sed -i "/^#define Optimise  /s/  .*/  false/" config.h
sed -i "/^#define logData  /s/  .*/  true/" config.h
sed -i "/^#define Stop  /s/  .*/  false/" config.h
sed -i "/^#define Analyis_Log 1/s/.*/\/\/ #define Analyis_Log 1/" config.h
sed -i "/^#define Noise_Analysis 1/s/.*/\/\/ #define Noise_Analysis 1/" config.h
sed -i "/^#define Post_Eval 1/s/.*/\/\/ #define Post_Eval 1/" config.h

case "$1" in

-compile|-c)
  cd ../../
  cmake .
  cd examples/playground/

  make enkiplayground
  shift
  ;;

esac


sed -i "/^No_Of_Trials	=/s/=.*/= 1;/" Parameters.cfg

if [ $# -eq 2 ]
then
  sed -i "/^noOfShepherd =/s/=.*/= ({Value = $1;});/" Parameters.cfg
  sed -i "/^noOfObjects 	=/s/=.*/= $2;/" Parameters.cfg

elif  [ $# -eq 3 ]
then
  sed -i "/^noOfShepherd =/s/=.*/= ({Value = $1;},{Value = $2;});/" Parameters.cfg
  sed -i "/^noOfObjects 	=/s/=.*/= $3;/" Parameters.cfg

elif  [ $# -eq 5 ]
then
  sed -i "/^noOfShepherd =/s/=.*/= ({Value = $1;},{Value = $2;},{Value = $3;},{Value = $4;});/" Parameters.cfg
  sed -i "/^noOfObjects 	=/s/=.*/= $5;/" Parameters.cfg

elif  [ $# -eq 9 ]
then
  sed -i "/^noOfShepherd =/s/=.*/= ({Value = $1;},{Value = $2;},{Value = $3;},{Value = $4;},{Value = $5;},{Value = $6;},{Value = $7;},{Value = $8;});/" Parameters.cfg
  sed -i "/^noOfObjects 	=/s/=.*/= $9;/" Parameters.cfg

fi

./enkiplayground
./plot.py
