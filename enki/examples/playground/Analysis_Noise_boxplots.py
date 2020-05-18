#!/usr/bin/env python

import numpy as np
import csv
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import string
import math
import seaborn as sns
import pandas
from scipy.stats import iqr

def quartiles(data,axis):
    median = np.median(data, axis=3)
    upper_quartile = np.quantile(data, 0.75, axis=3)
    lower_quartile = np.quantile(data, 0.25, axis=3)
    return  np.stack((median-lower_quartile,upper_quartile-median), axis=3)

mode = -1
scenario = -1
noise_scenario = -1

prev_noise_level = -1

Fitness_Val_A = [[[[] for _ in range(6)] for _ in range(4)] for _ in range(3)]
maxSR_A = [[[[] for _ in range(6)] for _ in range(4)] for _ in range(3)]
SR_A = [[[[] for _ in range(6)] for _ in range(4)] for _ in range(3)]

Fitness_Val_B = [[[[] for _ in range(6)] for _ in range(4)] for _ in range(3)]
maxSR_B = [[[[] for _ in range(6)] for _ in range(4)] for _ in range(3)]
SR_B = [[[[] for _ in range(6)] for _ in range(4)] for _ in range(3)]

Fitness_Val_C = [[[[] for _ in range(6)] for _ in range(5)] for _ in range(3)]
maxSR_C = [[[[] for _ in range(6)] for _ in range(5)] for _ in range(3)]
SR_C = [[[[] for _ in range(6)] for _ in range(5)] for _ in range(3)]

Fitness_Val_Simplified = [[[[] for _ in range(6)] for _ in range(3)] for _ in range(3)]
maxSR_Simplified = [[[[] for _ in range(6)] for _ in range(3)] for _ in range(3)]
SR_Simplified = [[[[] for _ in range(6)] for _ in range(3)] for _ in range(3)]

with open('Noise.csv','r') as csvfile:
    data = csv.reader(csvfile, delimiter=',')
    row1 = next(data)
    for row in data:
        if("Controller" in row[0]):
            mode = mode + 1
            scenario = -1
        elif("-Scenario" in row[0]):
            scenario = scenario + 1
            noise_scenario = -1
        elif("Noise Scenario" in row[0]):
            noise_scenario = noise_scenario + 1
            noise_idx = -1
        else:
            noise_level = float(row[1])/0.2
            if(noise_level != prev_noise_level):
                prev_noise_level = noise_level
                noise_idx = noise_idx + 1
                # print(mode,scenario,noise_scenario,noise_idx)
            if(mode == 0):
                Fitness_Val_A[scenario][noise_scenario][noise_idx].append(float(row[3]))
                maxSR_A[scenario][noise_scenario][noise_idx].append(float(row[4])*100)
                SR_A[scenario][noise_scenario][noise_idx].append(float(row[5])*100)
            elif(mode == 1):
                Fitness_Val_B[scenario][noise_scenario][noise_idx].append(float(row[3]))
                maxSR_B[scenario][noise_scenario][noise_idx].append(float(row[4])*100)
                SR_B[scenario][noise_scenario][noise_idx].append(float(row[5])*100)
            elif(mode == 2):
                Fitness_Val_C[scenario][noise_scenario][noise_idx].append(float(row[3]))
                maxSR_C[scenario][noise_scenario][noise_idx].append(float(row[4])*100)
                SR_C[scenario][noise_scenario][noise_idx].append(float(row[5])*100)
            elif(mode == 3):
                Fitness_Val_Simplified[scenario][noise_scenario][noise_idx].append(float(row[3]))
                maxSR_Simplified[scenario][noise_scenario][noise_idx].append(float(row[4])*100)
                SR_Simplified[scenario][noise_scenario][noise_idx].append(float(row[5])*100)


Fitness_Val_A = np.array(Fitness_Val_A)
Fitness_Val_B = np.array(Fitness_Val_B)
Fitness_Val_C = np.array(Fitness_Val_C)
Fitness_Val_Simplified = np.array(Fitness_Val_Simplified)

print(Fitness_Val_A.shape)
print(Fitness_Val_B.shape)
print(Fitness_Val_C.shape)
print(Fitness_Val_Simplified.shape)

x = np.array([0,0.2,0.4,0.6,0.8,1.0])

titles = ["Shepherding", "Obj. Clustering", "Combined Scenario"]

Values = pandas.DataFrame()
Counter = 0
for j in range(3):
    for k in range(5):
        Counter = Counter + 1
        for i in range(3):
            Values1 = pandas.DataFrame(Fitness_Val[0,i,j,k,:], columns=[titles[i]]).assign(Legend='Controller A')
            Values2 = pandas.DataFrame(Fitness_Val[1,i,j,k,:], columns=[titles[i]]).assign(Legend='Controller B')
            Values3 = pandas.DataFrame(Fitness_Val[2,i,j,k,:], columns=[titles[i]]).assign(Legend='Controller C')
            Values4 = pandas.DataFrame(Fitness_Val[3,i,j,k,:], columns=[titles[i]]).assign(Legend='Simplified Controller')

            Values = pandas.concat([Values,Values1,Values2,Values3,Values4],sort=False)


        mdf = pandas.melt(Values, id_vars=['Legend'], var_name=['Letter'])

        plt.rcParams.update({'font.size': 22})
        fig, ax = plt.subplots(nrows=1, ncols=1,figsize=(12,6))
        ax = sns.boxplot(x="Letter", y="value",hue="Legend", data=mdf, width = 0.5, linewidth = 2.0)
        ax.legend_.remove()
        plt.grid(axis='y')
        ax.set(yscale="log")

        fig.add_subplot(111, frameon=False)
        # hide tick and tick label of the big axis
        plt.tick_params(labelcolor='none', top=False, bottom=False, left=False, right=False)
        fig.legend(loc="lower center", ncol = 4, handleheight=1.8,fontsize=16)
        fig.suptitle(str((j+1)*5) + " shepherds & " + str((k+1)*10) + " sheep and/or objects")

        plt.subplots_adjust(bottom=0.2, top = 0.9, right = 0.95, left = 0.1)
        ax.set_ylabel('Total Fitness Value after 1500s')
        ax.set_xlabel('')
        plt.savefig("Boxplot"+str(j)+","+str(k))
        # plt.xlabel(None)
        # plt.show()
