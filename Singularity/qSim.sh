#!/bin/bash
##$ -P acsehpc
##$ -q acsehpc.q
#$ -l rmem=1G
#$ -l h_rt=24:30:00
#$ -pe smp 10
#$ -m bea
#$ -M co4xgd@sheffield.ac.uk
#$ -e error.txt
#$ -j y
# 4GB memory, 48hrs max, 10 processors

#Have Singularity call a custom script from your home or other mounted directories
#Don't forget to make the script executable before running by using chmod

chmod +x ../enki/examples/playground/RunOptimisation.sh
singularity exec myimage.simg ../enki/examples/playground/RunOptimisation.sh $1
