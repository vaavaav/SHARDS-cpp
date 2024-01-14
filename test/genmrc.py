#!/bin/python3

import matplotlib.pyplot as plt
import pandas as pd
import os
import subprocess
import numpy as np
from scipy.interpolate import PchipInterpolator

cwd = os.getcwd()
executable = f"{cwd}/build/bin/shards_test"
path = f"{cwd}/test/cache-trace/samples/2020Mar"
traces = sorted(os.listdir(path))
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

plt.figure(figsize=(10, 5))
plt.rcParams["font.family"] = ['NewsGotT', 'Iosevka', 'DejaVu Sans']
plt.rcParams['font.size'] = 14

for trace in traces:
    plt.figure(figsize=(15, 10))
    for setup in setups:
        print(f"Plotting {trace} with {setup}")
        command = f'{executable} {" ".join([f"-{k} {v}" for k,v in setups[setup].items()])} -f {path}/{trace}'
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
        print(
            f"{{{','.join([f'{{{int(k)},{float(v)}}}' for [k,v] in results])}}}"
        )
        plt.plot(results[:, 0], results[:, 1], label=setup, marker='o')
    plt.legend()
    plt.margins(0)
    plt.title(f'Miss Ratio Curve for Trace-{trace}')
    plt.ylabel('Miss Ratio')
    plt.xlabel('Cache Size (in objects)')
    plt.savefig(f'{cwd}/test/plots/{trace}.pdf',
                bbox_inches='tight',
                format='pdf')
    plt.close()
