#!/usr/bin/python
import os
import subprocess
import numpy as np
from matplotlib import pyplot as plt

if __name__ == "__main__":


    output_dir = "../output"
    if(not os.path.exists(output_dir)):
        os.makedirs(output_dir, exist_ok=True)

    x = np.arange(0, 10, 0.05)
    true_y = np.sin(x) + 1
    obs_y = true_y + np.random.normal(0, 0.25, len(x))

    with open("../data/test_kalman_filter_and_fileio.txt", "w") as f:
        f.write(f"{len(x)}, 1\n")
        for vx in true_y:
            f.write(f"{vx}\n")

    subprocess.run("../../build/bin/./TestKalmanFilterWithFileIO")

    filter_value = []
    with open("../data/kalman_and_fileio_results.txt", "r") as f:
        for line in f:
            parts = line.strip().split(",")
            print(parts)
            num = float(parts[0])
            filter_value.append(num)
    fig = plt.figure(figsize=(12/2.54, 9/2.54))
    # ax = fig.add_subplot()
    ax = fig.add_axes([0.15, 0.15, 0.8, 0.8])
    ax.plot(x, true_y, "b", label="Real", markersize=0.5)
    ax.plot(x, obs_y, "ro-", label="Obs")
    ax.plot(x, filter_value[1:], 'g-', label="Filter")
    ax.legend(frameon=False)
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    fig.savefig("../output/kalman_filter_results.png", dpi=500)