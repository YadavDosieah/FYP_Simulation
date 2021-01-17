#!/bin/bash

cd "$(dirname "$0")"
./Post_Eval.sh 0 && mv Analysis.csv Post_Eval0.4.csv
sleep 5
./Post_Eval.sh 1 && mv Analysis.csv Post_Eval1.4.csv
sleep 5
./Post_Eval.sh 2 && mv Analysis.csv Post_Eval2.4.csv
