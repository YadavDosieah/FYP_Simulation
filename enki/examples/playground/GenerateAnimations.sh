#!/bin/bash

cd "$(dirname "$0")"

# Source progress bar
source ./progress_bar.sh

# Make sure that the progress bar is cleaned up when user presses ctrl+c
enable_trapping
# Create progress bar
setup_scroll_area

# This accounts as the "totalState" variable for the ProgressBar function
_counter=0
_end=18


#Calling ls directly using the exec command
cd ../../
cmake .
cd examples/playground/

sed -i "/^#define GUI  /s/  .*/  false/" config.h
sed -i "/^#define Analysis  /s/  .*/  true/" config.h
sed -i "/^#define Optimise  /s/  .*/  false/" config.h
sed -i "/^#define logData  /s/  .*/  true/" config.h
sed -i "/^#define Stop  /s/  .*/  false/" config.h
sed -i "/^#define Analyis_Log 1/s/.*/\/\/ #define Analyis_Log 1/" config.h
sed -i "/^#define Noise_Analysis 1/s/.*/\/\/ #define Noise_Analysis 1/" config.h
make enkiplayground

Controllers=( "A" "B" "C" "D" "D.A" "D.B")
Scenarios=( "0" "1" "2" )

mode=0
sed -i "/^No_Of_Trials	=/s/=.*/= 1;/" Parameters.cfg
sed -i "/^noOfShepherd 	=/s/=.*/= 10;/" Parameters.cfg

echo -e "Started at $(date)!"
for controller in "${Controllers[@]}"
do
  sed -i "/^mode		=/s/=.*/= $mode;/" Parameters.cfg
  for scenario in "${Scenarios[@]}"
  do
    if [ $scenario -eq 0 ]
    then
      sed -i "/^noOfSheep 	=/s/=.*/= 30;/" Parameters.cfg
      sed -i "/^noOfObjects 	=/s/=.*/= 0;/" Parameters.cfg

    elif  [ $scenario -eq 1 ]
    then
      sed -i "/^noOfSheep 	=/s/=.*/= 0;/" Parameters.cfg
      sed -i "/^noOfObjects 	=/s/=.*/= 30;/" Parameters.cfg

    elif  [ $scenario -eq 2 ]
    then
      sed -i "/^noOfSheep 	=/s/=.*/= 15;/" Parameters.cfg
      sed -i "/^noOfObjects 	=/s/=.*/= 15;/" Parameters.cfg

    fi
    draw_progress_bar $(($_counter*100/$_end))
    ./enkiplayground
    $(./plot.py "NewAnimations/$controller$scenario/")
    _counter=$((${_counter}+1))
  done
  mode=$(($mode+1))
done

echo -e "\nCompleted at $(date)!"
destroy_scroll_area
# poweroff
