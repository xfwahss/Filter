#!/usr/bin/python
import os
import subprocess
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
    print("Here") 
    subprocess.run("./build/bin/main")

    numbers = []
    with open("./output/res_status.txt", "r") as f:
        for line in f:
            parts = line.strip().split()
            num = float(parts[0])
            numbers.append(num)
    
    plt.plot(res_wl, "bo", label="Real", markersize=1)
    plt.plot(numbers, "r", label="Model")
    plt.ylim(120, 170)
    plt.legend(frameon=False)
    plt.savefig("./output/results.png", dpi=500)