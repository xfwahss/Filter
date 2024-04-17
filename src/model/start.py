#!/usr/bin/python
import os
import subprocess
import numpy as np
import pandas as pd
from matplotlib import pyplot as plt

if __name__ == "__main__":
    input_dir = "./input"
    temp_dir = "./tmp"
    output_dir = "./output"
    if(not os.path.exists(temp_dir)):
        os.makedirs(temp_dir, exist_ok=True)
    if(not os.path.exists(output_dir)):
        os.makedirs(output_dir, exist_ok=True)
    xiahui = pd.read_excel(input_dir + "/xiahui.xlsx")
    xiahui_input = xiahui["日平均入库流量\n(立方米/秒)"]
    xiahui_output = xiahui["日平均出库流量\n(立方米/秒)"]
    with open("./tmp/chaohe.txt", 'w') as f:
        for i in xiahui_input:
            f.write(f"{i} 0 0 0\n")
    
    with open("./tmp/chaodam.txt", 'w') as f:
        for i in xiahui_output:
            f.write(f"{i} 0 0 0\n")
    zhangjiafen = pd.read_excel(input_dir + "/zhangjiafen.xlsx")
    res_wl = zhangjiafen['wl']
    zhangjiafen_input = zhangjiafen['inflow']
    zhangjiafen_output = zhangjiafen['outflow']

    with open("./tmp/baihe.txt", "w") as f:
        for i in zhangjiafen_input:
            f.write(f"{i} 0 0 0\n")

    with open("./tmp/baidam.txt", "w") as f:
        for i in zhangjiafen_output:
            f.write(f"{i} 0 0 0\n")

    ns = pd.read_excel("./input/nanshui.xlsx", sheet_name="Day")
    ns_input = ns['FLOW']
    with open("./tmp/ns.txt", "w") as f:
        for i in ns_input:
            f.write(f"{i} 0 0 0\n")
    subprocess.run("./build/bin/main")

    numbers = []
    with open("./output/res_status.txt", "r") as f:
        for line in f:
            parts = line.strip().split()
            num = float(parts[0])
            numbers.append(num)

    date_range = pd.date_range(start='2015-01-01', end='2023-04-11', freq='D')
    # print(len(date_range)) 
    fig = plt.figure(figsize=(12/2.54, 9/2.54))
    # ax = fig.add_subplot()
    ax = fig.add_axes([0.15, 0.15, 0.8, 0.8])
    ax.plot(date_range, res_wl, "bo", label="Real", markersize=1)
    ax.plot(date_range, numbers, "r", label="Model")
    res_wl = res_wl.values
    numbers = np.array(numbers)
    rrr = (numbers - res_wl)**2
    rrr[rrr > 100] = 0 # 除去实际水位的异常值
    rmse = np.sqrt(np.mean(rrr))
    ax.set_ylim(100, 160)
    ax.text(0.75, 0.1, f"RMSE={rmse:.2f}", transform=ax.transAxes)
    ax.legend(frameon=False)
    ax.set_xlabel("Time")
    ax.set_ylabel("Water Level(m)")
    fig.savefig("./output/results.png", dpi=500)