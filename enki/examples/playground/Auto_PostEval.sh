#!/bin/bash

cd "$(dirname "$0")"
./Post_Eval.sh 6 && mv Analysis.csv Post_EvalHetero.csv
