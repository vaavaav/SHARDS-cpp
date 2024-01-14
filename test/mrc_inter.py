#!/bin/python3

import matplotlib.pyplot as plt
import os
import subprocess
import numpy as np
from scipy.interpolate import PchipInterpolator

cwd = os.getcwd()
executable = f"{cwd}/build/bin/shards_test"
path = f"{cwd}/test/cache-trace/samples/2020Mar"
traces = ['cluster003', 'cluster006', 'cluster008']
smaxs = [128, 512, 2048, 8192, 32786]
P = 1 << 24
T = P // 1000
B = 100
R = 0.001

setups = {
    f"Fixed Rate (B={B})": {
        "m": "fixed_rate",
        "t": T,
        "p": P,
        "b": B,
        "r": R,
    }
}
for smax in smaxs:
    setups[f"Fixed Size (S={smax}) (B={smax//100})"] = {
        "m": "fixed_size",
        "t": T,
        "p": P,
        "b": smax // 100,
        "s": smax,
        "r": R,
    }

plt.rcParams["font.family"] = ['NewsGotT', 'Iosevka', 'DejaVu Sans']
plt.rcParams['font.size'] = 18

for trace in traces:
    print(f"Plotting {trace}")
    command = f'{executable} -m fixed_rate -t {T} -p {P} -b {32786//100} -s {32786} -r {R} -f {path}/{trace}'
    print(f"Running {command}")
    output = subprocess.run(command,
                            shell=True,
                            text=True,
                            capture_output=True).stdout.split('\n')
    results = [[0, 1.0]]
    for line in output:
        if line:
            [cache_size, miss_rate] = line.split(',')
            results.append([int(cache_size), float(miss_rate)])
    results = np.array(sorted(results, key=lambda x: x[0]))
    print(f"{{{','.join([f'{{{int(k)},{float(v)}}}' for [k,v] in results])}}}")
    # interpolate the miss ratio curve
    results1 = PchipInterpolator(results[:, 0], results[:, 1])
    plt.plot(np.arange(0, 90000, 1),
             results1(np.arange(0, 90000, 1)),
             label=trace),
plt.legend()
plt.margins(0)
plt.title(f'Miss Ratio Curves')
plt.ylabel('Miss Ratio')
plt.xlabel('Cache Size (number of objects)')
plt.savefig(f'{cwd}/test/plots/mrc_example.pdf',
            bbox_inches='tight',
            format='pdf')
plt.close()

#=---------------

for trace in traces:
    print(f"Plotting {trace}")
    command = f'{executable} -m fixed_rate -t {T} -p {P} -b {32786//100} -s {32786} -r {R} -f {path}/{trace}'
    print(f"Running {command}")
    output = subprocess.run(command,
                            shell=True,
                            text=True,
                            capture_output=True).stdout.split('\n')
    results = [[0, 0.0]]
    for line in output:
        if line:
            [cache_size, miss_rate] = line.split(',')
            results.append([int(cache_size), 1 - float(miss_rate)])
    results = np.array(sorted(results, key=lambda x: x[0]))
    print(f"{{{','.join([f'{{{int(k)},{float(v)}}}' for [k,v] in results])}}}")
    # interpolate the miss ratio curve
    results1 = PchipInterpolator(results[:, 0], results[:, 1])
    plt.plot(np.arange(0, 90000, 1),
             results1(np.arange(0, 90000, 1)),
             label=trace),
plt.legend()
plt.margins(0)
plt.title(f'Hit Ratio Curves')
plt.ylabel('Hit Ratio')
plt.xlabel('Cache Size (number of objects)')
plt.savefig(f'{cwd}/test/plots/hrc_example.pdf',
            bbox_inches='tight',
            format='pdf')
plt.close()
