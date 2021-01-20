#!/usr/bin/env python3

import numpy as np
import csv
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import PIL
import string
import math
import io, libconf
import os
import sys

noOfSheep = 0
noOfShepherd = 0
noOfObjects = 0

with io.open('Parameters.cfg') as f:
    config = libconf.load(f)

Goalx = config.Goalx
Goaly = config.Goaly
GoalRadius = config.GoalRadius
GoalDistance = config.GoalDistance
Xbound = config.Xbound
Ybound = config.Ybound

f = open("Output.csv")
numlines = len(f.readlines())

with open('Output.csv','r') as csvfile:
    data = csv.reader(csvfile, delimiter=',')
    row1 = next(data)
    for i in range(len(row1)):
        if(("SheepX") in row1[i]):
            noOfSheep = noOfSheep + 1
        elif(("ShepherdX") in row1[i]):
            noOfShepherd = noOfShepherd + 1
        elif(("CylinderX") in row1[i]):
            noOfObjects = noOfObjects + 1

    SheepX = []
    SheepY = []
    SheepAngle = []
    ShepherdX = []
    ShepherdY = []
    ShepherdAngle =[]

    CylinderX = []
    CylinderY = []
    i = 0
    for row in data:
        if(i%250 == 0 or i == numlines-2):
            #Start:End:Step
            SheepX.append(row[0:noOfSheep*3:3])
            SheepY.append(row[1:noOfSheep*3:3])
            SheepAngle.append(row[2:noOfSheep*3:3])
            ShepherdX.append(row[noOfSheep*3+noOfObjects*2:(noOfSheep+noOfShepherd)*3+noOfObjects*2:3])
            ShepherdY.append(row[noOfSheep*3+noOfObjects*2+1::3])
            ShepherdAngle.append(row[noOfSheep*3+noOfObjects*2+2::3])
            CylinderX.append(row[noOfSheep*3:noOfSheep*3+noOfObjects*2:2])
            CylinderY.append(row[noOfSheep*3+1:noOfSheep*3+noOfObjects*2:2])
        i = i +1

fig = plt.figure()
ax = plt.axes(xlim=(0, Xbound), ylim=(0, Ybound))
rec = plt.Rectangle((0,0),Xbound,Ybound,fill=True,edgecolor = None, color='#d3d3d3')
ax.add_patch(rec)

sheepDots = {}
shepherdDot = {}
ObjectDot = {}
sheepLines = {}
shepherdLines = {}

goal = plt.Circle((Goalx,Goaly),GoalRadius,fc='b')
goalArea = plt.Circle((Goalx,Goaly),GoalDistance,fc='b',alpha=0.3)
ax.add_patch(goal)
ax.add_patch(goalArea)
# ax.plot([0,600], [100,100], color = 'k', linewidth = 1)[0]
# ax.plot([0,600], [500,500], color = 'k', linewidth = 1)[0]
# ax.plot([500,500], [0,600], color = 'k', linewidth = 1)[0]
# ax.plot([100,100], [0,600], color = 'k', linewidth = 1)[0]
for i in range(noOfSheep):
    sheepDots[i] = plt.Circle((float(SheepX[0][i]),float(SheepY[0][i])),3.7,fc='g')
    sheepLines[i] = ax.plot([], [], color = 'w', linewidth = 1)[0]

for n in range(noOfShepherd):
    shepherdDot[n] = plt.Circle((float(ShepherdX[0][n]),float(ShepherdY[0][n])),3.7,fc='r')
    shepherdLines[n] = ax.plot([], [], color = 'w', linewidth = 1)[0]

for k in range(noOfObjects):
    ObjectDot[k] = plt.Circle((float(CylinderX[0][k]),float(CylinderY[0][k])),3.7,fc='y')


def init():
    for i in range(noOfSheep):
        sheepDots[i].center = (float(SheepX[0][i]),float(SheepY[0][i]))
        endy = 3.5 * math.sin(float(SheepAngle[0][i])) + float(SheepY[0][i])
        endx = 3.5 * math.cos(float(SheepAngle[0][i])) + float(SheepX[0][i])
        sheepLines[i].set_data([float(SheepX[0][i]), endx], [float(SheepY[0][i]), endy])
        ax.add_patch(sheepDots[i])

    for n in range(noOfShepherd):
        shepherdDot[n].center = (float(ShepherdX[0][n]),float(ShepherdY[0][n]))
        endy = 3.5 * math.sin(float(ShepherdAngle[0][n])) + float(ShepherdY[0][n])
        endx = 3.5 * math.cos(float(ShepherdAngle[0][n])) + float(ShepherdX[0][n])
        shepherdLines[n].set_data([float(ShepherdX[0][n]), endx], [float(ShepherdY[0][n]), endy])
        ax.add_patch(shepherdDot[n])

    for k in range(noOfObjects):
        ObjectDot[k].center = (float(CylinderX[0][k]),float(CylinderY[0][k]))
        ax.add_patch(ObjectDot[k])
    return sheepLines

def animate(i):
    for j in range(noOfSheep):
        sheepDots[j].center = (float(SheepX[i][j]),float(SheepY[i][j]))
        endy = 3.5 * math.sin(float(SheepAngle[i][j])) + float(SheepY[i][j])
        endx = 3.5 * math.cos(float(SheepAngle[i][j])) + float(SheepX[i][j])
        sheepLines[j].set_data([float(SheepX[i][j]), endx], [float(SheepY[i][j]), endy])

    for n in range(noOfShepherd):
        shepherdDot[n].center = (float(ShepherdX[i][n]),float(ShepherdY[i][n]))
        endy = 3.5 * math.sin(float(ShepherdAngle[i][n])) + float(ShepherdY[i][n])
        endx = 3.5 * math.cos(float(ShepherdAngle[i][n])) + float(ShepherdX[i][n])
        shepherdLines[n].set_data([float(ShepherdX[i][n]), endx], [float(ShepherdY[i][n]), endy])

    for k in range(noOfObjects):
        ObjectDot[k].center = (float(CylinderX[i][k]),float(CylinderY[i][k]))
    return sheepLines

anim=animation.FuncAnimation(fig,animate,init_func=init,interval=1,frames=len(ShepherdY), repeat=False)
ax.set_aspect('equal', 'box')
fig.tight_layout()
# Set up formatting for the movie files

if len(sys.argv) > 1:
    try:
        # Create target Directory
        os.mkdir(sys.argv[1])
    except:
        pass
    anim.save(os.path.join(sys.argv[1], 'anim.png'), writer="imagemagick")
else:
    pass


################### Mp4 animation
with open('Output.csv','r') as csvfile:
    data = csv.reader(csvfile, delimiter=',')
    row1 = next(data)

    SheepX = []
    SheepY = []
    SheepAngle = []
    ShepherdX = []
    ShepherdY = []
    ShepherdAngle =[]

    CylinderX = []
    CylinderY = []
    i = 0
    for row in data:
        if(i%25 == 0 or i == numlines-2):
            #Start:End:Step
            SheepX.append(row[0:noOfSheep*3:3])
            SheepY.append(row[1:noOfSheep*3:3])
            SheepAngle.append(row[2:noOfSheep*3:3])
            ShepherdX.append(row[noOfSheep*3+noOfObjects*2:(noOfSheep+noOfShepherd)*3+noOfObjects*2:3])
            ShepherdY.append(row[noOfSheep*3+noOfObjects*2+1::3])
            ShepherdAngle.append(row[noOfSheep*3+noOfObjects*2+2::3])
            CylinderX.append(row[noOfSheep*3:noOfSheep*3+noOfObjects*2:2])
            CylinderY.append(row[noOfSheep*3+1:noOfSheep*3+noOfObjects*2:2])
        i = i +1

anim=animation.FuncAnimation(fig,animate,init_func=init,interval=1,frames=len(ShepherdY), repeat=False)
ax.set_aspect('equal', 'box')
fig.tight_layout()

if len(sys.argv) > 1:
    try:
        # Create target Directory
        os.mkdir(sys.argv[1])
    except:
        pass
    anim.save(os.path.join(sys.argv[1], 'sim.mp4'),fps=20)
else:
    plt.show()
