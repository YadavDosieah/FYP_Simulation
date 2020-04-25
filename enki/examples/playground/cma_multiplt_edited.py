#!/usr/bin/env python
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

    return fevals,fvalue


if __name__ == "__main__":
    feval1, Controller1 = read(sys.argv[1])
    feval2, Controller2 = read(sys.argv[2])
    feval3, Controller3 = read(sys.argv[3])
    print(sys.argv[3])

    # print((feval1.shape))
    # Max1 = [max(Controller1[i:i+20]) for i in range(0, len(Controller1), 20)]
    # Min1 = [min(Controller1[i:i+20]) for i in range(0, len(Controller1), 20)]
    # Average1 = [np.mean(Controller1[i:i+20]) for i in range(0, len(Controller1), 20)]


    plt.rcParams['font.size'] = 18
    plt.semilogy(Controller1,'r', label='Only Shepherding',marker = "x")
    # plt.semilogy(Min1,'r', label='Only Shepherding')
    # plt.semilogy(Max1,'r', label='Only Shepherding')

    plt.semilogy(Controller2,'b', label='Only Object Clustering')
    plt.semilogy(Controller3,'g', label='Shepherding + Object Clustering')
    plt.grid(True)

    plt.xlabel('Generation')
    plt.ylabel('Fitness Value')
    plt.tight_layout()
    plt.legend()

    plt.show()

    # axs.fill_between(x, min_ser, max_ser, alpha=0.2)
