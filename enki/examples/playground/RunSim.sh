#!/bin/bash

#Calling ls directly using the exec command
pwd
cd ../enki
cmake .
cd examples/playground/
make enkiplayground

sed -i '/^mode		=/s/=.*/= 0;/' Parameters.cfg
./enkiplayground

sed -i '/^mode		=/s/=.*/= 1;/' Parameters.cfg
./enkiplayground

sed -i '/^mode		=/s/=.*/= 2;/' Parameters.cfg
./enkiplayground
