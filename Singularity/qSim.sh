#!/bin/bash
#$ -P acsehpc
#$ -q acsehpc.q
#$ -l rmem=8G
#$ -l h_rt=48:00:00
#$ -pe smp 20
#$ -m bea
#$ -M gydosieah1@sheffield.ac.uk
#$ -o output.txt
#$ -e error.txt
#$ -j y
# 8GB memory, 48hrs max, 20 processors

#Have Singularity call a custom script from your home or other mounted directories
#Don't forget to make the script executable before running by using chmod

chmod +x ../enki/examples/playground/RunSim.sh
singularity exec myimage.simg ../enki/examples/playground/RunSim.sh
