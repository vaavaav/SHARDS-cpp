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
        results = {}
        results['Fixed Size'] = np.array([[k,v] for [t,k,v] in data if t == "Fixed Size" ]).astype(float)
        results['Fixed Rate'] = np.array([[k,v] for [t,k,v] in data if t == "Fixed Rate" ]).astype(float)
        #plot dict of two different time series of two columns
        for k,v in results.items():
            v = np.array(sorted(v, key=lambda x: x[0]))
            plt.plot(v[:,0], v[:,1], label=k, marker='o')
        plt.legend()
        plt.show()
        

plot_data(sys.argv[1])
