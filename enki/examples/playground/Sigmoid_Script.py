#!/usr/bin/env python

import numpy as np
import csv
import math
import string
import sys

with open(sys.argv[1],'r') as csvfile:
    data = csv.reader(csvfile, delimiter=',')
    row = next(data)

    data = row[1].split('x=')
    data = data[1].split(' ')
    data = list(filter(None, data))

    for val in data:
        x = float(val)
        x = (1 - math.exp(-x))/(1 + math.exp(-x))
        print("{0:0.2f}".format(x))
