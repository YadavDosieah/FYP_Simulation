#!/bin/bash

cd "$(dirname "$0")"

sed -i "/^Sheep_MaxSpeed_Multiplier	=/s/=.*/= 0.05;/" Parameters.cfg
./Auto_analysis.sh && mv Analysis.csv Analysis_Speed0.1.csv
sleep 10

sed -i "/^Sheep_MaxSpeed_Multiplier	=/s/=.*/= 0.125;/" Parameters.cfg
./Auto_analysis.sh && mv Analysis.csv Analysis_Speed0.25.csv
sleep 10

sed -i "/^Sheep_MaxSpeed_Multiplier	=/s/=.*/= 0.25;/" Parameters.cfg
./Auto_analysis.sh && mv Analysis.csv Analysis_Speed0.5.csv
sleep 10

# sed -i "/^Sheep_MaxSpeed_Multiplier	=/s/=.*/= 0.5;/" Parameters.cfg
# ./Auto_analysis.sh && mv Analysis.csv Analysis_Speed1.csv
sleep 10

sed -i "/^Sheep_MaxSpeed_Multiplier	=/s/=.*/= 1;/" Parameters.cfg
./Auto_analysis.sh && mv Analysis.csv Analysis_Speed2.csv
