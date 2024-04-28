#!/usr/bin/python
import os
import subprocess
import numpy as np
from matplotlib import pyplot as plt

if __name__ == "__main__":
    output_dir = "../output"
    if(not os.path.exists(output_dir)):
        os.makedirs(output_dir, exist_ok=True)
    # 进入目录完成编译步骤
    os.chdir("../../build")
    subprocess.run(["cmake", ".."])
    subprocess.run("make")
    os.chdir("../test/scripts")

    x = np.arange(0, 10, 0.1)
    true_y = np.sin(x) + 3 * np.sin(2 * x) + 0.5 * x
    obs_y = true_y + np.random.normal(0, 1, len(x))
    obs_true_diff = np.sqrt(np.mean((true_y - obs_y)**2))

    with open("../data/sin.txt", "w") as f:
        f.write(f"{len(x)}, 1\n")
        for vx in true_y:
            f.write(f"{vx}\n")

    subprocess.run("../../build/bin/TestEnsembleSin")

    filter_value = []
    sigmas = []
    with open("../data/filter_sin.txt", "r") as f:
        for line in f:
            parts = line.strip().split(",")
            num = float(parts[0])
            sigma = float(parts[1])
            filter_value.append(num)
            sigmas.append(sigma)
    filter_value = np.array(filter_value[1:]) 
    filter_true_diff = np.sqrt(np.mean((true_y - filter_value)**2))
    fig = plt.figure(figsize=(12/2.54, 12/2.54))
    ax = fig.add_subplot(2, 1, 1)
    ax.plot(x, true_y, "b-", label="True Value", markersize=2, lw=2)
    ax.plot(x, obs_y, "ro-", label="Observed Value", markersize=3)
    ax.plot(x, filter_value, 'g--', label="Filtered Value", alpha=0.8)
    ax.legend(frameon=False, fontsize=8)
    ax.set_ylabel("y")
    ax.set_xlim(-2, 12)
    ax.set_ylim(-5, 10)
    ax.text(0.4, 0.05, "$y=x + sin(x)+ 3sin(2x)$",
            transform=ax.transAxes, fontsize=5)
    ax.text(0.7, 0.9, f"RMSE_OBS={obs_true_diff:.2f}\nRMSE_FILTER={filter_true_diff:.2f}", transform=ax.transAxes, fontsize=5)
    ax2 = fig.add_subplot(2, 1, 2)
    ax2.plot(x, sigmas[1:])
    ax2.set_ylabel("Variance")

    ax2.set_xlim(-2, 12)
    ax2.set_ylim(0, 0.3)
    ax2.set_xlabel("x")
    fig.savefig("../output/test_ensemble_kalman.png", dpi=500)