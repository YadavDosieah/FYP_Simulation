#!/usr/bin/env python3

import numpy as np
import csv
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.colors import to_rgba
import string
import math
import pandas
import seaborn as sns
from itertools import cycle

mode = -1

Fitness_Val = [[[] for _ in range(6)] for _ in range(6)]
maxSR = [[[] for _ in range(6)] for _ in range(6)]
SR = [[[] for _ in range(6)] for _ in range(6)]

with open('Analysis.csv','r') as csvfile:
    data = csv.reader(csvfile, delimiter=',')
    row1 = next(data)
    for row in data:
        if("mode" in row[0]):
            mode = mode + 1
        else:
            objIdx = int(row[2])//2
            Fitness_Val[mode][objIdx].append(float(row[4]))
            maxSR[mode][objIdx].append(float(row[5])*100)
            SR[mode][objIdx].append(float(row[6])*100)

Fitness_Val = np.array(Fitness_Val)
AvgFitness = np.mean(Fitness_Val, axis=2)
AvgFitness= AvgFitness/2

# print(maxSR[:,:,0:50])
maxSR = np.array(maxSR)
AvgMaxSR = np.mean(maxSR, axis=2)

SR = np.array(SR)
AvgSR = np.mean(SR, axis=2)

sheep =  ["00", "02", "04", "06", "08", "10"]
object = ["10", "08", "06", "04", "02", "00"]
fname = ["Controller A", "Controller B", "Controller C", "Simplified Controller C","Simplified Controller A","Simplified Controller B"]
titles = ["Complex Controller", "Simplified Controller"]

sns.set_palette('bright')
# Values = pd.DataFrame(AvgMaxSR, index=fname, columns=sheep)
Max = (AvgFitness.max())
Min = (AvgFitness.min())

data = []

for j in range(6):
    Values = np.zeros((6,6))
    # Values=[[[] for _ in range(6)] for _ in range(6)]
    for i in range(6):
        Values[i][i] = AvgMaxSR[j,i]
    Values = pandas.DataFrame(Values,index=sheep, columns=object)
    Values = Values.sort_index(1)
    Values = Values.sort_index(0,ascending=False)
    data.append(Values)

mask = (np.ma.diag([1,1,1,1,1,1]))
mask = np.logical_not(mask)
# print(mask)
j_arr = [0,1,2]
for i in range(2):
    fig,axes = plt.subplots(ncols=4, gridspec_kw=dict(width_ratios=[1,1,1,0.1]),figsize=(9,4))
    for j in range(3):
        if(j==0):
            sns.heatmap(data[(3*i)+j_arr[j]],mask=mask,cmap=sns.color_palette("RdYlGn",100),vmin=0, vmax=100,annot=True,fmt=".1f",cbar=False,ax=axes[j])
        else:
            sns.heatmap(data[(3*i)+j_arr[j]],mask=mask,cmap=sns.color_palette("RdYlGn",100),vmin=0, vmax=100,annot=True,fmt=".1f",cbar=False,ax=axes[j],yticklabels=False)
        axes[j].set_title(fname[(3*i)+j_arr[j]])
    j_arr = [1,2,0]

    fig.add_subplot(111, frameon=False)
    # hide tick and tick label of the big axis
    plt.tick_params(labelcolor='none', top=False, bottom=False, left=False, right=False)
    plt.xlabel('No. of Objects',fontsize=12)
    plt.ylabel('No. of Sheep',fontsize=12)

    cbar = fig.colorbar(axes[1].collections[0], cax=axes[3])
    cbar.set_ticks([0, 20, 40, 60, 80, 100])
    cbar.set_ticklabels(['0%', '20%', '40%', '60%','80%', '100%'])

    # cbar.set_ticks([Min, Max])
    # cbar.set_ticklabels(['Min','Max'])

    plt.tight_layout()
    plt.savefig(titles[i],bbox_inches='tight',pad_inches = 0)
    # plt.savefig(fname[i]+"Fitness_Val",bbox_inches='tight',pad_inches = 0)

# plt.show()
