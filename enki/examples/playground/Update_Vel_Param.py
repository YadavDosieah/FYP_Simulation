#!/usr/bin/env python3

import subprocess
import sys, os
import csv
import numpy as np
import pandas as pd
from sh import sed


file = sys.argv[1]
Evo = int(sys.argv[2])

def read():
    try:
        with open(file,'r') as csvfile:
            data = csv.reader(csvfile, delimiter=',')
            data = list(data)[::2]
            # print(len(data))
            try:
                data = data[Evo-1]
                # print(data)
            except Exception as e:
                print("Invalid Evolution")
                exit()

            try:
                Wheel_vel = data[1].split('x=')
                Wheel_vel = Wheel_vel[1].split(' ')
                Wheel_vel = list(filter(None, Wheel_vel))
                # print(len(Wheel_vel))
                for j in range(len(Wheel_vel)):
                    # print(Wheel_vel[j])
                    sed(['-i', '/^x{:d}	=/s/=.*/= {:f};/'.format(j,float(Wheel_vel[j])), 'Parameters.cfg'])
                for j in range(48-len(Wheel_vel)):
                    sed(['-i', '/^x{:d}	=/s/=.*/= 0.0;/'.format(j+len(Wheel_vel)), 'Parameters.cfg'])

            except Exception as e:
                print("File Corrupted. Please Check File")
    except Exception as e:
        print("Invalid File")

read()
