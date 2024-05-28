#!/usr/bin/python
import os
import subprocess
import numpy as np
from matplotlib import pyplot as plt
from plotstyles.fonts import global_fonts
import pandas as pd

if __name__ == "__main__":
    data_dir = os.path.abspath("../test/data")
    output_dir = os.path.abspath("../test/output")
    exe_dir = os.path.abspath("bin")

    x = np.arange(0, 10, 0.1)
    true_y = np.sin(x) + 3 * np.sin(2 * x) + 0.5 * x
    diff_y = np.cos(x) + 3 * 2 * np.cos(2 * x) + 0.5
    obs_y = true_y + np.random.normal(0, 5, len(x))
    obs_true_diff = np.sqrt(np.mean((true_y - obs_y)**2))

    # subprocess.run("../../build/bin/TestEnsembleSin")
    writer = True
    if writer:
        data = pd.DataFrame()
        data['Z'] = obs_y
        df_writer = pd.ExcelWriter(os.path.join(data_dir, "sin.xlsx"), mode='a')
        wb = df_writer.book
        if("Z" in wb.sheetnames):
            wb.remove(wb["Z"])
        data.to_excel(df_writer, sheet_name="Z")
        df_writer.close()

    subprocess.run(os.path.join(exe_dir, "TestEnsembleSin"))

    fig = plt.figure(figsize=(4, 6))
    ax = fig.add_subplot(311)
    ax2 = fig.add_subplot(312)
    ax3 = fig.add_subplot(313)
    ax.plot(x, true_y, 'b-')
    ax.plot(x, obs_y, 'ro')
    value = pd.read_excel(os.path.join(data_dir, "filter_sin.xlsx"), sheet_name="X")
    error = pd.read_excel(os.path.join(data_dir, "filter_sin.xlsx"), sheet_name="P")
    ax.plot(x, value['x'].values, 'g--')
    ax2.plot(x, error['x'].values, 'k-')
    filter_true_diff = np.sqrt(np.mean((true_y - value['x'].values)**2))
    print(obs_true_diff)
    print(filter_true_diff)
    ax3.plot(x, diff_y)
    ax3.plot(x, value['dx'].values)

    plt.show()
