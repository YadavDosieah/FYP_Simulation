#!/usr/bin/env python3

import subprocess
import sys, os
import csv
import numpy as np
import pandas as pd
from sh import sed
import plotly.express as px
import plotly.graph_objects as go
import math


def enumerate2(xs, start=0, step=1):
    for x in xs:
        yield (start, x)
        start += step

def read(file):
    with open(file,'r') as csvfile:
        data = csv.reader(csvfile, delimiter=',')
        data = list(data)[::2]
        data_points = [[] for _ in range(len(data))]

        for idx,row in enumerate2(data,start=0,step=2):
            data_point = row[1].split('x=')
            data_point = data_point[1].split(' ')
            data_point = list(filter(None, data_point))
            data_point = list(map(float, data_point))
            data_point = [(1 - math.exp(-x))/(1 + math.exp(-x)) for x in data_point]

            Fitness = row[1].split('f-value=')
            Fitness = Fitness[1].split(' ')
            Fitness = float(Fitness[0])
            data_point.append(Fitness)
            data_points[idx//2] = data_point

    data_points = np.array(data_points)
    num_param = data_points.shape[1]
    if(num_param == 9):
        Values = pd.DataFrame(data_points,columns=['v_l0','v_r0','v_l1','v_r1',
        'v_l2','v_r2','v_l3','v_r3','Fit_Val'])
        # print(Values)
        fig = go.Figure(data=go.Parcoords(
        line=dict(color = Values['Fit_Val'],showscale = True,colorscale=px.colors.diverging.Tealrose),
        dimensions = list([
        dict(range = [-1,1],values = Values['v_l0'],label = 'v_l0'),
        dict(range = [-1,1],values = Values['v_r0'],label = 'v_r0'),
        dict(range = [-1,1],values = Values['v_l1'],label = 'v_l1'),
        dict(range = [-1,1],values = Values['v_r1'],label = 'v_r1'),
        dict(range = [-1,1],values = Values['v_l2'],label = 'v_l2'),
        dict(range = [-1,1],values = Values['v_r2'],label = 'v_r2'),
        dict(range = [-1,1],values = Values['v_l3'],label = 'v_l3'),
        dict(range = [-1,1],values = Values['v_r3'],label = 'v_r3'),
        dict(values = Values['Fit_Val'],label = 'Fitness Value'),
        ])))
        if(("S.A") in file):
            fig.update_layout(title_text="Simplified Controller A (Evolved in Shepherding Scenario)",title_x=0.5)
            fig.write_image("SC.A.png")
        elif(("S.B") in file):
            fig.update_layout(title_text="Simplified Controller B (Evolved in Object Clustering Scenario)",title_x=0.5)
            fig.write_image("SC.B.png")
        else:
            fig.update_layout(title_text="Simplified Controller C (Evolved in Combined Scenario)",title_x=0.5)
            fig.write_image("SC.C.png")


    elif(num_param == 13):
        Values = pd.DataFrame(data_points,columns=['v_l0','v_r0','v_l1','v_r1',
        'v_l2','v_r2','v_l3','v_r3','v_l4','v_r4','v_l5','v_r5','Fit_Val'])
        # print(Values)
        fig = go.Figure(data=go.Parcoords(
        line=dict(color = Values['Fit_Val'],showscale = True,colorscale=px.colors.diverging.Tealrose),
        dimensions = list([
        dict(range = [-1,1],values = Values['v_l0'],label = 'v_l0'),
        dict(range = [-1,1],values = Values['v_r0'],label = 'v_r0'),
        dict(range = [-1,1],values = Values['v_l1'],label = 'v_l1'),
        dict(range = [-1,1],values = Values['v_r1'],label = 'v_r1'),
        dict(range = [-1,1],values = Values['v_l2'],label = 'v_l2'),
        dict(range = [-1,1],values = Values['v_r2'],label = 'v_r2'),
        dict(range = [-1,1],values = Values['v_l3'],label = 'v_l3'),
        dict(range = [-1,1],values = Values['v_r3'],label = 'v_r3'),
        dict(range = [-1,1],values = Values['v_l4'],label = 'v_l4'),
        dict(range = [-1,1],values = Values['v_r4'],label = 'v_r4'),
        dict(range = [-1,1],values = Values['v_l5'],label = 'v_l5'),
        dict(range = [-1,1],values = Values['v_r5'],label = 'v_r5'),
        dict(values = Values['Fit_Val'],label = 'Fitness Value'),
        ])))
        if(("mode0") in file):
            fig.update_layout(title_text="Controller A (Evolved in Shepherding Scenario)",title_x=0.5)
            fig.write_image("C.A.png")
        elif(("mode1") in file):
            fig.update_layout(title_text="Controller B (Evolved in Object Clustering Scenario)",title_x=0.5)
            fig.write_image("C.B.png")

    elif(num_param == 17):
        Values = pd.DataFrame(data_points,columns=['v_l0','v_r0','v_l1','v_r1',
        'v_l2','v_r2','v_l3','v_r3','v_l4','v_r4','v_l5','v_r5','v_l6','v_r6',
        'v_l7','v_r7','Fit_Val'])
        # print(Values)
        fig = go.Figure(data=go.Parcoords(
        line=dict(color = Values['Fit_Val'],showscale = True,colorscale=px.colors.diverging.Tealrose),
        dimensions = list([
        dict(range = [-1,1],values = Values['v_l0'],label = 'v_l0'),
        dict(range = [-1,1],values = Values['v_r0'],label = 'v_r0'),
        dict(range = [-1,1],values = Values['v_l1'],label = 'v_l1'),
        dict(range = [-1,1],values = Values['v_r1'],label = 'v_r1'),
        dict(range = [-1,1],values = Values['v_l2'],label = 'v_l2'),
        dict(range = [-1,1],values = Values['v_r2'],label = 'v_r2'),
        dict(range = [-1,1],values = Values['v_l3'],label = 'v_l3'),
        dict(range = [-1,1],values = Values['v_r3'],label = 'v_r3'),
        dict(range = [-1,1],values = Values['v_l4'],label = 'v_l4'),
        dict(range = [-1,1],values = Values['v_r4'],label = 'v_r4'),
        dict(range = [-1,1],values = Values['v_l5'],label = 'v_l5'),
        dict(range = [-1,1],values = Values['v_r5'],label = 'v_r5'),
        dict(range = [-1,1],values = Values['v_l6'],label = 'v_l6'),
        dict(range = [-1,1],values = Values['v_r6'],label = 'v_r6'),
        dict(range = [-1,1],values = Values['v_l7'],label = 'v_l7'),
        dict(range = [-1,1],values = Values['v_r7'],label = 'v_r7'),
        dict(values = Values['Fit_Val'],label = 'Fitness Value'),
        ])))
        fig.update_layout(title_text="Controller C (Evolved in Combined Scenario)",title_x=0.5)
        # fig.write_image("C.C.png",width=2160,height=1440,scale=5)
        fig.write_image("C.C.png")


    else:
        print("Check data")
        return
    # fig = px.parallel_coordinates(Values,color="Fit_Val",color_continuous_scale=px.colors.diverging.Tealrose)
    # fig.show()

for arg in sys.argv[1:]:
    read(arg)
