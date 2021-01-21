#!/usr/bin/env python3

import numpy as np
import csv
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import string
import math
import pandas
import seaborn as sns

mode = -1
scenario = -1
trial = 0

Fitness_Val = [[[[[] for _ in range(5)] for _ in range(3)] for _ in range(3)] for _ in range(6)]
maxSR = [[[[[] for _ in range(5)] for _ in range(3)] for _ in range(3)] for _ in range(6)]
SR = [[[[[] for _ in range(5)] for _ in range(3)] for _ in range(3)] for _ in range(6)]

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
                shepherdIdx = int(row[1])//5
                sheepIdx = int(row[2])//10
                Fitness_Val[mode][scenario][shepherdIdx-1][sheepIdx-1].append(float(row[4]))
                maxSR[mode][scenario][shepherdIdx-1][sheepIdx-1].append(float(row[5])*100)
                SR[mode][scenario][shepherdIdx-1][sheepIdx-1].append(float(row[6])*100)

            if(scenario == 1):
                shepherdIdx = int(row[1])//5
                objIdx = int(row[3])//10
                Fitness_Val[mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[4]))
                maxSR[mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[5])*100)
                SR[mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[6])*100)

            # if(scenario == 2):
            #     shepherdIdx = int(row[1])/5
            #     objIdx = int(row[3])/10
            #     Fitness_Val[mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[4]))
            #     maxSR[mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[5])*100)
            #     SR[mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[6])*100)

            if(scenario == 2):
                shepherdIdx = int(row[1])//5
                objIdx = int(row[3])//5
                Fitness_Val[mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[4]))
                maxSR[mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[5])*100)
                SR[mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[6])*100)

            if(scenario == 3):
                shepherdIdx = int(row[1])//5
                objIdx = int(row[3])//10
                Fitness_Val[mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[4]))
                maxSR[mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[5])*100)
                SR[mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[6])*100)

Fitness_Val = np.array(Fitness_Val)
Fitness_Val[:,2,:,:,:] = Fitness_Val[:,2,:,:,:]/2
# print(Fitness_Val.shape)
Fitness_Val=Fitness_Val[:,:,:,:,0:50]
Fitness_Val_merged = Fitness_Val.reshape(6,3,750)
# print(Fitness_Val.shape)

titles = ["Shepherding", "Obj. Clustering", "Combined Scenario"]
fname = ["Controller A", "Controller B", "Controller C", "Simplified Controller","Simplified Controller Scenario A","Simplified Controller Scenario B"]

sns.set_palette('bright')
#
# Values = pandas.DataFrame()
# Counter = 0
# for j in range(3):
#     for k in range(5):
#         Counter = Counter + 1
#         for i in range(3):
#             Values1 = pandas.DataFrame(Fitness_Val[0,i,j,k,:], columns=[titles[i]]).assign(Legend='Controller A')
#             Values2 = pandas.DataFrame(Fitness_Val[1,i,j,k,:], columns=[titles[i]]).assign(Legend='Controller B')
#             Values3 = pandas.DataFrame(Fitness_Val[2,i,j,k,:], columns=[titles[i]]).assign(Legend='Controller C')
#             Values4 = pandas.DataFrame(Fitness_Val[3,i,j,k,:], columns=[titles[i]]).assign(Legend='Simplified Controller')
#
#             Values = pandas.concat([Values,Values1,Values2,Values3,Values4],sort=False)
#
#
#         mdf = pandas.melt(Values, id_vars=['Legend'], var_name=['Letter'])
#
#         plt.rcParams.update({'font.size': 22})
#         fig, ax = plt.subplots(nrows=1, ncols=1,figsize=(12,6))
#         ax = sns.boxplot(x="Letter", y="value",hue="Legend", data=mdf, width = 0.5, linewidth = 2.0)
#         ax.legend_.remove()
#         plt.grid(axis='y')
#         ax.set(yscale="log")
#
#         fig.add_subplot(111, frameon=False)
#         # hide tick and tick label of the big axis
#         plt.tick_params(labelcolor='none', top=False, bottom=False, left=False, right=False)
#         fig.legend(loc="lower center", ncol = 4, handleheight=1.8,fontsize=16)
#         fig.suptitle(str((j+1)*5) + " shepherds & " + str((k+1)*10) + " sheep and/or objects")
#
#         plt.subplots_adjust(bottom=0.2, top = 0.9, right = 0.95, left = 0.1)
#         ax.set_ylabel('Total Fitness Value after 1500s')
#         ax.set_xlabel('')
#         plt.savefig("Boxplot"+str((j*5)+k))
#         # plt.xlabel(None)
#         # plt.show()

Values = pandas.DataFrame()
for i in range(3):
    Values1 = pandas.DataFrame(Fitness_Val_merged[0,i,:], columns=[titles[i]]).assign(Legend='Cont. A')
    Values2 = pandas.DataFrame(Fitness_Val_merged[1,i,:], columns=[titles[i]]).assign(Legend='Cont. B')
    Values3 = pandas.DataFrame(Fitness_Val_merged[2,i,:], columns=[titles[i]]).assign(Legend='Cont. C')
    Values4 = pandas.DataFrame(Fitness_Val_merged[3,i,:], columns=[titles[i]]).assign(Legend='Simp. Controller')
    Values5 = pandas.DataFrame(Fitness_Val_merged[4,i,:], columns=[titles[i]]).assign(Legend='Simpl. Controller Sc. A')
    Values6 = pandas.DataFrame(Fitness_Val_merged[5,i,:], columns=[titles[i]]).assign(Legend='Simpl.Controller Sc. B')


    Values = pandas.concat([Values,Values1,Values2,Values3,Values4,Values5,Values6],sort=False)


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
fig.legend(loc="lower center", ncol = 6, handleheight=1.8,fontsize=12)

plt.subplots_adjust(bottom=0.2, top = 0.9, right = 0.95, left = 0.1)
ax.set_ylabel('Total Fitness Value after 1500s')
ax.set_xlabel('')
plt.savefig("Boxplot_merged")
# plt.xlabel(None)
# plt.show()
