#!/usr/bin/env python3
"""In a OS shell::

    python cma_multiplt.py data_file_name

or in a python shell::

    import cma_multiplt as lcmaplt
    lcmaplt.plot(data_file_name)

"""
# CMA-ES, Covariance Matrix Adaptation Evolution Strategy
# Copyright (c) 2014 Inria
# Author: Emmanuel Benazera <emmanuel.benazera@lri.fr>
#
# This file is part of libcmaes.
#
# libcmaes is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# libcmaes is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with libcmaes.  If not, see <http://www.gnu.org/licenses/>.
##

import sys, pylab, csv
import numpy as np
from matplotlib.pylab import subplot, semilogy, grid, title
import matplotlib.pyplot as plt
import math
# from matplotlib.pylab import figure, subplot, semilogy, hold, grid, axis, title, text, xlabel, isinteractive, draw, gcf
# TODO: the above direct imports clutter the interface in a Python shell

# number of static variables at the head of every line (i.e. independent of problem dimension)
single_values = 4

def read(filename):
    # read data into numpy array
    dat = np.loadtxt(filename,dtype=float)

    dim = int(np.ceil(np.shape(dat)[1] - single_values) / 3) # we estimate the problem dimension from the data
    #print dim

    fvalue = np.absolute(dat[:,0])
    fevals = dat[:,1]
    sigma = dat[:,2]
    kappa = dat[:,3]
    if dim > 0:
        eigenvc = []
        for c in range(single_values,single_values+dim):
            eigenvc.append(c)
        eigenv = dat[:,eigenvc]
        stdsc = []
        for c in range(single_values+dim,single_values+2*dim):
            stdsc.append(c)
        stds = dat[:,stdsc]
        minstds = np.amin(stds,axis=1)
        maxstds = np.amax(stds,axis=1)
        xmeanc = []
        for c in range(single_values+2*dim,single_values+3*dim):
            xmeanc.append(c)
        xmean = dat[:,xmeanc]
        xmean = (1 - np.exp(-xmean))/(1 + np.exp(-xmean))
    print(xmean.shape)
    # for
    return fevals,xmean


if __name__ == "__main__":
    feval1, Controller1 = read(sys.argv[1])
    feval2, Controller2 = read(sys.argv[2])
    feval3, Controller3 = read(sys.argv[3])
    feval4, Controller4 = read(sys.argv[4])
    nVar1 = Controller1.shape[1]
    nVar2 = Controller2.shape[1]
    nVar3 = Controller3.shape[1]
    nVar4 = Controller4.shape[1]

    #####################################################################
    ############################# Controller A
    #####################################################################
    plt.rcParams['font.size'] = 18
    fig, ax = plt.subplots(nrows=1, ncols=1,figsize=(12,6))#14.2 x 8
    ax.plot(Controller1)
    ax.grid(True)
    ax.set_ylim(-1.05, 1.05)
    ax.set_xlim(0, 80)
    plt.xlabel('Generation')
    plt.ylabel('Normalised Wheel Velocities')

    fig.add_subplot(111, frameon=False)
    # hide tick and tick label of the big axis
    plt.tick_params(labelcolor='none', top=False, bottom=False, left=False, right=False)
    fig.legend(['v_l_0','v_r_0','v_l_1','v_r_1','v_l_3','v_r_3',
    'v_l_4','v_r_4','v_l_5','v_r_5','v_l_7','v_r_7'],loc="lower center", ncol = nVar1/2, handlelength=1, handleheight=1.8,fontsize=16)

    plt.subplots_adjust(bottom=0.28, top = 0.97, right = 0.95, left = 0.1)
    plt.savefig("Controller A - Wheel Vel")

    #####################################################################
    ############################# Controller B
    #####################################################################
    fig, ax = plt.subplots(nrows=1, ncols=1,figsize=(12,6))#14.2 x 8
    ax.plot(Controller2)
    ax.grid(True)
    ax.set_ylim(-1.05, 1.05)
    ax.set_xlim(0, 80)
    plt.xlabel('Generation')
    plt.ylabel('Normalised Wheel Velocities')

    fig.add_subplot(111, frameon=False)
    # hide tick and tick label of the big axis
    plt.tick_params(labelcolor='none', top=False, bottom=False, left=False, right=False)
    fig.legend(['v_l_0','v_r_0','v_l_2','v_r_2','v_l_3','v_r_3',
    'v_l_4','v_r_4','v_l_6','v_r_6','v_l_7','v_r_7'],loc="lower center", ncol = nVar2/2, handlelength=1, handleheight=1.8,fontsize=16)

    plt.subplots_adjust(bottom=0.28, top = 0.97, right = 0.95, left = 0.1)
    plt.savefig("Controller B - Wheel Vel")

    #####################################################################
    ############################# Controller C
    #####################################################################
    fig, ax = plt.subplots(nrows=1, ncols=1,figsize=(12,6))#14.2 x 8
    ax.plot(Controller3)
    ax.grid(True)
    ax.set_ylim(-1.05, 1.05)
    ax.set_xlim(0, 80)
    plt.xlabel('Generation')
    plt.ylabel('Normalised Wheel Velocities')

    fig.add_subplot(111, frameon=False)
    # hide tick and tick label of the big axis
    plt.tick_params(labelcolor='none', top=False, bottom=False, left=False, right=False)
    fig.legend(['v_l_0','v_r_0','v_l_1','v_r_1','v_l_2','v_r_2','v_l_3','v_r_3',
    'v_l_4','v_r_4','v_l_5','v_r_5','v_l_6','v_r_6','v_l_7','v_r_7'],loc="lower center", ncol = nVar3/2, handlelength=1, handleheight=1.8,fontsize=16)

    plt.subplots_adjust(bottom=0.28, top = 0.97, right = 0.95, left = 0.1)
    plt.savefig("Controller C - Wheel Vel")

    #####################################################################
    ############################# Controller D
    #####################################################################
    fig, ax = plt.subplots(nrows=1, ncols=1,figsize=(12,6))#14.2 x 8
    ax.plot(Controller4)
    ax.grid(True)
    ax.set_ylim(-1.05, 1.05)
    ax.set_xlim(0, 80)
    plt.xlabel('Generation')
    plt.ylabel('Normalised Wheel Velocities')

    fig.add_subplot(111, frameon=False)
    # hide tick and tick label of the big axis
    plt.tick_params(labelcolor='none', top=False, bottom=False, left=False, right=False)
    fig.legend(['v_l_0','v_r_0','v_l_1','v_r_1','v_l_2','v_r_2','v_l_3','v_r_3',
    'v_l_4','v_r_4','v_l_5','v_r_5','v_l_6','v_r_6','v_l_7','v_r_7'],loc="lower center", ncol = nVar4/2, handlelength=1, handleheight=1.8,fontsize=16)

    plt.subplots_adjust(bottom=0.28, top = 0.97, right = 0.95, left = 0.1)
    plt.savefig("Simplified Controller - Wheel Vel")

    # plt.show()
