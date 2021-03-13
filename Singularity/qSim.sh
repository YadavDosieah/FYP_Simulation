#!/bin/bash
#$ -P acsehpc
#$ -q acsehpc.q
#$ -l rmem=1G
#$ -l h_rt=06:00:00
#$ -pe smp 20
#$ -m bea
#$ -M co4xgd@sheffield.ac.uk
#$ -e error.txt
#$ -j y
# 4GB memory, 48hrs max, 10 processors

#Have Singularity call a custom script from your home or other mounted directories
#Don't forget to make the script executable before running by using chmod

# chmod +x ../enki/examples/playground/RunOptimisation.sh
# singularity exec myimage.simg ../enki/examples/playground/RunOptimisation.sh

# chmod +x ../enki/examples/playground/Auto_analysis.sh
# singularity exec myimage.simg ../enki/examples/playground/Auto_analysis.sh

chmod +x ../enki/examples/playground/Post_Eval.sh
singularity exec myimage.simg ../enki/examples/playground/Post_Eval.sh

# chmod +x ../enki/examples/playground/Auto_analysis_varyingSpeed.sh
# singularity exec myimage.simg ../enki/examples/playground/Auto_analysis_varyingSpeed.sh
