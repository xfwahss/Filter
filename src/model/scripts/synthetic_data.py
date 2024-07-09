import pandas as pd
from plotstyles.fonts import global_fonts
from plotstyles import tools
from matplotlib import pyplot as plt
import os
import subprocess
import numpy as np

def run_model(base_dir, bin_dir, toggle=True):
    exe_path = os.path.join(base_dir, bin_dir, "ModelReservoir.exe")
    if toggle:
        subprocess.run([exe_path])
    return

def visualize(ax, fin, fout, var_name):
    fin_data = pd.read_excel(fin, "Res_Force")
    fout_data = pd.read_excel(fout, "Value")
    x = list(range(fout_data[var_name].size))
    ax.plot(x, fout_data[var_name])

if __name__=="__main__":
    base_dir = "D:/Gitlocal/Filter"
    os.chdir(os.path.join(base_dir, "build"))
    run_model(base_dir, "build/bin", False)

    fig = plt.figure(figsize=tools.cm2inch([18, 6]))
    ax = fig.add_subplot(121)
    wl_ax = fig.add_subplot(122)
    fin = os.path.join(base_dir, "test", "data", "Synthetic_Modeling.xlsx")
    fout = os.path.join(base_dir, "test", "output", "Synthetic_Modeling_out.xlsx")
    visualize(ax, fin, fout, "c_no")
    visualize(ax, fin, fout, "c_na")
    visualize(ax, fin, fout, "c_nn")
    visualize(wl_ax, fin, fout, "wl")
    # visualize(ax, fin, fout, "T")
    wl_ax.set_ylim(140, 150)

    plt.show()