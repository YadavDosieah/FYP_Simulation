#!/bin/bash

cd "$(dirname "$0")"

# Source progress bar
source ./progress_bar.sh

# Make sure that the progress bar is cleaned up when user presses ctrl+c
enable_trapping
# Create progress bar
setup_scroll_area

# This accounts as the "totalState" variable for the ProgressBar function
_end=36 #180
_counter=0

cd ../../
cmake .
cd examples/playground/

sed -i "/^#define GUI  /s/  .*/  false/" config.h
sed -i "/^#define Analysis  /s/  .*/  true/" config.h
sed -i "/^#define logData  /s/  .*/  false/" config.h
sed -i "/^#define Stop  /s/  .*/  true/" config.h
sed -i "/^\/\/ #define Analyis_Log 1/s/.*/#define Analyis_Log 1/" config.h
sed -i "/^#define Noise_Analysis 1/s/.*/\/\/ #define Noise_Analysis 1/" config.h
sed -i "/^#define Post_Eval 1/s/.*/\/\/ #define Post_Eval 1/" config.h

make enkiplayground

sed -i "/^No_Of_Trials	=/s/=.*/= 100;/" Parameters.cfg
sed -i '/^No_Of_Threads	=/s/=.*/= 20;/' Parameters.cfg

shepherd=5
sheepArray=( 0 2 4 6 8 10 )
modeArray=( 0 1 2 3 4 5)

sed -i "/^noOfShepherd 	=/s/=.*/= $shepherd;/" Parameters.cfg

echo "mode,Shepherds,Sheep,Objects, Fit Val, max SR, SR" > Analysis.csv

echo -e "\Started at $(date)!"

for mode in "${modeArray[@]}"
do
  echo "-----------------------mode $mode--------------------" >> Analysis.csv
  sed -i "/^mode		=/s/=.*/= $mode;/" Parameters.cfg

  for sheep in "${sheepArray[@]}"
  do
    sed -i "/^noOfSheep 	=/s/=.*/= $sheep;/" Parameters.cfg
    sed -i "/^noOfObjects 	=/s/=.*/= $((10 - $sheep));/" Parameters.cfg
    echo -e "\rRun $mode,$shepherd,$sheep,$((10 - $sheep))"
    draw_progress_bar $(($_counter*100/$_end))
    ./enkiplayground
    _counter=$((${_counter}+1))
  done
done
echo -e "\nCompleted at $(date)!"
destroy_scroll_area

# ./GenerateAnimations.sh
# poweroff
