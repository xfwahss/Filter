import pandas as pd
from plotstyles.fonts import global_fonts
from plotstyles.figure import Figure
import os
import subprocess
import numpy as np

def run_model(bin_dir, src_dir, out_dir, filename_in, filename_out, toggle=True):
    exe_path = os.path.join(os.path.abspath(bin_dir), "ModelIntegrate.exe")
    filein_path = os.path.join(os.path.abspath(src_dir), filename_in)
    fileout_path = os.path.join(os.path.abspath(out_dir), filename_out)
    if toggle:
        subprocess.run([exe_path, "-fi", filein_path, "-fo", fileout_path])
    return filein_path, fileout_path

def visualize_var(ax, filein_path, fileout_path, var_name, ylabel):
    if var_name in ["Baihe_Flow", "Chaohe_Flow", "Baihe_Cno", "Baihe_Cna",
                    "Baihe_Cnn", "Chaohe_Cno", "Chaohe_Cna", "Chaohe_Cnn"]:
        data_obs = pd.read_excel(filein_path, sheet_name="Rivers_in")
        data_obs["Baihe_Cno"] = data_obs["Baihe_Ctn"] - data_obs["Baihe_Cnn"] - data_obs["Baihe_Cna"]
        data_obs["Chaohe_Cno"] = data_obs["Chaohe_Ctn"] - data_obs["Chaohe_Cnn"] - data_obs["Chaohe_Cna"]
    elif var_name in ["Baihe_Dam_Flow", "Chaohe_Dam_Flow", "Baihe_Dam_Cna", 
                      "Baihe_Dam_Cnn", "Baihe_Dam_Cno", "Chaohe_Dam_Cna", 
                      "Chaohe_Dam_Cnn", "Chaohe_Dam_Cno"]:
        data_obs = pd.read_excel(filein_path, sheet_name="Rivers_out")
        data_obs["Baihe_Dam_Cno"] = data_obs["Baihe_Dam_Ctn"] - data_obs["Baihe_Dam_Cna"] - data_obs["Baihe_Dam_Cnn"]
        data_obs["Chaohe_Dam_Cno"] = data_obs["Chaohe_Dam_Ctn"] - data_obs["Chaohe_Dam_Cna"] - data_obs["Chaohe_Dam_Cnn"]
    elif var_name in ["Water_Level", "Res_Cno", "Res_Cna", "Res_Cnn"]:
        data_obs = pd.read_excel(filein_path, sheet_name="Reservoir")
        data_obs["Res_Ctn"] = data_obs[["Kuxi_TN", "Taoli_TN", "Neihu_TN",
                                        "Henghe_TN", "Kudong_TN", "Jingou_TN"]].mean(axis=1, skipna=True).values
        data_obs["Res_Cna"] = data_obs[["Kuxi_NHN", "Taoli_NHN", "Neihu_NHN",
                                        "Henghe_NHN", "Kudong_NHN", "Jingou_NHN"]].mean(axis=1, skipna=True).values
        data_obs["Res_Cnn"] = data_obs[["Kuxi_NON", "Taoli_NON", "Neihu_NON",
                                        "Henghe_NON", "Kudong_NON", "Jingou_NON"]].mean(axis=1, skipna=True).values
        data_obs["Res_Cno"] = data_obs["Res_Ctn"] - data_obs["Res_Cna"] - data_obs["Res_Cnn"]
    else:
        raise NotImplementedError
    data_filter = pd.read_excel(fileout_path, sheet_name="X")
    data_error = pd.read_excel(fileout_path, sheet_name="P")
    nums = pd.read_excel(filein_path, sheet_name="Params", header=0).iat[0, 1]
    x = np.arange(0, nums, 1)
    y_obs = data_obs[var_name].values[0:nums]
    y_filter = data_filter[var_name].values[0:nums]
    filtered_error = data_error[var_name].values[0:nums]
    ax.plot(x, y_obs, 'bo', label='Observation', markersize=2, lw=0.75)
    ax.plot(x, y_filter, '-r', label='Simulated', lw=0.75)
    # ax2 = ax.twinx()
    # ax2.plot(x, filtered_error, '-k', label='Error', lw=0.75)
    # ax2.set_ylim(0, 10)
    ax.legend(frameon=False, loc='upper right')
    ax.set_xlabel("时间步(日)")
    ax.set_ylabel(ylabel)


if __name__ == '__main__':
    fig = Figure(16, 11)
    ax = fig.add_axes_cm("WL", 1.8, 0.5, 6, 4, 'left upper')
    ax2 = fig.add_axes_cm("Res_Cnn", 9.2, 0.5, 6, 4, 'left upper')
    ax3 = fig.add_axes_cm("Res_Cna", 1.8, 5.7, 6, 4, 'left upper')
    ax4 = fig.add_axes_cm("Res_Cno", 9.2, 5.7, 6, 4, 'left upper')
    filein_path, fileout_path = run_model("build/bin", "test/data", 
                                          "test/output", "Miyun_Model.xlsx", 
                                          "Miyun_Model_out.xlsx", True)
    visualize_var(ax, filein_path, fileout_path, "Water_Level", '水库水位(m)')
    visualize_var(ax2, filein_path, fileout_path, "Res_Cnn", '水库硝氮(mg/L)')
    visualize_var(ax3, filein_path, fileout_path, "Res_Cna", '水库氨氮(mg/L)')
    visualize_var(ax4, filein_path, fileout_path, "Res_Cno", '水库有机氮(mg/L)')
    fig.show()