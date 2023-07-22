#!/bin/python3

import csv
import matplotlib.pyplot as plt
import numpy as np
import sys

# read csv and plot data
def plot_data(filename):
    with open(filename, 'r') as csvfile:
        reader = csv.reader(csvfile, delimiter=',')
        data = list(reader)
        data = np.array(data).astype(float)
        plt.plot(data[:,0], data[:,1], 'ro')
        plt.show()

plot_data(sys.argv[1])
