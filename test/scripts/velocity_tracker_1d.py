#!/usr/bin/python
import os
import subprocess
import numpy as np
from matplotlib import pyplot as plt
import pandas as pd
from plotstyles.fonts import global_fonts
from plotstyles import tools
import utils
from plotstyles.figure import Figure

def rmse(value_model:np.array, value_obs:np.array):  
    """  
    Calculate Root Mean Squared Error (RMSE)  
      
    Parameters:  
    value_obs (np.array): Array of observed values  
    value_model (np.array): Array of simulated (predicted) values  
      
    Returns:  
    rmse (float): Root Mean Squared Error  
    """  
    # 确保 obs 和 sim 的长度相同  
    assert value_obs.shape == value_model.shape, "obs and sim must have the same shape"  
    # 计算 RMSE  
    rmse_value = np.sqrt(np.mean((value_obs - value_model) ** 2))  
    return rmse_value 

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

    obs_error, da_error, a_proc_error, v_proc_error, x_proc_error = 1, 0.01, 0.01, 0.01, 0.01
    np.random.seed(20)

    # 数据生成
    a = 0.2
    t = np.arange(0, 40, 0.1)
    true_v = 2 + a * t
    true_x = (2 + 0.5 * a * t) * t
    obs_x = true_x + np.random.normal(0, obs_error, len(t))
    calc_v = (obs_x[1:] - obs_x[0:-1])/0.1


    # 数据写入
    init_X = np.array([[obs_x[0]], 
                       [0], 
                    ])
    init_P = np.array([[2, 0], 
                       [0, 2]])
    H = np.array([[1, 0]])

    Q = np.array([[x_proc_error], 
                  [v_proc_error]])

    df_init_x = pd.DataFrame(init_X, index=['x', 'v'])
    df_init_p = pd.DataFrame(init_P)
    df_h = pd.DataFrame(H)
    df_q = pd.DataFrame(Q, index=['x', 'v'])
    df_obs = pd.DataFrame(obs_x)
    df_r = pd.DataFrame(np.ones_like(t) * 3)

    def remove_sheet(name, wb):
        if (name in wb.sheetnames):
            wb.remove(wb[name])

    with pd.ExcelWriter("../data/velocity_tracker_in_1d.xlsx", mode='a') as f:
        wb = f.book
        remove_sheet("Init_X", wb)
        remove_sheet("Init_P", wb)
        remove_sheet("H", wb)
        remove_sheet("Q", wb)
        remove_sheet("Obs", wb)
        remove_sheet("R", wb)
        df_init_x.to_excel(f, sheet_name="Init_X", header=0, index=True)
        df_init_p.to_excel(f, sheet_name="Init_P", header=0, index=False)
        df_h.to_excel(f, sheet_name="H", header=0, index=False)
        df_q.to_excel(f, sheet_name="Q", header=0, index=True)
        df_obs.to_excel(f, sheet_name="Obs", header=1, index=True)
        df_r.to_excel(f, sheet_name="R", header=1, index=True)

    subprocess.run("../../build/bin/velocity_tracker_1d")

    fig = Figure(16, 9)
    ax = fig.add_axes_cm('ax', 2, 1, 6, 4, 'left upper')
    v_ax = fig.add_axes_cm('v_ax', 9.5, 1, 6, 4, 'left upper')

    err_ax = fig.add_axes_cm('err_ax', 2, 5.5, 6, 2, 'left upper')
    v_err_ax = fig.add_axes_cm('v_err_ax', 9.5, 5.5, 6, 2, 'left upper')

    v_ax.plot(t, true_v, 'g-', label='真实值', lw=0.75)
    ax.plot(t, true_x, 'b-', label='真实值', lw=0.75)
    ax.plot(t, obs_x, 'ro', markersize=2, label='观测值', lw=0.75)
    
    # 绘图
    df_ana = pd.read_excel("../data/velocity_tracker_out_1d.xlsx", sheet_name="X")
    df_P = pd.read_excel("../data/velocity_tracker_out_1d.xlsx", sheet_name="P")
    y_ana = df_ana['x'].values
    v_ana = df_ana['v'].values

    error_v = rmse(v_ana, true_v)
    print(error_v)
    
    error_ana = df_P['x'].values
    v_error_ana = df_P['v'].values
    ax.plot(t, y_ana, label='同化值', color='g', lw=0.75)
    v_ax.plot(t, v_ana, label='同化值', color='b', lw=0.75)
    # v_ax.plot(t, df_ana['a'].values, label='a')
    # v_ax.plot(t[0:-1], calc_v, label='$Q=dV/dt$', color='r', lw=0.75, alpha=0.5)
    err_ax.plot(t, error_ana, lw=0.75, color='k')
    v_err_ax.plot(t, v_error_ana, lw=0.75, color='k')
    # v_err_ax.plot(t, df_P['a'].values)
    ax.legend(fontsize=8, frameon=False)
    v_ax.legend(fontsize=8, frameon=False)
    v_ax.text(0.7, 0.1, f'RMSE={error_v:.2f}', transform=v_ax.transAxes, fontsize=8)

    ax.set_ylim(-10, 300)
    ax.set_xlim(-5, 45)
    err_ax.set_xlim(-5, 45)
    err_ax.set_ylim(0, 1.5)
    err_ax.set_ylabel('误差$\sigma^2_x$')
    ax.set_xticks([])
    err_ax.set_yticks([0, 0.5, 1.0, 1.5])
    err_ax.set_xlabel('时间(s)')
    ax.set_ylabel('水量$(m^3)$')

    # v_ax.set_ylim(-40, 60)
    v_ax.set_xlim(-5, 45)
    v_ax.set_xticks([])
    v_ax.set_ylabel('流量差$(m^3/s)$')

    v_err_ax.set_xlim(-5, 45)
    v_err_ax.set_ylim(0, 4)
    v_err_ax.set_ylabel('误差$\sigma^2_v$')
    v_err_ax.set_xlabel('时间(s)')
    # fig.savefig("../../doc/velocity_tracker.jpg", dpi=500)

    ax.text(0.9, 0.9, '$\mathbf{(a)}$', transform=ax.transAxes)
    v_ax.text(0.9, 0.9, '$\mathbf{(b)}$', transform=v_ax.transAxes)
    err_ax.text(0.9, 0.85, '$\mathbf{(c)}$', transform=err_ax.transAxes)
    v_err_ax.text(0.9, 0.85, '$\mathbf{(d)}$', transform=v_err_ax.transAxes)

    fig.align_ylabels_coords([ax, err_ax], -0.15, 0.5)
    fig.align_ylabels_coords([v_ax, v_err_ax], -0.15, 0.5)

    mini_ax = ax.inset_axes([0.6, 0., 0.4, 0.3])
    mini_ax.plot(t, true_x, 'b-', label='真实值', lw=0.75)
    mini_ax.plot(t, obs_x, 'ro', markersize=2, label='观测值', lw=0.75)
    mini_ax.plot(t, y_ana, label='同化值', color='g', lw=0.75)
    mini_ax.set_xlim(20, 21)
    mini_ax.set_ylim(80, 85)
    mini_ax.set_xticks([])
    mini_ax.set_yticks([])



    # fig.savefig("../../doc/线性模型参数同化_1d.jpg", dpi=1000)
    fig.show()