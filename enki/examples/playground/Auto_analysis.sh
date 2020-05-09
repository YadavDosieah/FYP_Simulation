#!/bin/bash

#Calling ls directly using the exec command
make enkiplayground

sed -i "/^GUI		=/s/=.*/= false;/" Parameters.cfg
sed -i "/^Analysis	=/s/=.*/= true;/" Parameters.cfg
sed -i "/^logData		=/s/=.*/= false;/" Parameters.cfg
sed -i "/^No_Of_Trials	=/s/=.*/= 50;/" Parameters.cfg
sed -i "/^Stop		=/s/=.*/= true;/" Parameters.cfg
sed -i '/^No_Of_Threads	=/s/=.*/= 10;/' Parameters.cfg


shepherdArray=( 5 10 15 )
sheepArray=( 10 20 30 40 50 )
objectArray=( 10 20 30 40 50 )
bothArray=( 10 20 30 40 50 )
modeArray=( 0 1 2 )


echo "mode,Shepherds,Sheep,Objects, Fit Val, max SR, SR" > Analysis.csv

for mode in "${modeArray[@]}"
do
  echo "-----------------------mode $mode--------------------" >> Analysis.csv
  sed -i "/^mode		=/s/=.*/= $mode;/" Parameters.cfg


  echo "-----------------------Scenario 0--------------------" >> Analysis.csv
  for shepherd in "${shepherdArray[@]}"
  do
    sed -i "/^noOfShepherd 	=/s/=.*/= $shepherd;/" Parameters.cfg
    sed -i "/^noOfObjects 	=/s/=.*/= 0;/" Parameters.cfg

    for sheep in "${sheepArray[@]}"
    do
      sed -i "/^noOfSheep 	=/s/=.*/= $sheep;/" Parameters.cfg
      echo "Run $mode,$shepherd,$sheep,0"
      date
      ./enkiplayground
    done
  done

  echo "-----------------------Scenario 1--------------------" >> Analysis.csv
  for shepherd in "${shepherdArray[@]}"
  do
    sed -i "/^noOfShepherd 	=/s/=.*/= $shepherd;/" Parameters.cfg
    sed -i "/^noOfSheep 	=/s/=.*/= 0;/" Parameters.cfg

    for object in "${objectArray[@]}"
    do
      sed -i "/^noOfObjects 	=/s/=.*/= $object;/" Parameters.cfg
      echo "Run $mode,$shepherd,0,$object"
      date
      ./enkiplayground
    done
  done


  echo "-----------------------Scenario 2--------------------" >> Analysis.csv
  for shepherd in "${shepherdArray[@]}"
  do
    sed -i "/^noOfShepherd 	=/s/=.*/= $shepherd;/" Parameters.cfg

    for both in "${bothArray[@]}"
    do
      sed -i "/^noOfSheep 	=/s/=.*/= $both;/" Parameters.cfg
      sed -i "/^noOfObjects 	=/s/=.*/= $both;/" Parameters.cfg
      echo "Run $mode,$shepherd,$both,$both"
      date
      ./enkiplayground
    done
  done
done

poweroff
