#!/usr/bin/env python

import numpy as np
import csv
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import string
import math
import sys
from scipy.interpolate import griddata
from scipy.ndimage.filters import gaussian_filter1d

# V_r = [[]]
# V_l = [[]]
# Fitness_Val = []

No_Of_Generation = 80
Population_size = 20
No_Of_Trials = 50

def readLogfile(filename):
    if(("mode2") in filename):
        No_Of_Var = 8
    else:
        No_Of_Var = 6

    v_r = [[] for _ in range(No_Of_Var)]
    v_l = [[] for _ in range(No_Of_Var)]

    # No_Of_Generation = 10
    # Population_size = 2
    # No_Of_Trials = 2
    V_r = [[[[] for _ in range(Population_size)] for _ in range(No_Of_Generation)]  for _ in range(No_Of_Var)]
    V_l = [[[[] for _ in range(Population_size)] for _ in range(No_Of_Generation)]  for _ in range(No_Of_Var)]
    Fitness_Val = [[[] for _ in range(Population_size)] for _ in range(No_Of_Generation)]


    with open(filename,'r') as csvfile:
        data = csv.reader(csvfile, delimiter=',')
        row = next(data)

        for row in data:
            generation = int(row[0])
            population = int(row[1])
            trial = int(row[2])
            if(trial==1):
                for i in range(No_Of_Var):
                    V_l_temp = float(row[3+(i*2)])
                    V_l_temp = (1-math.exp(-V_l_temp))/(1+math.exp(-V_l_temp))
                    V_l[i][generation-1][population-1].append(V_l_temp)

                    V_r_temp = float(row[4+(i*2)])
                    V_r_temp = (1-math.exp(-V_r_temp))/(1+math.exp(-V_r_temp))
                    V_r[i][generation-1][population-1].append(V_r_temp)
            Fitness_Val[generation-1][population-1].append(float(row[3+(2*No_Of_Var)]))
        # print(Fitness_Val[0][0][0])
        # print(V_r[0])
        # print(V_l[0])
        V_r = np.array(V_r)
        V_l = np.array(V_l)

        for i in range(No_Of_Var):
            v_r[i] = V_r[i,:,:].ravel()
            v_l[i] = V_l[i,:,:].ravel()

        return v_l,v_r,Fitness_Val

if __name__ == "__main__":
    V_l,V_r,Fitness_Val = readLogfile(sys.argv[1])
    Fitness_Val = np.array(Fitness_Val)
    array = np.mean(Fitness_Val, axis=2)
    max = np.max(Fitness_Val, axis=2)
    max = max.ravel()
    min = np.min(Fitness_Val, axis=2)
    min = min.ravel()
    y = array.ravel()

    #range(start_value, end_value, step)
    y_sample = [np.min(y[i:i+Population_size]) for i in range(0, len(y), Population_size)]
    y_index = [np.argmin(y[i:i+Population_size])+i for i in range(0, len(y), Population_size)]

    x = np.linspace(0, (No_Of_Generation), num = len(y_sample))
    # Slice notation a[start_index:end_index:step]

    max_sampled = max[y_index]
    min_sampled = min[y_index]

    max = [np.max(max[i:i+Population_size]) for i in range(0, len(max), Population_size)]
    min = [np.min(min[i:i+Population_size]) for i in range(0, len(min), Population_size)]
    x2 = np.linspace(0, (No_Of_Generation), num = len(max))


    plt.rcParams['font.size'] = 18
    plt.semilogy(x,y_sample,color='k',label='Average Fitness Value')
    plt.fill_between(x, min_sampled, max_sampled, alpha=0.2,color='b',label='Min/Max Fitness Value')
    # plt.fill_between(x2, min, max, alpha=0.2,color='b')

    plt.xlabel('Generation')
    plt.ylabel('Fitness Value')
    plt.tight_layout()
    plt.legend()
    plt.grid(True)
    plt.show()

    # # create x-y points to be used in heatmap
    # xi = np.linspace(-1, 1, 100)
    # yi = np.linspace(-1, 1, 100)
    #
    # # array = (Fitness_Val[:,:,5]).ravel()
    # # array = np.log10(array)
    #
    # # Z is a matrix of x-y values
    # zi = griddata((V_l[2], V_r[2]), array, (xi[None,:], yi[:,None]), method='cubic')
    #
    # zmin = min(array)
    # zmax = max(array)
    # zi[(zi<zmin) | (zi>zmax)] = None
    #
    # # Create the contour plot
    # CS = plt.contourf(xi, yi, zi, 15, cmap=plt.cm.rainbow,
    #                   vmax=zmax, vmin=zmin)
    # plt.colorbar()
    # plt.show()
