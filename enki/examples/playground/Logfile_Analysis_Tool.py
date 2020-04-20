#!/usr/bin/env python

import numpy as np
import csv
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import string
import math
import sys

# V_r = [[]]
# V_l = [[]]
# Fitness_Val = []

def readLogfile(filename):
    if(("mode2") in filename):
        No_Of_Var = 8
    else:
        No_Of_Var = 6

    V_r = [[] for _ in range(No_Of_Var)]
    V_l = [[] for _ in range(No_Of_Var)]

    No_Of_Generation = 80
    Population_size = 20
    No_Of_Trials = 50
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
                    V_l[i].append(V_l_temp)

                    V_r_temp = float(row[4+(i*2)])
                    V_r_temp = (1-math.exp(-V_r_temp))/(1+math.exp(-V_r_temp))
                    V_r[i].append(V_r_temp)
            Fitness_Val[generation-1][population-1].append(float(row[3+(2*No_Of_Var)]))
        # print(Fitness_Val[0][0][0])
        # print(V_r[0])
        # print(V_l[0])
        return V_l,V_r,Fitness_Val

if __name__ == "__main__":
    V_l,V_r,Fitness_Val = readLogfile(sys.argv[1])
    Fitness_Val = np.array(Fitness_Val)
    array = Fitness_Val
    array = np.mean(array, axis=2)
    array = np.mean(array, axis=1)
    array = array.ravel()
    print(array.shape)
    plt.plot(array)
    plt.show()
