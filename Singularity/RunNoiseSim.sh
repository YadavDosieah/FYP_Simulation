#!/bin/bash

#Have Singularity call a custom script from your home or other mounted directories
#Don't forget to make the script executable before running by using chmod

chmod +x ../enki/examples/playground/Auto_Noise.sh
singularity exec myimage.simg ../enki/examples/playground/Auto_Noise.sh $1
