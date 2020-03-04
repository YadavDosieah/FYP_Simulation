#!/usr/bin/env python

import numpy as np
import csv
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import string

noOfSheep = 0
noOfShepherd = 0

with open('Output.csv','r') as csvfile:
    data = csv.reader(csvfile, delimiter=',')
    row1 = next(data)
    for i in range(len(row1)):
        if(("SheepX") in row1[i]):
            noOfSheep = noOfSheep + 1
        elif(("ShepherdX") in row1[i]):
            noOfShepherd = noOfShepherd + 1

    SheepX = []
    SheepY = []
    ShepherdX = []
    ShepherdY = []
    i = 0
    for row in data:
        if(i%25 == 0):
            SheepX.append(row[0:noOfSheep*2:2])
            SheepY.append(row[1:noOfSheep*2:2])
            ShepherdX.append(row[noOfSheep*2:(noOfSheep+noOfShepherd)*2:2])
            ShepherdY.append(row[noOfSheep*2+1::2])

        i = i +1
fig = plt.figure()
ax = plt.axes(xlim=(0, 300), ylim=(0, 300))

sheepDots = {}
shepherdDot = {}

goal = plt.Circle((150,250),25,fc='b')
ax.add_patch(goal)
for i in range(noOfSheep):
    sheepDots[i] = plt.Circle((float(SheepX[0][i]),float(SheepY[0][i])),3.7,fc='g')

for n in range(noOfShepherd):
    shepherdDot[n] = plt.Circle((float(ShepherdX[0][n]),float(ShepherdY[0][n])),3.7,fc='r')


def init():
    for i in range(noOfSheep):
        sheepDots[i].center = (float(SheepX[0][i]),float(SheepY[0][i]))
        ax.add_patch(sheepDots[i])
    for n in range(noOfShepherd):
        shepherdDot[n].center = (float(ShepherdX[0][n]),float(ShepherdY[0][n]))
        ax.add_patch(shepherdDot[n])
    return sheepDots.values()

def animate(i):
    for j in range(noOfSheep):
        sheepDots[j].center = (float(SheepX[i][j]),float(SheepY[i][j]))
    for n in range(noOfShepherd):
        shepherdDot[n].center = (float(ShepherdX[i][n]),float(ShepherdY[i][n]))
    return sheepDots.values()

anim=animation.FuncAnimation(fig,animate,init_func=init,interval=1,frames=len(SheepX), repeat=False)

plt.show()
