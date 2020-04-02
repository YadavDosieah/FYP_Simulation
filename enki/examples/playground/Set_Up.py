#!/usr/bin/env python3

import numpy as np
import csv
import matplotlib.pyplot as plt
import random

noOfObjects = 7
noOfSheep = 7
noOfShepherd = 3
fig = plt.figure(figsize=(8, 10))
ax = plt.axes(xlim=(-20, 320), ylim=(-20, 400))
plt.axis('off')

sheepDots = {}
shepherdDot = {}
ObjectDot = {}

rec2 = plt.Rectangle((0,0),300,300,fill=True,edgecolor = None, color='#d3d3d3')
plt.annotate(s='', xy=(0,-10), xytext=(300,-10), arrowprops=dict(arrowstyle='<->'))
ax.annotate('3m', xy=(150,-20), ha='center', va='center', rotation=0)
plt.annotate(s='', xy=(-10,0), xytext=(-10,200), arrowprops=dict(arrowstyle='<->'))
ax.annotate('2m', xy=(-20,100), ha='center', va='center', rotation=90)
plt.annotate(s='', xy=(-10,200), xytext=(-10,300), arrowprops=dict(arrowstyle='<->'))
ax.annotate('1m', xy=(-20,250), ha='center', va='center', rotation=90)
plt.annotate(s='', xy=(310,0), xytext=(310,300), arrowprops=dict(arrowstyle='<->'))
ax.annotate('3m', xy=(320,150), ha='center', va='center', rotation=270)
ax.add_patch(rec2)

rec1 = plt.Rectangle((0,0),300,200,ls=':',fill=False,label='Initial Area of Sheep')
ax.add_patch(rec1)

plt.annotate(s='25cm', xy=(150,250), xytext=(175,267.7), arrowprops=dict(arrowstyle='<-'))
plt.annotate(s='50cm', xy=(150,250), xytext=(200,215.6), arrowprops=dict(arrowstyle='<-'))

plt.annotate(s='Goal Area', xy=(150,290), xytext=(190,315), arrowprops=dict(arrowstyle='<-',color='b'),color='b')
plt.annotate(s='Initial Area for Sheep, Shepherds and Objects', xy=(10,190), xytext=(10,345), arrowprops=dict(arrowstyle='<-',color='g'),color='g')
goal = plt.Circle((150,250),25,fc='b')
goal2 = plt.Circle((150,250),50,ls=':',fill=False)
ax.add_patch(goal)
ax.add_patch(goal2)

for i in range(noOfSheep):
    sheepDots[i] = plt.Circle((random.randint(0,300), random.randint(0,200)),3.7,fc='g')
    ax.add_patch(sheepDots[i])

for n in range(noOfShepherd):
    shepherdDot[n] = plt.Circle((random.randint(0,300),random.randint(0,200)),3.7,fc='r')
    ax.add_patch(shepherdDot[n])

for m in range(noOfObjects):
    ObjectDot[m] = plt.Circle((random.randint(0,300),random.randint(0,200)),3.7,fc='gold')
    ax.add_patch(ObjectDot[m])

legendbox = plt.Rectangle((240,320),60,60,fill=False,edgecolor='k')
legend1 = plt.Circle((250, 350),3.7,fc='g')
legend2 = plt.Circle((250, 370),3.7,fc='r')
legend3 = plt.Circle((250, 330),3.7,fc='gold')
ax.add_patch(legend1)
ax.add_patch(legend2)
ax.add_patch(legend3)
ax.add_patch(legendbox)
ax.annotate('Legend', xy=(270,390), ha='center', va='center', rotation=0)
ax.annotate('Sheep', xy=(260,350), ha='left', va='center', rotation=0)
ax.annotate('Shepherd', xy=(260,370), ha='left', va='center', rotation=0)
ax.annotate('Object', xy=(260,330), ha='left', va='center', rotation=0)
#
# def init():
#     for i in range(noOfSheep):
#         sheepDots[i].center = (float(SheepX[0][i]),float(SheepY[0][i]))
#         ax.add_patch(sheepDots[i])
#     for n in range(noOfShepherd):
#         shepherdDot[n].center = (float(ShepherdX[0][n]),float(ShepherdY[0][n]))
#         ax.add_patch(shepherdDot[n])
#     return sheepDots.values()
#
# def animate(i):
#     for j in range(noOfSheep):
#         sheepDots[j].center = (float(SheepX[i][j]),float(SheepY[i][j]))
#     for n in range(noOfShepherd):
#         shepherdDot[n].center = (float(ShepherdX[i][n]),float(ShepherdY[i][n]))
#     return sheepDots.values()
#
# anim=animation.FuncAnimation(fig,animate,init_func=init,interval=1,frames=len(SheepX), repeat=False)

plt.show()
