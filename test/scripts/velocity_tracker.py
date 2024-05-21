#!/usr/bin/python
import os
import subprocess
import numpy as np
from matplotlib import pyplot as plt
import pandas as pd
from plotstyles.fonts import global_fonts
from plotstyles import tools
import utils

def compile(toggle=True):
    if(toggle):
        os.chdir("../../build")
        subprocess.run(["cmake", ".."])
        subprocess.run("make")
        os.chdir("../test/scripts")
    else:
        print("Compile skipped!")

if __name__ == "__main__":
    # 进入脚本目录并创建相应的输出结果目录
    utils.cd_file_dir(__file__)
    output_dir = "../output"
    if(not os.path.exists(output_dir)):
        os.makedirs(output_dir, exist_ok=True)
    # 进入目录完成编译步骤
    utils.compile(False)

    obs_error, v_proc_error, x_proc_error = 1, 0.3, 0.01
    a = 0.2

    t = np.arange(0, 20, 0.1)
    true_v = 2 + a * t
    true_x = (2 + 0.5 * a * t) * t
    obs_x = true_x + np.random.normal(0, obs_error, len(t))


    init_X = np.array([[obs_x[0]], [2]])
    init_P = np.array([[obs_error, 0], [0, 3]])
    H = np.array([[1, 0]])
    Q = np.array([[x_proc_error], [v_proc_error]])

    df_init_x = pd.DataFrame(init_X, index=['x', 'v'])
    df_init_p = pd.DataFrame(init_P)
    df_h = pd.DataFrame(H)
    df_q = pd.DataFrame(Q, index=['x', 'v'])
    df_obs = pd.DataFrame(obs_x)
    df_r = pd.DataFrame(np.ones_like(t) * obs_error)

    with pd.ExcelWriter("../data/velocity_tracker_in.xlsx") as f:
        df_init_x.to_excel(f, sheet_name="Init_X", header=0, index=True)
        df_init_p.to_excel(f, sheet_name="Init_P", header=0, index=False)
        df_h.to_excel(f, sheet_name="H", header=0, index=False)
        df_q.to_excel(f, sheet_name="Q", header=0, index=True)
        df_obs.to_excel(f, sheet_name="Obs", header=1, index=True)
        df_r.to_excel(f, sheet_name="R", header=1, index=True)

    subprocess.run("../../build/bin/velocity_tracker")

    fig = plt.figure(figsize=tools.cm2inch([16, 12]))
    ax = fig.add_subplot(221)
    v_ax = fig.add_subplot(222)
    err_ax = fig.add_subplot(223)
    v_err_ax = fig.add_subplot(224)
    v_ax.plot(t, true_v, 'g-', label='Velocity')
    ax.plot(t, true_x, 'b-', label='True_X')
    ax.plot(t, obs_x, 'ro', markersize=3, label='Observed_X')
    # ax.set_xlim(-2, 12)
    # v_ax.set_xlim(-2, 12)
    
    df_ana = pd.read_excel("../data/velocity_tracker_out.xlsx", sheet_name="X")
    df_P = pd.read_excel("../data/velocity_tracker_out.xlsx", sheet_name="P")
    y_ana = df_ana['x'].values
    v_ana = df_ana['v'].values
    error_ana = df_P['x'].values
    v_error_ana = df_P['v'].values
    ax.plot(t, y_ana, label='Filtered_X', color='g')
    v_ax.plot(t, v_ana, label='Filtered_V')
    err_ax.plot(t, error_ana)
    v_err_ax.plot(t, v_error_ana)
    ax.legend()
    v_ax.legend()
    fig.savefig("../../doc/velocity_tracker.jpg", dpi=500)




    plt.show()