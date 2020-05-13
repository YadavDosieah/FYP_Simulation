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

#######################Controller A
Fitness_Val_A = np.array(Fitness_Val_A)
AvgFitness_Val_A = np.median(Fitness_Val_A, axis=3)
StdFitness_Val_A = np.std(Fitness_Val_A, axis=3)
IQRFitness_Val_A = quartiles(Fitness_Val_A, axis=3)

maxSR_A = np.array(maxSR_A)
SR_A = np.array(SR_A)
maxSR_A[:,2,:,:] = SR_A[:,2,:,:]
AvgMaxSR_A = np.median(maxSR_A, axis=3)
StdMaxSR_A = np.std(maxSR_A, axis=3)
IQRMaxSR_A = quartiles(maxSR_A, axis=3)

# SR_A = np.array(SR_A)
AvgSR_A = np.median(SR_A, axis=3)
StdSR_A = np.std(SR_A, axis=3)
IQRSR_A = quartiles(SR_A, axis=3)

#######################Controller B
Fitness_Val_B = np.array(Fitness_Val_B)
AvgFitness_Val_B = np.median(Fitness_Val_B, axis=3)
StdFitness_Val_B = np.std(Fitness_Val_B, axis=3)
IQRFitness_Val_B = quartiles(Fitness_Val_B, axis=3)

maxSR_B = np.array(maxSR_B)
SR_B = np.array(SR_B)
maxSR_B[:,2,:,:] = SR_B[:,2,:,:]
AvgMaxSR_B = np.median(maxSR_B, axis=3)
StdMaxSR_B = np.std(maxSR_B, axis=3)
IQRMaxSR_B = quartiles(maxSR_B, axis=3)

AvgSR_B = np.median(SR_B, axis=3)
StdSR_B = np.std(SR_B, axis=3)
IQRSR_B = quartiles(SR_B, axis=3)

#######################Controller C
Fitness_Val_C = np.array(Fitness_Val_C)
AvgFitness_Val_C = np.median(Fitness_Val_C, axis=3)
StdFitness_Val_C = np.std(Fitness_Val_C, axis=3)
IQRFitness_Val_C = quartiles(Fitness_Val_C, axis=3)

maxSR_C = np.array(maxSR_C)
SR_C = np.array(SR_C)
maxSR_C[:,2,:,:] = SR_C[:,2,:,:]
AvgMaxSR_C = np.median(maxSR_C, axis=3)
StdMaxSR_C = np.std(maxSR_C, axis=3)
IQRMaxSR_C = quartiles(maxSR_C, axis=3)

# SR_C = np.array(SR_C)
AvgSR_C = np.median(SR_C, axis=3)
StdSR_C = np.std(SR_C, axis=3)
IQRSR_C = quartiles(SR_C, axis=3)

x = np.array([0,0.2,0.4,0.6,0.8,1.0])

titles = ["Shepherding", "Obj. Clustering", "Combined Scenario"]


fig, ax = plt.subplots(nrows=1, ncols=3,figsize=(9,4))
for i in range(3):
    for j in range(4):
        ax[i].errorbar(x,AvgMaxSR_A[i,j,:],yerr=IQRMaxSR_A[i,j,:].T)#scenario,noise scenario, noise level
        ax[i].set_title(titles[i])
        ax[i].grid(b=True,axis='y')
        ax[i].set_ylim(0, 105)
        ax[i].set_xlim(0, 1.05)
        ax[i].xaxis.set_ticks(np.arange(0, 1.1, 0.2))

fig.add_subplot(111, frameon=False)
# hide tick and tick label of the big axis
plt.tick_params(labelcolor='none', top=False, bottom=False, left=False, right=False)
plt.xlabel('Noise Level',fontsize=12)
plt.ylabel('Success Rate (%)',fontsize=12)
plt.subplots_adjust(bottom=0.23, top = 0.93, right = 0.97, left = 0.075)
fig.legend(['Sheep & Object','Shepherd','Goal','All'],loc="lower center", ncol = 6,fontsize=12)
plt.savefig("Controller A - Noise")

fig, ax = plt.subplots(nrows=1, ncols=3,figsize=(9,4))
for i in range(3):
    for j in range(4):
        ax[i].errorbar(x,AvgMaxSR_B[i,j,:],yerr=IQRMaxSR_B[i,j,:].T)#scenario,noise scenario, noise level
        ax[i].set_title(titles[i])
        ax[i].grid(b=True,axis='y')
        ax[i].set_ylim(0, 105)
        ax[i].set_xlim(0, 1.05)
        ax[i].xaxis.set_ticks(np.arange(0, 1.1, 0.2))

fig.add_subplot(111, frameon=False)
# hide tick and tick label of the big axis
plt.tick_params(labelcolor='none', top=False, bottom=False, left=False, right=False)
plt.xlabel('Noise Level',fontsize=12)
plt.ylabel('Success Rate (%)',fontsize=12)
plt.subplots_adjust(bottom=0.23, top = 0.93, right = 0.97, left = 0.075)
fig.legend(['Shepherd','Sheep & Object','Goal','All'],loc="lower center", ncol = 6,fontsize=12)
plt.savefig("Controller B - Noise")

fig, ax = plt.subplots(nrows=1, ncols=3,figsize=(9,4))
for i in range(3):
    for j in range(5):
        ax[i].errorbar(x,AvgMaxSR_C[i,j,:],yerr=IQRMaxSR_C[i,j,:].T)#scenario,noise scenario, noise level
        ax[i].set_title(titles[i])
        ax[i].grid(b=True,axis='y')
        ax[i].set_ylim(0, 105)
        ax[i].set_xlim(0, 1.05)
        ax[i].xaxis.set_ticks(np.arange(0, 1.1, 0.2))

fig.add_subplot(111, frameon=False)
# hide tick and tick label of the big axis
plt.tick_params(labelcolor='none', top=False, bottom=False, left=False, right=False)
plt.xlabel('Noise Level',fontsize=12)
plt.ylabel('Success Rate (%)',fontsize=12)
plt.subplots_adjust(bottom=0.23, top = 0.93, right = 0.97, left = 0.075)
fig.legend(['Sheep','Shepherd','Object','Goal','All'],loc="lower center", ncol = 6,fontsize=12)
plt.savefig("Controller C - Noise")

# plt.show()

# fig,ax2 = plt.subplots(1)
# Legend = ['Sheep','Shepherd','Goal','All']
# Values1 = pandas.DataFrame(maxSR_A[0,0,:].transpose(), columns=x).assign(Legend='Sheep')
# Values2 = pandas.DataFrame(maxSR_A[0,1,:].transpose(), columns=x).assign(Legend='Shepherd')
# Values3 = pandas.DataFrame(maxSR_A[0,2,:].transpose(), columns=x).assign(Legend='Goal')
# Values4 = pandas.DataFrame(maxSR_A[0,3,:].transpose(), columns=x).assign(Legend='All')
#
# Values = pandas.concat([Values1, Values2, Values3, Values4])
# mdf1 = pandas.melt(Values1, id_vars=['Legend'], var_name=['Letter'])
# mdf2 = pandas.melt(Values2, id_vars=['Legend'], var_name=['Letter'])
# mdf3 = pandas.melt(Values3, id_vars=['Legend'], var_name=['Letter'])
# mdf4 = pandas.melt(Values4, id_vars=['Legend'], var_name=['Letter'])
#
# mdf = pandas.melt(Values, id_vars=['Legend'], var_name=['Letter'])
# ax = sns.boxplot(x="Letter", y="value", hue="Legend", data=mdf)
#
# paper_rc = {'lines.linewidth': 1, 'lines.markersize': 1}
# sns.set_context("paper", rc = paper_rc)
# sns.pointplot(x='Letter', y='value', data=mdf1.groupby('Letter', as_index=False).median(), ax=ax)
# sns.pointplot(x='Letter', y='value', data=mdf2.groupby('Letter', as_index=False).median(), ax=ax, color='darkorange')
# sns.pointplot(x='Letter', y='value', data=mdf3.groupby('Letter', as_index=False).median(), ax=ax, color='green')
# sns.pointplot(x='Letter', y='value', data=mdf4.groupby('Letter', as_index=False).median(), ax=ax, color='red')
#
# for i,artist in enumerate(ax2.artists):
#     # Set the linecolor on the artist to the facecolor, and set the facecolor to None
#     col = artist.get_facecolor()
#     artist.set_edgecolor(col)
#     artist.set_facecolor('None')
#
#     # Each box has 6 associated Line2D objects (to make the whiskers, fliers, etc.)
#     # Loop over them here, and use the same colour as above
#     for j in range(i*6,i*6+6):
#         line = ax2.lines[j]
#         line.set_color(col)
#         line.set_mfc(col)
#         line.set_mec(col)
#
#
# # Also fix the legend
# for legpatch in ax2.get_legend().get_patches():
#     col = legpatch.get_facecolor()
#     legpatch.set_edgecolor(col)
#     legpatch.set_facecolor('None')
#     legpatch.set_linewidth(2.0)
#
# plt.grid(axis='y')
# plt.show()
