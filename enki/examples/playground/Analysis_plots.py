#!/usr/bin/env python

import numpy as np
import csv
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import string
import math
import pandas as pd
import seaborn as sns

mode = -1
scenario = -1
trial = 0

Fitness_Val = [[[[[] for _ in range(5)] for _ in range(3)] for _ in range(3)] for _ in range(4)]
maxSR = [[[[[] for _ in range(5)] for _ in range(3)] for _ in range(3)] for _ in range(4)]
SR = [[[[[] for _ in range(5)] for _ in range(3)] for _ in range(3)] for _ in range(4)]

with open('Analysis.csv','r') as csvfile:
    data = csv.reader(csvfile, delimiter=',')
    row1 = next(data)
    for row in data:
        if("mode" in row[0]):
            mode = mode + 1
            scenario = -1
            # print("mode {:d}".format(mode))
        if("Scenario" in row[0]):
            scenario = scenario + 1
            trial = 0
            # print(scenario)
        else:
            if(scenario == 0):
                shepherdIdx = int(row[1])/5
                sheepIdx = int(row[2])/10
                Fitness_Val[mode][scenario][shepherdIdx-1][sheepIdx-1].append(float(row[4]))
                maxSR[mode][scenario][shepherdIdx-1][sheepIdx-1].append(float(row[5])*100)
                SR[mode][scenario][shepherdIdx-1][sheepIdx-1].append(float(row[6])*100)

            if(scenario == 1):
                shepherdIdx = int(row[1])/5
                objIdx = int(row[3])/10
                Fitness_Val[mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[4]))
                maxSR[mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[5])*100)
                SR[mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[6])*100)

            if(scenario == 2):
                shepherdIdx = int(row[1])/5
                objIdx = int(row[3])/10
                Fitness_Val[mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[4]))
                maxSR[mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[5])*100)
                SR[mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[6])*100)

            if(scenario == 3):
                shepherdIdx = int(row[1])/5
                objIdx = int(row[3])/10
                Fitness_Val[mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[4]))
                maxSR[mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[5])*100)
                SR[mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[6])*100)

Fitness_Val = np.array(Fitness_Val)
AvgFitness = np.mean(Fitness_Val, axis=4)
AvgFitness[:,2,:,:] = AvgFitness[:,2,:,:]/2

maxSR = np.array(maxSR)
AvgMaxSR = np.mean(maxSR, axis=4)

SR = np.array(SR)
AvgSR = np.mean(SR, axis=4)

shepherds = ["05", "10", "15"]
sheep = ["10", "20", "30", "40", "50"]
titles = ["Shepherding", "Obj. Clustering", "Combined Scenario"]
fname = ["Controller A", "Controller B", "Controller C", "Simplified Controller"]

sns.set_palette('bright')

# print(AvgFitness.shape)
for i in range(4):
    Max = (AvgFitness.max())
    Min = (AvgFitness.min())

# print(Max)
# print(Min)
for i in range(4):
    # plt.figure(figsize=(9,3))
    fig,axes = plt.subplots(ncols=4, gridspec_kw=dict(width_ratios=[1,1,1,0.1]),figsize=(9,4))
    for j in range(3):
        # print("mode {:d}, Scenario {:d}".format(i,j))
        # print(AvgSR[i,j,:,:])
        Values = pd.DataFrame(AvgMaxSR[i,j,:,:], index=shepherds, columns=sheep)
        # print(Values)
        # Values = Values.sort_index()
        Values = Values[["50", "40", "30", "20", "10"]]
        Values = Values.transpose()
        # print(Values)
        # plt.subplot(1, 3, j+1)
        if(j==0):
            sns.heatmap(Values,cmap=sns.color_palette("RdYlGn",100),vmin=0, vmax=100,annot=True,fmt=".1f",cbar=False,ax=axes[j])
        else:
            sns.heatmap(Values,cmap=sns.color_palette("RdYlGn",100),vmin=0, vmax=100,annot=True,fmt=".1f",cbar=False,ax=axes[j],yticklabels=False)

        # if(j==0):
        #     sns.heatmap(Values,cmap=sns.color_palette("RdYlGn_r",100),vmin=Min, vmax=Max,annot=True,fmt="3.2g",cbar=False,ax=axes[j])
        # else:
        #     sns.heatmap(Values,cmap=sns.color_palette("RdYlGn_r",100),vmin=Min, vmax=Max,annot=True,fmt="3.2g",cbar=False,ax=axes[j],yticklabels=False)
        axes[j].set_title(titles[j])

    fig.add_subplot(111, frameon=False)
    # hide tick and tick label of the big axis
    plt.tick_params(labelcolor='none', top=False, bottom=False, left=False, right=False)
    plt.xlabel('No. of Shepherds',fontsize=12)
    plt.ylabel('No. of Sheep,Objects',fontsize=12)

    cbar = fig.colorbar(axes[1].collections[0], cax=axes[3])
    cbar.set_ticks([0, 20, 40, 60, 80, 100])
    cbar.set_ticklabels(['0%', '20%', '40%', '60%','80%', '100%'])

    # cbar.set_ticks([Min, Max])
    # cbar.set_ticklabels(['Min','Max'])

    plt.tight_layout()
    plt.savefig(fname[i],bbox_inches='tight',pad_inches = 0)
    # plt.savefig(fname[i]+"Fitness_Val",bbox_inches='tight',pad_inches = 0)

# plt.show()
