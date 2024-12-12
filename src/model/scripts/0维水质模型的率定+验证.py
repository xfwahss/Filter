import sys
sys.path.append("D:/Gitlocal/plotstyles")
from plotstyles import styles
from plotstyles import figure
import pandas as pd
import subprocess
import time
from functools import wraps
from datetime import datetime
from matplotlib.dates import DateFormatter, MonthLocator, DayLocator
import numpy as np

styles.use(["wr"])
calibrate_toggle = False
validate_toggle = False
params_dict = {
    #  有机氮参数
    "--b_ndo_flux": 0.040,
    "--s_nrp": 0.06,
    "--s_nlp": 0.05,
    "--alpha_rpon": 0.50,
    "--alpha_lpon": 0.25,
    "--k_rpon": 0.001,
    "--theta_rpon": 1.05,
    "--k_lpon": 0.014,
    "--theta_lpon": 1.040,
    "--k_don": 0.059,
    "--theta_don": 1.042,
    # 硝化过程参数
    "--b_amm_flux": 0.005,
    "--rnit0": 0.0026,
    "--knit20": 0.038,
    "--foxmin": 0.1,
    "--c_oxc_nit": 4.5,
    "--c_oxo_nit": 15.0,
    "--theta_nit": 1.018,
    "--T_c_nit": 4.5,
    "--alpha": 0.0,
    # 反硝化过程参数
    "--b_nit_flux": 0.00,
    "--rdeni0": 0.0033,
    "--kdeni20": 0.021,
    "--Tc_deni": -2,
    "--theta_deni": 1.11,
    "--c_oxc_deni": 10,
    "--c_oxo_deni": 3,
    "--beta": 1,
    # 其他参数
    "--h0": 122,
}


def timer(func):
    @wraps(func)
    def wrapper(*args, **kwargs):
        start_time = time.time()
        result = func(*args, **kwargs)
        end_time = time.time()
        print(
            f"Function {func.__name__} took {end_time - start_time:.6f} seconds to execute."
        )
        return result

    return wrapper


@timer
def run_model(exe_path, filein, fileout, params: dict, id=0, toggle=True):
    if toggle:
        exe_params = [exe_path, "-i", filein, "-o", fileout, "--id", f"{id}"]
        for key, value in params.items():
            exe_params.append(key)
            exe_params.append(f"{value}")
        subprocess.run(exe_params)
    else:
        return


def skip_na(obs, simu):
    comb = np.hstack([obs.reshape(-1, 1), simu.reshape(-1, 1)])
    comb_skipna = comb[~np.isnan(comb).any(axis=1)]
    return comb_skipna[:, 0], comb_skipna[:, 1]

def r2(obs, simu):
    obs, simu = skip_na(obs, simu)
    return np.corrcoef(obs, simu)[0, 1]

def nse(obs, simu):
    obs, simu = skip_na(obs, simu)
    obs_mean = np.mean(obs)
    # 计算残差平方和（RSS）
    residuals = simu - obs
    rss = np.sum(residuals**2)
    # 计算观测值与观测值均值之差的平方和（TSS）
    tss = np.sum((obs - obs_mean) ** 2)
    # 防止除以零（如果 TSS 为零，则 NSE 未定义）
    if tss == 0:
        return float("nan")  # 返回 NaN 表示未定义
    # 计算 NSE
    nse_val = 1 - (rss / tss)
    return nse_val
def rmse(obs, simu):
    obs, simu = skip_na(obs, simu)
    differences = simu - obs
    differences_squared = differences ** 2
    mean_of_differences_squared = differences_squared.mean()
    rmse_val = np.sqrt(mean_of_differences_squared)
    return rmse_val

def mape(obs, simu):
    obs, simu = skip_na(obs, simu)
    mape = np.mean(np.abs((obs - simu) / obs)) * 100  
    return mape  

# water research 的双栏排版19cm，1.5是 14cm， 单栏排版 9 cm
# 正文字号最小7pt
def create_fig(
    width=15, left_bound=2.0, top_bound=1, hspace=0.30, wspace=0.2, bottom_bound=1.2
):
    ax_width, ax_height = 4, 2.04
    loc_x1, loc_x2 = left_bound, left_bound + ax_width + hspace
    loc_y1 = top_bound
    loc_y2 = loc_y1 + ax_height + wspace
    loc_y3 = loc_y2 + ax_height + wspace
    loc_y4 = loc_y3 + ax_height + wspace
    loc_y5 = loc_y4 + ax_height + wspace
    height = loc_y5 + ax_height + bottom_bound
    rlength, rhspace, rwspace = 2, 1.7, 1
    r_x = rhspace + loc_x2 + ax_width
    r_y2 = loc_y1 + rlength + rwspace
    r_y3 = r_y2 + rlength + rwspace
    r_y4 = r_y3 + rlength + rwspace
    fig = figure.Figure(width=width, height=height)
    # Factor，Calibrate和Validate
    fig.add_axes_cm("FACT", loc_x1, loc_y1, ax_width, ax_height, "left upper")
    fig.add_axes_cm(
        "FACD",
        loc_x1,
        loc_y1,
        ax_width,
        ax_height,
        "left upper",
        sharex=fig.axes_dict["FACT"],
    )
    fig.axes_dict["FACD"].set_facecolor("none")
    fig.axes_dict["FACT"].spines["left"].set_position(("axes", -0.14))
    fig.add_axes_cm("FAVT", loc_x2, loc_y1, ax_width, ax_height, "left upper")
    fig.add_axes_cm(
        "FAVD",
        loc_x2,
        loc_y1,
        ax_width,
        ax_height,
        "left upper",
        sharex=fig.axes_dict["FAVT"],
    )
    fig.axes_dict["FAVD"].set_facecolor("none")
    fig.add_axes_cm("WLC", loc_x1, loc_y2, ax_width, ax_height, "left upper")
    fig.add_axes_cm("WLV", loc_x2, loc_y2, ax_width, ax_height, "left upper")
    fig.add_axes_cm("ONC", loc_x1, loc_y3, ax_width, ax_height, "left upper")
    fig.add_axes_cm("ONV", loc_x2, loc_y3, ax_width, ax_height, "left upper")
    fig.add_axes_cm("ANC", loc_x1, loc_y4, ax_width, ax_height, "left upper")
    fig.add_axes_cm("ANV", loc_x2, loc_y4, ax_width, ax_height, "left upper")
    fig.add_axes_cm("NNC", loc_x1, loc_y5, ax_width, ax_height, "left upper")
    fig.add_axes_cm("NNV", loc_x2, loc_y5, ax_width, ax_height, "left upper")
    fig.add_axes_cm("RWL", r_x, loc_y1, rlength, rlength, "left upper")
    fig.add_axes_cm("RON", r_x, r_y2, rlength, rlength, "left upper")
    fig.add_axes_cm("RAN", r_x, r_y3, rlength, rlength, "left upper")
    fig.add_axes_cm("RNN", r_x, r_y4, rlength, rlength, "left upper")
    fig.axes_dict["FACD"].set_ylabel("Environmental\nfactors")
    fig.axes_dict["WLC"].set_ylabel("Water level\n$(m)$")
    fig.axes_dict["ONC"].set_ylabel("Organic nitrogen\n$(mg/L)$")
    fig.axes_dict["ANC"].set_ylabel("Ammonia nitrogen\n$(mg/L)$")
    fig.axes_dict["NNC"].set_ylabel("Nitrate nitrogen\n$(mg/L)$")
    fig.axes_dict["RWL"].set_xlabel("Observed water level $(m)$")
    fig.axes_dict["RWL"].set_ylabel("Simulated water\nlevel $(m)$")
    fig.axes_dict["RON"].set_xlabel("Observed organic nitrogen $(mg/L)$")
    fig.axes_dict["RON"].set_ylabel("Simulated organic\nnitrogen $(mg/L)$")
    fig.axes_dict["RAN"].set_xlabel("Observed ammonia nitrogen $(mg/L)$")
    fig.axes_dict["RAN"].set_ylabel("Simulated ammonia\nnitrogen $(mg/L)$")
    fig.axes_dict["RNN"].set_xlabel("Observed nitrate nitrogen $(mg/L)$")
    fig.axes_dict["RNN"].set_ylabel("Simulated nitrate\nnitrogen $(mg/L)$")

    fig.axes_dict["FACD"].set_ylim(4, 12)
    fig.axes_dict["FAVD"].set_ylim(4, 12)
    fig.axes_dict["FACD"].tick_params(axis="y", color="b")
    fig.axes_dict["FACD"].spines["left"].set_color("b")
    for label in fig.axes_dict["FACD"].get_yticklabels():
        label.set_color("b")
    fig.axes_dict["FACT"].set_ylim(-4, 36)
    fig.axes_dict["FAVT"].set_ylim(-4, 36)
    fig.axes_dict["FACT"].tick_params(axis="y", color="r")
    fig.axes_dict["FACT"].spines["left"].set_color("r")
    for label in fig.axes_dict["FACT"].get_yticklabels():
        label.set_color("r")

    fig.axes_dict["WLC"].set_ylim(130, 145)
    fig.axes_dict["WLV"].set_ylim(130, 145)
    fig.axes_dict["ONC"].set_ylim(0, 0.40)
    fig.axes_dict["ONV"].set_ylim(0, 0.40)
    fig.axes_dict["ANC"].set_ylim(0, 0.30)
    fig.axes_dict["ANV"].set_ylim(0, 0.30)

    fig.axes_dict["NNC"].set_ylim(0.00, 1.50)
    fig.axes_dict["NNV"].set_ylim(0.00, 1.50)

    for key in ["FAVT", "WLV", "ONV", "ANV", "NNV", "FAVD"]:
        fig.axes_dict[key].yaxis.set_visible(False)
        fig.axes_dict[key].spines["left"].set_visible(False)
        fig.axes_dict[key].set_xlim(datetime(2016, 1, 1), datetime(2016, 12, 31))

        fig.axes_dict[key].set_xticks(
            pd.date_range("2015-12-31", "2016-12-31", freq="M")
        )
        fig.axes_dict[key].xaxis.set_major_formatter(DateFormatter("%m-%d"))
        fig.axes_dict[key].set_xticklabels(
            fig.axes_dict[key].get_xticklabels(), ha="right", rotation=45
        )

    for key in ["FACT", "WLC", "ONC", "ANC", "NNC", "FACD"]:
        fig.axes_dict[key].yaxis.set_major_formatter("{x:.2f}")
        fig.axes_dict[key].spines["right"].set_visible(False)
        fig.axes_dict[key].set_xlim(datetime(2014, 12, 31), datetime(2015, 12, 31))

        fig.axes_dict[key].set_xticks(
            pd.date_range("2014-12-31", "2015-12-31", freq="M")
        )
        fig.axes_dict[key].xaxis.set_major_formatter(DateFormatter("%m-%d"))
        fig.axes_dict[key].set_xticklabels(
            fig.axes_dict[key].get_xticklabels(), ha="right", rotation=45
        )
    fig.axes_dict["FACD"].yaxis.set_major_formatter("{x:.0f}")
    fig.axes_dict["FACT"].yaxis.set_major_formatter("{x:.0f}")
    fig.axes_dict["WLC"].yaxis.set_major_formatter("{x:.1f}")

    for key in ["FACT", "FAVT", "WLC", "WLV", "ONC", "ONV", "ANC", "ANV"]:
        fig.axes_dict[key].set_xticklabels([])
    axes_aligned = [fig.axes_dict[i] for i in ["FACD", "WLC", "ONC", "ANC", "NNC"]]
    fig.plot_line_between_2ax(
        (1, 1.2),
        (1, 0),
        ax1=fig.axes_dict["FACT"],
        ax2=fig.axes_dict["NNC"],
        coords1="axes fraction",
        coords2="axes fraction",
        linewidth=0.5,
        color="grey",
    )
    fig.plot_line_between_2ax(
        (0, 1.2),
        (0, 0),
        ax1=fig.axes_dict["FACT"],
        ax2=fig.axes_dict["NNC"],
        coords1="axes fraction",
        coords2="axes fraction",
        linewidth=0.5,
        color="grey",
    )
    fig.plot_line_between_2ax(
        (0, 1.2),
        (0, 0),
        ax1=fig.axes_dict["FAVT"],
        ax2=fig.axes_dict["NNV"],
        coords1="axes fraction",
        coords2="axes fraction",
        linewidth=0.5,
        color="grey",
    )
    fig.plot_line_between_2ax(
        (1, 1.2),
        (1, 0),
        ax1=fig.axes_dict["FAVT"],
        ax2=fig.axes_dict["NNV"],
        coords1="axes fraction",
        coords2="axes fraction",
        linewidth=0.5,
        color="grey",
    )
    fig.align_ylabels_coords(axes_aligned, -0.27, 0.5)
    fig.axes_dict["FACT"].set_title(" $\mathbf{(a)}$")
    fig.axes_dict["WLC"].set_title(" $\mathbf{(b_1)}$")
    fig.axes_dict["ONC"].set_title(" $(\mathbf{b_2})$")
    fig.axes_dict["ANC"].set_title(" $(\mathbf{b_3})$")
    fig.axes_dict["NNC"].set_title(" $(\mathbf{b_4})$")
    fig.axes_dict["RWL"].set_title(" $(\mathbf{c_1})$")
    fig.axes_dict["RON"].set_title(" $(\mathbf{c_2})$")
    fig.axes_dict["RAN"].set_title(" $(\mathbf{c_3})$")
    fig.axes_dict["RNN"].set_title(" $(\mathbf{c_4})$")
    fig.line_annoate_text(
        fig.axes_dict["FACT"],
        (0, 1.12),
        (1, 1.12),
        "Calibration (2015)",
        transform=fig.axes_dict["FACT"].transAxes,
    )
    fig.line_annoate_text(
        fig.axes_dict["FAVT"],
        (0, 1.12),
        (1, 1.12),
        "Validaton (2016)",
        transform=fig.axes_dict["FAVT"].transAxes,
    )
    fig.axes_dict["NNC"].set_xlabel("Time")
    fig.axes_dict["NNC"].xaxis.set_label_coords(1.02, -0.42)
    fig.axes_dict["RNN"].xaxis.set_label_coords(0.5, -0.3)
    fig.axes_dict["RWL"].plot(
        [0, 1],
        [0, 1],
        transform=fig.axes_dict["RWL"].transAxes,
        lw=0.5,
        linestyle="dashed",
        color="k",
    )
    fig.axes_dict["RON"].plot(
        [0, 1],
        [0, 1],
        transform=fig.axes_dict["RON"].transAxes,
        lw=0.5,
        linestyle="dashed",
        color="k",
    )
    fig.axes_dict["RAN"].plot(
        [0, 1],
        [0, 1],
        transform=fig.axes_dict["RAN"].transAxes,
        lw=0.5,
        linestyle="dashed",
        color="k",
    )
    fig.axes_dict["RNN"].plot(
        [0, 1],
        [0, 1],
        transform=fig.axes_dict["RNN"].transAxes,
        lw=0.5,
        linestyle="dashed",
        color="k",
    )
    return fig, fig.axes_dict


def read_observation():
    obs_dataset = pd.read_excel(
        "test/data/5-0ResModel-Calibrate.xlsx",
        sheet_name="ResAverage",
    )
    calibrate_range = slice(0, 365, 1)
    obs_wl, obs_T_init, obs_DO_init, obs_T, obs_DO, obs_Con, obs_Can, obs_Cnn, index = (
        obs_dataset["Water_Level"],
        obs_dataset["Res_T_init"],
        obs_dataset["Res_DO_init"],
        obs_dataset["Res_T"],
        obs_dataset["Res_DO"],
        obs_dataset["Res_Cno"],
        obs_dataset["Res_Cna"],
        obs_dataset["Res_Cnn"],
        obs_dataset["Date"],
    )
    obs_calibrate = [
        obs_wl[calibrate_range].values,
        obs_T_init[calibrate_range].values,
        obs_DO_init[calibrate_range].values,
        obs_Con[calibrate_range].values,
        obs_Can[calibrate_range].values,
        obs_Cnn[calibrate_range].values,
        obs_T[calibrate_range].values,
        obs_DO[calibrate_range].values,
        index[calibrate_range].values,
    ]

    obs_dataset = pd.read_excel(
        "test/data/5-0ResModel-Validate.xlsx",
        sheet_name="ResAverage",
    )
    obs_wl, obs_T_init, obs_DO_init, obs_T, obs_DO, obs_Con, obs_Can, obs_Cnn, index = (
        obs_dataset["Water_Level"],
        obs_dataset["Res_T_init"],
        obs_dataset["Res_DO_init"],
        obs_dataset["Res_T"],
        obs_dataset["Res_DO"],
        obs_dataset["Res_Cno"],
        obs_dataset["Res_Cna"],
        obs_dataset["Res_Cnn"],
        obs_dataset["Date"],
    )
    validate_range = slice(0, 366, 1)
    obs_validate = [
        obs_wl[validate_range].values,
        obs_T_init[validate_range].values,
        obs_DO_init[validate_range].values,
        obs_Con[validate_range].values,
        obs_Can[validate_range].values,
        obs_Cnn[validate_range].values,
        obs_T[validate_range].values,
        obs_DO[validate_range].values,
        index[validate_range].values,
    ]
    return obs_calibrate, obs_validate


def read_simulation():
    simu_c = pd.read_excel("test/output/0DRMC_out.xlsx", sheet_name="Simulation")
    simu_v = pd.read_excel("test/output/0DRMV_out.xlsx", sheet_name="Simulation")
    calibrate_range = slice(0, 365, 1)
    validate_range = slice(0, 366, 1)
    wl_simu = simu_c.iloc[:, 0].values
    c_rpon = simu_c.iloc[:, 1].values
    c_lpon = simu_c.iloc[:, 2].values
    c_on = simu_c.iloc[:, 3].values + c_rpon + c_lpon
    c_na = simu_c.iloc[:, 4].values
    c_nn = simu_c.iloc[:, 5].values
    simu_calibrate = [
        wl_simu[calibrate_range],
        c_rpon[calibrate_range],
        c_lpon[calibrate_range],
        c_on[calibrate_range],
        c_na[calibrate_range],
        c_nn[calibrate_range],
    ]

    wl_v = simu_v.iloc[:, 0][validate_range].values
    rpon_v = simu_v.iloc[:, 1][validate_range].values
    lpon_v = simu_v.iloc[:, 2][validate_range].values
    on_v = simu_v.iloc[:, 3][validate_range].values + rpon_v + lpon_v
    na_v = simu_v.iloc[:, 4][validate_range].values
    nn_v = simu_v.iloc[:, 5][validate_range].values
    simu_validate = [wl_v, rpon_v, lpon_v, on_v, na_v, nn_v]
    return simu_calibrate, simu_validate


def visualize(axes, obs_calibrate, obs_validate, simu_calibrate, simu_validate):
    # 观测数据
    axes["FACT"].plot(obs_calibrate[-1], obs_calibrate[1], "ro", markersize=2)
    axes["FACT"].plot(obs_calibrate[-1], obs_calibrate[-3], "r--")
    (obs_t,) = axes["FAVT"].plot(obs_validate[-1], obs_validate[1], "ro", markersize=2)
    (int_t,) = axes["FAVT"].plot(obs_validate[-1], obs_validate[-3], "r--")

    axes["FACD"].plot(obs_calibrate[-1], obs_calibrate[2], "bs", markersize=2)
    axes["FACD"].plot(obs_calibrate[-1], obs_calibrate[-2], "b--")
    (obs_do,) = axes["FAVD"].plot(obs_validate[-1], obs_validate[2], "bs", markersize=2)
    (int_do,) = axes["FAVD"].plot(obs_validate[-1], obs_validate[-2], "b--")
    obs_t = axes["FAVD"].legend(
        [obs_t, int_t, obs_do, int_do],
        ["T $(^\circ C)$", "Interpolation", "DO $(mg/L)$", "Interpolation"],
        frameon=False,
        loc="lower center",
        fontsize=5,
        edgecolor="k",
        ncols=2,
        handlelength=1,
        columnspacing=0.3,
        handleheight=0.3,
        handletextpad=0.3,
    )
    obs_t.legendPatch.set_linewidth(0.3)

    # 模型对比
    axes["WLC"].plot(
        obs_calibrate[-1], obs_calibrate[0], linewidth=0.75, color="r", label="Observed"
    )
    axes["WLV"].plot(
        obs_validate[-1], obs_validate[0], linewidth=0.75, color="r", label="Observed"
    )
    rmse_wl_c = rmse(obs_calibrate[0], simu_calibrate[0])
    rmse_wl_v = rmse(obs_validate[0], simu_validate[0])
    mape_wl_c = mape(obs_calibrate[0], simu_calibrate[0])
    mape_wl_v = mape(obs_validate[0], simu_validate[0])
    axes["WLC"].text(0.2, 0.87, f"RMSE={rmse_wl_c:.2f}, MAPE={mape_wl_c:.2f}%", transform=axes["WLC"].transAxes, fontsize=6)
    axes["WLV"].text(0.05, 0.87, f"RMSE={rmse_wl_v:.2f}, MAPE={mape_wl_v:.2f}%", transform=axes["WLV"].transAxes, fontsize=6)

    axes["WLC"].plot(
        obs_calibrate[-1],
        simu_calibrate[0],
        linewidth=0.75,
        color="b",
        label="Simulation",
    )
    axes["WLV"].plot(
        obs_validate[-1],
        simu_validate[0],
        linewidth=0.75,
        color="b",
        label="Simulation",
    )
    axes["RWL"].scatter(obs_calibrate[0], simu_calibrate[0], color="r", s=3)
    axes["RWL"].scatter(obs_validate[0], simu_validate[0], color='b', s=3, marker='s')
    axes["RWL"].set_xlim(130, 145)
    axes["RWL"].set_ylim(130, 145)
    axes["RWL"].set_aspect(1)
    nse_c = r2(obs_calibrate[0], simu_calibrate[0])
    nse_v = r2(obs_validate[0], simu_validate[0])
    axes["RWL"].text(
        0.02, 0.62, f"$\\rho_c$={nse_c:.4f}\n$\\rho_v$={nse_v:.4f}", transform=axes["RWL"].transAxes, fontsize=6
    )

    axes["ONC"].plot(obs_calibrate[-1], obs_calibrate[3], "ro", markersize=2)
    axes["ONC"].plot(obs_calibrate[-1], simu_calibrate[3], "b-", linewidth=0.75)
    axes["ONC"].plot(obs_calibrate[-1], simu_calibrate[1], color="grey", linestyle='-', lw=0.5)
    axes["ONC"].plot(obs_calibrate[-1], simu_calibrate[2], color="k", linestyle='-', lw=0.5)

    axes["ONV"].plot(obs_validate[-1], obs_validate[3], "ro", markersize=2)
    axes["ONV"].plot(obs_validate[-1], simu_validate[3], "b-", linewidth=0.75)
    axes["ONV"].plot(obs_validate[-1], simu_validate[1], color="grey", linestyle='-', lw=0.5)
    axes["ONV"].plot(obs_validate[-1], simu_validate[2], color="k", linestyle='-', lw=0.5)

    rmse_on_c = rmse(obs_calibrate[3], simu_calibrate[3])
    rmse_on_v = rmse(obs_validate[3], simu_validate[3])
    mape_on_c = mape(obs_calibrate[3], simu_calibrate[3])
    mape_on_v = mape(obs_validate[3], simu_validate[3])
    axes["ONC"].text(0.2, 0.87, f"RMSE={rmse_on_c:.2f}, MAPE={mape_on_c:.2f}%", transform=axes["ONC"].transAxes, fontsize=6)
    axes["ONV"].text(0.05, 0.87, f"RMSE={rmse_on_v:.2f}, MAPE={mape_on_v:.2f}%", transform=axes["ONV"].transAxes, fontsize=6)
    axes["RON"].scatter(obs_calibrate[3], simu_calibrate[3], color="r", s=3)
    axes["RON"].scatter(obs_validate[3], simu_validate[3], color="b", s=3, marker="s")
    axes["RON"].set_xlim(0, 0.4)
    axes["RON"].set_ylim(0, 0.4)
    axes["RON"].set_aspect(1)
    nse_c = r2(obs_calibrate[3], simu_calibrate[3])
    nse_v = r2(obs_validate[3], simu_validate[3])
    axes["RON"].text(
        0.02, 0.62, f"$\\rho_c$={nse_c:.4f}\n$\\rho_v$={nse_v:.4f}", transform=axes["RON"].transAxes, fontsize=6
    )

    axes["ANC"].plot(obs_calibrate[-1], obs_calibrate[4], "ro", markersize=2)
    axes["ANV"].plot(obs_validate[-1], obs_validate[4], "ro", markersize=2)
    axes["ANC"].plot(obs_calibrate[-1], simu_calibrate[4], "b-", linewidth=0.75)
    axes["ANV"].plot(obs_validate[-1], simu_validate[4], "b-", linewidth=0.75)
    rmse_an_c = rmse(obs_calibrate[4], simu_calibrate[4])
    rmse_an_v = rmse(obs_validate[4], simu_validate[4])
    mape_an_c = mape(obs_calibrate[4], simu_calibrate[4])
    mape_an_v = mape(obs_validate[4], simu_validate[4])
    axes["ANC"].text(0.2, 0.87, f"RMSE={rmse_an_c:.2f}, MAPE={mape_an_c:.2f}%", transform=axes["ANC"].transAxes, fontsize=6)
    axes["ANV"].text(0.05, 0.87, f"RMSE={rmse_an_v:.2f}, MAPE={mape_an_v:.2f}%", transform=axes["ANV"].transAxes, fontsize=6)
    axes["RAN"].scatter(obs_calibrate[4], simu_calibrate[4], color="r", s=3)
    axes["RAN"].scatter(obs_validate[4], simu_validate[4], color="b", s=3, marker='s')
    axes["RAN"].set_xlim(0, 0.3)
    axes["RAN"].set_ylim(0, 0.3)
    axes["RAN"].set_aspect(1)
    nse_c = r2(obs_calibrate[4], simu_calibrate[4])
    nse_v = r2(obs_validate[4], simu_validate[4])
    axes["RAN"].text(
        0.02, 0.62, f"$\\rho_c$={nse_c:.4f}\n$\\rho_v$={nse_v:.4f}", transform=axes["RAN"].transAxes, fontsize=6
    )

    axes["NNC"].plot(obs_calibrate[-1], obs_calibrate[5], "ro", markersize=2)
    axes["NNV"].plot(obs_validate[-1], obs_validate[5], "ro", markersize=2)
    axes["NNC"].plot(obs_calibrate[-1], simu_calibrate[5], "b-", linewidth=0.75)
    axes["NNV"].plot(obs_validate[-1], simu_validate[5], "b-", linewidth=0.75)
    rmse_nn_c = rmse(obs_calibrate[5], simu_calibrate[5])
    rmse_nn_v = rmse(obs_validate[5], simu_validate[5])
    mape_nn_c = mape(obs_calibrate[5], simu_calibrate[5])
    mape_nn_v = mape(obs_validate[5], simu_validate[5])
    axes["NNC"].text(0.2, 0.87, f"RMSE={rmse_nn_c:.2f}, MAPE={mape_nn_c:.2f}%", transform=axes["NNC"].transAxes, fontsize=6)
    axes["NNV"].text(0.05, 0.87, f"RMSE={rmse_nn_v:.2f}, MAPE={mape_nn_v:.2f}%", transform=axes["NNV"].transAxes, fontsize=6)
    axes["RNN"].scatter(obs_calibrate[5], simu_calibrate[5], color="r", s=3)
    axes["RNN"].scatter(obs_validate[5], simu_validate[5], color="b", s=3, marker='s')
    axes["RNN"].set_xlim(0, 1.5)
    axes["RNN"].set_ylim(0, 1.5)
    # axes['RNN'].xaxis.set_major_locator(axes['RNN'].yaxis.get_major_locator())
    axes["RNN"].set_aspect(1)
    nse_c = r2(obs_calibrate[5], simu_calibrate[5])
    nse_v = r2(obs_validate[5], simu_validate[5])
    axes["RNN"].text(
        0.02, 0.62, f"$\\rho_c$={nse_c:.4f}\n$\\rho_v$={nse_v:.4f}", transform=axes["RNN"].transAxes, fontsize=6
    )

    return


if __name__ == "__main__":
    # 率定测试
    run_model(
        exe_path="build/bin/0DReservoirModel",
        filein="test/data/5-0ResModel-Calibrate.xlsx",
        fileout="test/output/0DRMC_out.xlsx",
        params=params_dict,
        id=0,
        toggle=calibrate_toggle,
    )
    # 验证测试
    run_model(
        exe_path="build/bin/0DReservoirModel",
        filein="test/data/5-0ResModel-Validate.xlsx",
        fileout="test/output/0DRMV_out.xlsx",
        params=params_dict,
        id=0,
        toggle=validate_toggle,
    )

    fig, axes = create_fig()
    obs_calibrate, obs_validate = read_observation()
    simu_calibrate, simu_validate = read_simulation()
    visualize(axes, obs_calibrate, obs_validate, simu_calibrate, simu_validate)
    fig.show()
