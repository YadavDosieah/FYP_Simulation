#!/bin/bash
cd "$(dirname "$0")"

# Source progress bar
source ./progress_bar.sh

# Make sure that the progress bar is cleaned up when user presses ctrl+c
enable_trapping
# Create progress bar
setup_scroll_area

# This accounts as the "totalState" variable for the ProgressBar function
_end=234

#Calling ls directly using the exec command
cd ../../
cmake .
cd examples/playground/
make enkiplayground

sed -i "/^GUI		=/s/=.*/= false;/" Parameters.cfg
sed -i "/^Analysis	=/s/=.*/= true;/" Parameters.cfg
sed -i "/^logData		=/s/=.*/= false;/" Parameters.cfg
sed -i "/^No_Of_Trials	=/s/=.*/= 25;/" Parameters.cfg
sed -i "/^Stop		=/s/=.*/= true;/" Parameters.cfg
sed -i '/^No_Of_Threads	=/s/=.*/= 10;/' Parameters.cfg

modeArray=( 'A' 'B' 'C' )
Controllers=( "0 1 3 4" "1 2 3 4" "0 1 2 3 4")
NoiseLevel=( 0.0 0.2 0.4 0.6 0.8 1.0 )

echo "mode,Noise Level,Noise Scenario, Fit Val, max SR, SR" > Noise.csv

sed -i "/^noOfShepherd 	=/s/=.*/= 10;/" Parameters.cfg

_counter=0
mode=0

echo -e "Started at $(date)!"
for list in "${Controllers[@]}"
do
  echo "-----------------------Controller "${modeArray[$mode]}"--------------------" >> Noise.csv
  sed -i "/^mode		=/s/=.*/= $mode;/" Parameters.cfg


  echo "-----------------------Scenario 1--------------------" >> Noise.csv
  sed -i "/^noOfObjects 	=/s/=.*/= 0;/" Parameters.cfg
  sed -i "/^noOfSheep 	=/s/=.*/= 30;/" Parameters.cfg
  for scenario in $list
  do
    echo "-----------------------Noise Scenario $scenario--------------------" >> Noise.csv
    sed -i "/^Noise_Scenario	=/s/=.*/= $scenario;/" Parameters.cfg

    for Noise_Level in "${NoiseLevel[@]}"
    do
      sed -i "/^Noise_Level	=/s/=.*/= $Noise_Level;/" Parameters.cfg
      echo -ne "\rMode $mode,Scenario 0, Noise Scenario $scenario, Noise Level $Noise_Level"
      draw_progress_bar $(($_counter*100/$_end))
      ./enkiplayground
      _counter=$((${_counter}+1))
    done
  done


  echo "-----------------------Scenario 2--------------------" >> Noise.csv
  sed -i "/^noOfSheep 	=/s/=.*/= 0;/" Parameters.cfg
  sed -i "/^noOfObjects 	=/s/=.*/= 30;/" Parameters.cfg
  for scenario in $list
  do
    echo "-----------------------Noise Scenario $scenario--------------------" >> Noise.csv
    sed -i "/^Noise_Scenario	=/s/=.*/= $scenario;/" Parameters.cfg

    for Noise_Level in "${NoiseLevel[@]}"
    do
      sed -i "/^Noise_Level	=/s/=.*/= $Noise_Level;/" Parameters.cfg
      echo -ne "\rMode $mode,Scenario 1, Noise Scenario $scenario, Noise Level $Noise_Level"
      draw_progress_bar $(($_counter*100/$_end))
      ./enkiplayground
      _counter=$((${_counter}+1))
    done
  done


  echo "-----------------------Scenario 3--------------------" >> Noise.csv
  sed -i "/^noOfSheep 	=/s/=.*/= 30;/" Parameters.cfg
  sed -i "/^noOfObjects 	=/s/=.*/= 30;/" Parameters.cfg
  for scenario in $list
  do
    echo "-----------------------Noise Scenario $scenario--------------------" >> Noise.csv
    sed -i "/^Noise_Scenario	=/s/=.*/= $scenario;/" Parameters.cfg

    for Noise_Level in "${NoiseLevel[@]}"
    do
      sed -i "/^Noise_Level	=/s/=.*/= $Noise_Level;/" Parameters.cfg
      echo -ne "\rMode $mode,Scenario 2, Noise Scenario $scenario, Noise Level $Noise_Level"
      draw_progress_bar $(($_counter*100/$_end))
      ./enkiplayground
      _counter=$((${_counter}+1))
    done
  done

mode=$(($mode+1))
done

echo -e "\nCompleted at $(date)!"
destroy_scroll_area
# poweroff
