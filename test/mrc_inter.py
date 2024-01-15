#!/bin/python3

import matplotlib.pyplot as plt
import os
import subprocess
import numpy as np
from scipy.interpolate import PchipInterpolator
from scipy.optimize import basinhopping

cwd = os.getcwd()
executable = f"{cwd}/build/bin/shards_test"
path = f"{cwd}/test/cache-trace/samples/2020Mar"
traces = ['cluster008', 'cluster006', 'cluster009']
smaxs = [128, 512, 2048, 8192, 32786]
P = 1 << 24
T = P // 1000
B = 100
R = 0.001

max_cache_size = 82000
mrcs = []
best = {'cluster008': 16600, 'cluster006': 36820, 'cluster009': 28480}

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
    #print(f"{{{','.join([f'{{{int(k)},{float(v)}}}' for [k,v] in results])}}}")
    # interpolate the miss ratio curve
    f = PchipInterpolator(results[:, 0], results[:, 1])
    mrcs.append(f)
    results = np.array([[x, f(x)] for x in np.arange(0, max_cache_size, 1)])
    plt.plot(results[:, 0], results[:, 1], label=trace),
    plt.plot(best[trace], f(best[trace]), 'x', color='black')
    plt.vlines(best[trace],
               0,
               f(best[trace]),
               color='#808080',
               linestyle='--',
               linewidth=0.5)
    plt.annotate("{:.2f}".format(f(best[trace])),
                 (best[trace] - 3500, f(best[trace]) + 0.015))
plt.xticks(list(best.values()), rotation=-30)
plt.legend()
plt.margins(0)

plt.suptitle(f'Miss Ratio Curves', y=1)
plt.title(
    f'Optimal partitioning when the cache size is {max_cache_size} (number of objects)',
    fontsize=12)
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
    #print(f"{{{','.join([f'{{{int(k)},{float(v)}}}' for [k,v] in results])}}}")
    # interpolate the miss ratio curve
    f = PchipInterpolator(results[:, 0], results[:, 1])
    results = np.array([[x, f(x)] for x in np.arange(0, max_cache_size, 1)])
    plt.plot(results[:, 0], results[:, 1], label=trace),
    plt.plot(best[trace], f(best[trace]), 'x', color='black')
    plt.vlines(best[trace],
               0,
               f(best[trace]),
               color='#808080',
               linestyle='--',
               linewidth=0.5)
    plt.annotate("{:.2f}".format(f(best[trace])),
                 (best[trace] - 3500, f(best[trace]) + 0.015))
plt.xticks(list(best.values()), rotation=-30)
plt.legend()
plt.margins(0)
plt.suptitle(f'Hit Ratio Curves', y=1)
plt.title(
    f'Optimal partitioning when the cache size is {max_cache_size} (number of objects)',
    fontsize=12)
plt.ylabel('Hit Ratio')
plt.xlabel('Cache Size (number of objects)')
plt.savefig(f'{cwd}/test/plots/hrc_example.pdf',
            bbox_inches='tight',
            format='pdf')
plt.close()

#def step(x):
#    [giver, taker] = np.random.choice(len(x), size=2, replace=False)
#    delta = np.random.random() * max(min(x[giver], max_cache_size - x[taker]),
#                                     0)
#    x[giver] -= delta
#    x[taker] += delta
#    return x
#
#
#def accept_test(f_new, x_new, f_old, x_old, **kwargs):
#    return all([x >= 0 for x in x_new]) and sum(x_new) <= max_cache_size
#
#
#def obj(x):
#    s = sum([mrc(m) for m, mrc in zip(x, mrcs)])
#    if not accept_test(None, x, None, None):
#        s = 1e9
#    print(x, s)
#    return s
#
#
#x0 = [max_cache_size // len(traces) for _ in range(len(traces))]
#
#res = basinhopping(func=obj, x0=x0, take_step=step, niter=2000, T=1000)
#
#print(res)
#
