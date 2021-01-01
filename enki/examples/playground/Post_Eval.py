#!/usr/bin/env python3

import subprocess
import sys, os
import csv
import numpy as np
import pandas as pd
from sh import sed


mode = sys.argv[1]
dimension = 0

def read():
    with open('Results/mode' + mode + '/Parameters_File.csv','r') as csvfile:
        data = csv.reader(csvfile, delimiter=',')
        data = list(data)[::2]
        # print(len(data))
        for idx,row in enumerate(data, start=1):
            Wheel_vel = row[1].split('x=')
            Wheel_vel = Wheel_vel[1].split(' ')
            Wheel_vel = list(filter(None, Wheel_vel))
            for j in range(dimension):
                # print(Wheel_vel[j])
                sed(['-i', '/^x{:d}	=/s/=.*/= {:f};/'.format(j,float(Wheel_vel[j])), 'Parameters.cfg'])
            for j in range(16-dimension):
                sed(['-i', '/^x{:d}	=/s/=.*/= 0.0;/'.format(j+dimension), 'Parameters.cfg'])
            print("Evolution {:d} running ...".format(idx))
            subprocess.call("./enkiplayground", shell=True)

if(mode == "0"):
    print("mode 0 - Only Shepherding")
    dimension = 12
    read()
elif(mode == "1"):
    print("mode 1 - Only Object Clustering")
    dimension = 12
    read()
elif(mode == "2"):
    print("mode 2 - Combined Shepherding and Object Clustering")
    dimension = 16
    read()
elif(mode == "3"):
    print("mode 3 - Simplified Controller")
    dimension = 8
    read()
