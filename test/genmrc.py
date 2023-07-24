#!/bin/python3

import matplotlib.pyplot as plt
import pandas as pd
import os
import subprocess
import numpy as np

cwd = os.getcwd()
executable=f"{cwd}/build/shards_test"
path = f"{cwd}/test/cache-trace/samples/2020Mar"
traces = ['cluster027']
smaxs = [128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32786]
T = 1000
P = 1000000
B = 100


setups = {
    "Fixed Rate": {
        "m": "fixed_rate",
        "t": T,
        "p": P,
        "b": B,
    }
}
for smax in smaxs:
    setups[f"Fixed Size ({smax})"] = {
        "m": "fixed_size",
        "t": T,
        "p": P,
        "b": B,
        "s": smax,
    }


for trace in traces:
    plt.figure(figsize=(15,10))
    for setup in setups:
        print(f"Plotting {trace} with {setup}")
        command = f'{executable} {" ".join([f"-{k} {v}" for k,v in setups[setup].items()])} -f {path}/{trace}'
        print(f"Running {command}")
        output = subprocess.run(command, shell=True, text=True, capture_output=True).stdout.split('\n')
        results = [[0,1.0]]
        for line in output:
            if line: 
                [cache_size, miss_rate] = line.split(',')
                results.append([int(cache_size), float(miss_rate)])
        results = np.array(sorted(results, key=lambda x: x[0]))
        plt.plot(results[:,0], results[:,1], label=setup, marker='o')
    plt.legend(bbox_to_anchor=(1,0.5))
    plt.xlabel("cache size")
    plt.ylabel("miss rate")
    plt.title(f"MRC for {trace}")
    plt.savefig(f"{cwd}/test/plots/{trace}.png")
    plt.close()



        








