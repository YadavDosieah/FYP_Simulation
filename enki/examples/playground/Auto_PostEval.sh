#!/bin/bash

cd "$(dirname "$0")"
./Post_Eval.sh 0 && mv Analysis.csv Post_Eval0.3.csv
sleep 5
./Post_Eval.sh 1 && mv Analysis.csv Post_Eval1.3.csv
sleep 5
./Post_Eval.sh 2 && mv Analysis.csv Post_Eval2.3.csv
