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
    ShepherdX = []
    ShepherdY = []

    CylinderX = []
    CylinderY = []
    i = 0
    skip = 100
    for row in data:
        if(i%skip == 0):
            #Start:End:Step
            SheepX.append(row[0:noOfSheep*3:3])
            SheepY.append(row[1:noOfSheep*3:3])
            ShepherdX.append(row[noOfSheep*3+noOfObjects*2:(noOfSheep+noOfShepherd)*3+noOfObjects*2:3])
            ShepherdY.append(row[noOfSheep*3+noOfObjects*2+1::3])
            CylinderX.append(row[noOfSheep*3:noOfSheep*3+noOfObjects*2:2])
            CylinderY.append(row[noOfSheep*3+1:noOfSheep*3+noOfObjects*2:2])
        i = i +1

SheepX = [list( map(float,i) ) for i in SheepX]
SheepY = [list( map(float,i) ) for i in SheepY]
ShepherdX = [list( map(float,i) ) for i in ShepherdX]
ShepherdY = [list( map(float,i) ) for i in ShepherdY]
CylinderX = [list( map(float,i) ) for i in CylinderX]
CylinderY = [list( map(float,i) ) for i in CylinderY]

SheepSpeed = [[] for _ in range(noOfSheep)]
ShepherdSpeed = [[] for _ in range(noOfShepherd)]
ObjectSpeed = [[] for _ in range(noOfObjects)]
for j in range(noOfSheep):
    for i in range(len(SheepX)):
        if i == 0:
            SheepSpeed[j].append(0)
        else:
            SheepSpeed[j].append(((SheepX[i][j]-SheepX[i-1][j])**2 + (SheepY[i][j]-SheepY[i-1][j])**2)**0.5)

for j in range(noOfShepherd):
    for i in range(len(ShepherdX)):
        if i == 0:
            ShepherdSpeed[j].append(0)
        else:
            ShepherdSpeed[j].append(((ShepherdX[i][j]-ShepherdX[i-1][j])**2 + (ShepherdY[i][j]-ShepherdY[i-1][j])**2)**0.5)

for j in range(noOfObjects):
    for i in range(len(CylinderX)):
        if i == 0:
            ObjectSpeed[j].append(0)
        else:
            ObjectSpeed[j].append(((CylinderX[i][j]-CylinderX[i-1][j])**2 + (CylinderY[i][j]-CylinderY[i-1][j])**2)**0.5)

SheepSpeed_avg = np.array(SheepSpeed)
SheepSpeed_avg = np.mean(SheepSpeed_avg, axis=0)
SheepSpeed_avg = SheepSpeed_avg/(skip/10)

ShepherdSpeed_avg = np.array(ShepherdSpeed)
ShepherdSpeed_avg = np.mean(ShepherdSpeed_avg, axis=0)
ShepherdSpeed_avg = ShepherdSpeed_avg/(skip/10)

ObjectSpeed_avg = np.array(ObjectSpeed)
ObjectSpeed_avg = np.mean(ObjectSpeed_avg, axis=0)
ObjectSpeed_avg = ObjectSpeed_avg/(skip/10)



fig = plt.figure()
ax = plt.axes()

x = np.linspace(0, 1500, len(ShepherdX))
# for i in range(noOfSheep):
#     ax.plot(x, SheepSpeed[i],'-g')
#
# for i in range(noOfObjects):
#     ax.plot(x, ObjectSpeed[i],'--c')
#
# for i in range(noOfShepherd):
#     ax.plot(x, ShepherdSpeed[i],':r')

ax.plot(x, SheepSpeed_avg,'-g', label='Average Sheep Speed')
ax.plot(x, ObjectSpeed_avg,'--c', label='Average Object Speed')
ax.plot(x, ShepherdSpeed_avg,':r', label='Average Shepherd Speed')

plt.xlabel('time(s)')
plt.ylabel('Speed(cm/s)')
plt.title('Evolution of agents\' speed with time')
plt.grid()
plt.xlim(left=0,right=1500)
plt.ylim(bottom=0, top=12.8)
plt.legend(loc="upper right")

plt.savefig("Velocity Plot.png")
# plt.show()
