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

Fitness_Val = [[[[[[] for _ in range(5)] for _ in range(3)] for _ in range(3)] for _ in range(6)] for _ in range(5)]
maxSR = [[[[[[] for _ in range(5)] for _ in range(3)] for _ in range(3)] for _ in range(6)] for _ in range(5)]
SR = [[[[[[] for _ in range(5)] for _ in range(3)] for _ in range(3)] for _ in range(6)] for _ in range(5)]

filenames = ["Analysis_Speed0.1.csv","Analysis_Speed0.25.csv",
"Analysis_Speed0.5.csv","Analysis_Speed1.csv","Analysis_Speed2.csv"]
for i in range(5):
    mode = -1
    scenario = -1
    with open(filenames[i],'r') as csvfile:
        data = csv.reader(csvfile, delimiter=',')
        row1 = next(data)
        for row in data:
            if("mode" in row[0]):
                mode = mode + 1
                scenario = -1
                # print("mode {:d}".format(mode))
            if("Scenario" in row[0]):
                scenario = scenario + 1
                # print(scenario)
            else:
                if(scenario == 0):
                    shepherdIdx = int(row[1])//5
                    sheepIdx = int(row[2])//10
                    Fitness_Val[i][mode][scenario][shepherdIdx-1][sheepIdx-1].append(float(row[4]))
                    maxSR[i][mode][scenario][shepherdIdx-1][sheepIdx-1].append(float(row[5])*100)
                    SR[i][mode][scenario][shepherdIdx-1][sheepIdx-1].append(float(row[6])*100)

                if(scenario == 1):
                    shepherdIdx = int(row[1])//5
                    objIdx = int(row[3])//10
                    Fitness_Val[i][mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[4]))
                    maxSR[i][mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[5])*100)
                    SR[i][mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[6])*100)

                # if(scenario == 2):
                #     shepherdIdx = int(row[1])/5
                #     objIdx = int(row[3])/10
                #     Fitness_Val[mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[4]))
                #     maxSR[mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[5])*100)
                #     SR[mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[6])*100)

                if(scenario == 2):
                    shepherdIdx = int(row[1])//5
                    objIdx = int(row[3])//5
                    Fitness_Val[i][mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[4]))
                    maxSR[i][mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[5])*100)
                    SR[i][mode][scenario][shepherdIdx-1][objIdx-1].append(float(row[6])*100)

Fitness_Val = np.array(Fitness_Val)
Fitness_Val[:,:,2,:,:,:] = Fitness_Val[:,:,2,:,:,:]/2
Fitness_Val_merged = Fitness_Val.reshape(5,6,3,1500)


titles = ["Shepherding", "Obj. Clustering", "Combined Scenario"]
fname = ["Controller A", "Controller B", "Controller C", "Simplified Controller C","Simplified Controller A","Simplified Controller B"]

sns.set_palette('bright')

for x in range(6):
    Values = pandas.DataFrame()
    for i in range(3):
        Values1 = pandas.DataFrame(Fitness_Val_merged[0,x,i,:], columns=[titles[i]]).assign(Legend='Speed 0.1x')
        Values2 = pandas.DataFrame(Fitness_Val_merged[1,x,i,:], columns=[titles[i]]).assign(Legend='Speed 0.25x')
        Values3 = pandas.DataFrame(Fitness_Val_merged[2,x,i,:], columns=[titles[i]]).assign(Legend='Speed 0.5x')
        Values4 = pandas.DataFrame(Fitness_Val_merged[3,x,i,:], columns=[titles[i]]).assign(Legend='Speed 1x')
        Values5 = pandas.DataFrame(Fitness_Val_merged[4,x,i,:], columns=[titles[i]]).assign(Legend='Speed 2x')

        Values = pandas.concat([Values,Values1,Values2,Values3,Values4,Values5],sort=False)


    mdf = pandas.melt(Values, id_vars=['Legend'], var_name=['Letter'])

    plt.rcParams.update({'font.size': 22})
    # paper_rc = {'lines.linewidth': 1, 'lines.markersize': 1}
    fig, ax = plt.subplots(nrows=1, ncols=1,figsize=(12,6))
    # sns.set_style('whitegrid')
    bp = sns.boxplot(x="Letter", y="value",hue="Legend", data=mdf, width = 0.5, linewidth = 2.0)


    # num_locations = len(mdf.Letter.unique())
    # hatches = cycle(["xx",""])
    # colours = cycle(['red','red','blue','blue','green','green'])
    # for i,patch in enumerate(ax.artists):
    #     hatch = next(hatches)
    #     patch.set_hatch(hatch)
    # for i, patch in enumerate(bp.patches):
    #     hatch = next(hatches)
    #     patch.set_hatch(hatch)
    #
    #
    # for i,artist in enumerate(ax.artists):
    #     col = next(colours)
    #     # Set the linecolor on the artist to the facecolor, and set the facecolor to None
    #     # col = artist.get_facecolor()
    #     artist.set_facecolor(to_rgba(col, 0.2))
    #     artist.set_edgecolor(col)
    #
    #     # Each box has 6 associated Line2D objects (to make the whiskers, fliers, etc.)
    #     # Loop over them here, and use the same colour as above
    #     for j in range(i*6,i*6+6):
    #         line = ax.lines[j]
    #         line.set_color(col)
    #         line.set_mfc(col)
    #         line.set_mec(col)
    #
    ax.legend_.remove()
    plt.grid(axis='y')
    ax.set(yscale="log")

    fig.add_subplot(111, frameon=False)
    # hide tick and tick label of the big axis
    plt.tick_params(labelcolor='none', top=False, bottom=False, left=False, right=False)
    leg = fig.legend(loc="lower center", ncol = 6, handleheight=1.8,fontsize=12)

    # Also fix the legend
    # for legpatch in leg.get_patches():
    #     # col = legpatch.get_facecolor()
    #     col = next(colours)
    #     legpatch.set_edgecolor(col)
    #     legpatch.set_facecolor(to_rgba(col, 0.2))


    plt.subplots_adjust(bottom=0.2, top = 0.9, right = 0.95, left = 0.1)
    ax.set_title(fname[x])
    ax.set_ylabel(' Total Fitness Value after 1500s')
    ax.set_xlabel('')
    plt.savefig(fname[x]+".png")
    # plt.xlabel(None)
    # plt.show()
