#!/usr/bin/python
import os
import subprocess
import numpy as np
from matplotlib import pyplot as plt
import pandas as pd

if __name__ == "__main__":
    output_dir = "../output"
    if(not os.path.exists(output_dir)):
        os.makedirs(output_dir, exist_ok=True)
    # 进入目录完成编译步骤
    compile = False
    if (compile == True):
        os.chdir("../../build")
        subprocess.run(["cmake", ".."])
        subprocess.run("make")
        os.chdir("../test/scripts")

    obs_error = 0.01
    x = np.arange(0, 10, 0.1)
    true_y = np.sin(x) + 3 * np.sin(2 * x) + 0.5 * x
    obs_y = true_y + np.random.normal(0, obs_error, len(x))

    init_X = np.array([0])
    init_P = np.array([[10]])
    H = np.array([[1]])
    Q = np.array([10])


    df_init_x = pd.DataFrame(init_X, index=['x'])
    df_init_p = pd.DataFrame(init_P)
    df_h = pd.DataFrame(H)
    df_q = pd.DataFrame(Q)
    df_obs = pd.DataFrame(obs_y)
    df_r = pd.DataFrame(np.ones_like(x) * obs_error)
    obs_true_diff = np.sqrt(np.mean((true_y - obs_y)**2))

    with pd.ExcelWriter("../data/TestModelIO_in.xlsx") as f:
        df_init_x.to_excel(f, sheet_name="Init_X", header=0, index=True)
        df_init_p.to_excel(f, sheet_name="Init_P", header=0, index=False)
        df_h.to_excel(f, sheet_name="H", header=0, index=False)
        df_q.to_excel(f, sheet_name="Q", header=0, index=True)
        df_obs.to_excel(f, sheet_name="Obs", header=1, index=True)
        df_r.to_excel(f, sheet_name="R", header=1, index=True)

    subprocess.run("../../build/bin/Model_Validate")

    fig = plt.figure()
    ax = fig.add_subplot(111)
    
    df_ana = pd.read_excel("../data/TestModelIO_out.xlsx", sheet_name="X")
    df_P = pd.read_excel("../data/TestModelIO_out.xlsx", sheet_name="P")
    y_ana = df_ana['x'].values
    error_ana = df_P['x'].values

    ax.plot(x, true_y, label='True')
    ax.plot(x, obs_y, 'ro-', label="Obs")
    ax.plot(x, y_ana, 'g', label='update')
    ax.plot(x, error_ana)

    # fig.savefig("../data/test.png", dpi=500)
    plt.show()
