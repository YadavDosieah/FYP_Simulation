#!/usr/bin/env python

import numpy as np
import csv
import math
import string
import sys

Wheel_vel = []

with open(sys.argv[1],'r') as csvfile:
    data = csv.reader(csvfile, delimiter=',')
    row = next(data)

    data = row[1].split('x=')
    data = data[1].split(' ')
    data = list(filter(None, data))

    for val in data:
        x = float(val)
        x = (1 - math.exp(-x))/(1 + math.exp(-x))
        Wheel_vel.append(x)
        print("{0:0.2f}".format(x))

Wheel_vel_l = Wheel_vel[0::2]
Wheel_vel_r = Wheel_vel[1::2]

print "Wheel Velocities\n&",
for i in range(len(Wheel_vel_l)):
    print "{0:0.2f}".format(Wheel_vel_l[i]),
    if (i != len(Wheel_vel_l)-1):
        print " & ",
    else:
        print "\\\\\n& $\\upsilon_r$ &",

for i in range(len(Wheel_vel_r)):
    print "{0:0.2f}".format(Wheel_vel_r[i]),
    if (i != len(Wheel_vel_l)-1):
        print " & ",
    else:
        print "\\\\",
