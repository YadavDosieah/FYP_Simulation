#!/bin/bash

# Source progress bar
source ./progress_bar.sh

# Make sure that the progress bar is cleaned up when user presses ctrl+c
enable_trapping
# Create progress bar
setup_scroll_area

# This accounts as the "totalState" variable for the ProgressBar function
_end=45
_counter=0



sed -i "/^#define GUI  /s/  .*/  false/" config.h
sed -i "/^#define Analysis  /s/  .*/  true/" config.h
sed -i "/^#define logData  /s/  .*/  false/" config.h
sed -i "/^#define Stop  /s/  .*/  true/" config.h
sed -i "/^\/\/ #define Analyis_Log 1/s/.*/#define Analyis_Log 1/" config.h
sed -i "/^#define Noise_Analysis 1/s/.*/\/\/ #define Noise_Analysis 1/" config.h

make enkiplayground

sed -i "/^No_Of_Trials	=/s/=.*/= 50;/" Parameters.cfg
sed -i '/^No_Of_Threads	=/s/=.*/= 10;/' Parameters.cfg


shepherdArray=( 05 10 15 )
sheepArray=( 10 20 30 40 50 )
objectArray=( 10 20 30 40 50 )
bothArray=( 10 20 30 40 50 )
modeArray=( 1 )


echo "mode,Shepherds,Sheep,Objects, Fit Val, max SR, SR" > Analysis.csv

echo -e "\Started at $(date)!"

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
      echo -ne "\rRun $mode,$shepherd,$sheep,00"
      draw_progress_bar $(($_counter*100/$_end))
      ./enkiplayground
      _counter=$((${_counter}+1))
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
      echo -ne "\rRun $mode,$shepherd,00,$object"
      draw_progress_bar $(($_counter*100/$_end))
      ./enkiplayground
      _counter=$((${_counter}+1))
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
      echo -ne "\rRun $mode,$shepherd,$both,$both"
      draw_progress_bar $(($_counter*100/$_end))
      ./enkiplayground
      _counter=$((${_counter}+1))
    done
  done
done
echo -e "\nCompleted at $(date)!"
destroy_scroll_area

# poweroff
