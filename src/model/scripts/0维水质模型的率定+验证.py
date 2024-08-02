from plotstyles import styles
from plotstyles import figure
import pandas as pd
import subprocess
import time
from functools import wraps
from datetime import datetime
from matplotlib.dates import DateFormatter, MonthLocator, DayLocator

styles.use(["wr"])
params_dict = {
    #  有机氮参数
    "--b_ndo_flux": 0.056,
    "--s_nrp": 0.015,
    "--s_nlp": 0.015,
    "--alpha_rpon": 0.75,
    "--alpha_lpon": 0.15,
    "--k_rpon": 0.0005,
    "--theta_rpon": 1.05,
    "--k_lpon": 0.005,
    "--theta_lpon": 1.06,
    "--k_don": 0.048,
    "--theta_don": 1.05,
    # 硝化过程参数
    "--b_amm_flux": 0.00,
    "--rnit0": 0.00135,
    "--knit20": 0.012,
    "--foxmin": 0.25,
    "--c_oxc_nit": 6.0,
    "--c_oxo_nit": 12.0,
    "--theta_nit": 1.08,
    "--T_c_nit": 5.5,
    "--alpha": 0.15,
    # 反硝化过程参数
    "--b_nit_flux": 0.00,
    "--rdeni0": 0.0016,
    "--kdeni20": 0.010,
    "--Tc_deni": -2,
    "--theta_deni": 1.20,
    "--c_oxc_deni": 10,
    "--c_oxo_deni": 3,
    "--beta": 1,
    # 其他参数
    "--h0": 100,
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
    fig.axes_dict['FACD'].tick_params(axis='y', color='b')
    fig.axes_dict['FACD'].spines['left'].set_color('b')
    for label in fig.axes_dict['FACD'].get_yticklabels():
        label.set_color('b')
    fig.axes_dict["FACT"].set_ylim(-4, 36)
    fig.axes_dict["FAVT"].set_ylim(-4, 36)
    fig.axes_dict['FACT'].tick_params(axis='y', color='r')
    fig.axes_dict['FACT'].spines['left'].set_color('r')
    for label in fig.axes_dict['FACT'].get_yticklabels():
        label.set_color('r')

    fig.axes_dict['WLC'].set_ylim(130, 145)
    fig.axes_dict['WLV'].set_ylim(130, 145)
    fig.axes_dict['ONC'].set_ylim(0, 0.40)
    fig.axes_dict['ONV'].set_ylim(0, 0.40)
    fig.axes_dict['ANC'].set_ylim(0, 0.50)
    fig.axes_dict['ANV'].set_ylim(0, 0.50)

    fig.axes_dict['NNC'].set_ylim(0.00, 1.50)
    fig.axes_dict['NNV'].set_ylim(0.00, 1.50)

    for key in ["FAVT", "WLV", "ONV", "ANV", "NNV", "FAVD"]:
        fig.axes_dict[key].yaxis.set_visible(False)
        fig.axes_dict[key].spines["left"].set_visible(False)
        fig.axes_dict[key].set_xlim(datetime(2016, 1, 1), datetime(2016, 12, 31))

        fig.axes_dict[key].set_xticks(pd.date_range('2015-12-31', '2016-12-31', freq='M'))
        fig.axes_dict[key].xaxis.set_major_formatter(DateFormatter("%m-%d"))
        fig.axes_dict[key].set_xticklabels(fig.axes_dict[key].get_xticklabels(), ha='right', rotation=45)

    for key in ["FACT", "WLC", "ONC", "ANC", "NNC", "FACD"]:
        fig.axes_dict[key].yaxis.set_major_formatter("{x:.2f}")
        fig.axes_dict[key].spines["right"].set_visible(False)
        fig.axes_dict[key].set_xlim(datetime(2014, 12, 31), datetime(2015, 12, 31))

        fig.axes_dict[key].set_xticks(pd.date_range('2014-12-31', '2015-12-31', freq='M'))
        fig.axes_dict[key].xaxis.set_major_formatter(DateFormatter("%m-%d"))
        fig.axes_dict[key].set_xticklabels(fig.axes_dict[key].get_xticklabels(), ha='right', rotation=45)
    fig.axes_dict['FACD'].yaxis.set_major_formatter("{x:.0f}")
    fig.axes_dict['FACT'].yaxis.set_major_formatter("{x:.0f}")
    fig.axes_dict['WLC'].yaxis.set_major_formatter("{x:.1f}")
    
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
    fig.axes_dict["RWL"].set_aspect(1)
    fig.axes_dict["RON"].plot(
        [0, 1],
        [0, 1],
        transform=fig.axes_dict["RON"].transAxes,
        lw=0.5,
        linestyle="dashed",
        color="k",
    )
    fig.axes_dict["RON"].set_aspect(1)
    fig.axes_dict["RAN"].plot(
        [0, 1],
        [0, 1],
        transform=fig.axes_dict["RAN"].transAxes,
        lw=0.5,
        linestyle="dashed",
        color="k",
    )
    fig.axes_dict["RAN"].set_aspect(1)
    fig.axes_dict["RNN"].plot(
        [0, 1],
        [0, 1],
        transform=fig.axes_dict["RNN"].transAxes,
        lw=0.5,
        linestyle="dashed",
        color="k",
    )
    fig.axes_dict["RNN"].set_aspect(1)
    return fig, fig.axes_dict


def read_observation():
    obs_dataset = pd.read_excel(
        "test/data/0维水质模型数据未插值.xlsx", sheet_name="ResAverage"
    )
    obs_wl, obs_T, obs_DO, obs_Con, obs_Can, obs_Cnn, index = (
        obs_dataset["Water_Level"],
        obs_dataset["Res_T"],
        obs_dataset["Res_DO"],
        obs_dataset["Res_Cno"],
        obs_dataset["Res_Cna"],
        obs_dataset["Res_Cnn"],
        obs_dataset["Date"],
    )
    calibrate_range = slice(0, 365, 1)
    validate_range = slice(366, 731, 1)
    obs_calibrate = [
        obs_wl[calibrate_range].values,
        obs_T[calibrate_range].values,
        obs_DO[calibrate_range].values,
        obs_Con[calibrate_range].values,
        obs_Can[calibrate_range].values,
        obs_Cnn[calibrate_range].values,
        index[calibrate_range].values,
    ]
    obs_validate = [
        obs_wl[validate_range].values,
        obs_T[validate_range].values,
        obs_DO[validate_range].values,
        obs_Con[validate_range].values,
        obs_Can[validate_range].values,
        obs_Cnn[validate_range].values,
        index[validate_range].values,
    ]
    return obs_calibrate, obs_validate


def read_simulation():
    pass


def read_interporlate():
    pass


def visualize(axes, obs_calibrate, obs_validate):
    # 观测数据
    axes['FACT'].plot(obs_calibrate[-1], obs_calibrate[1], 'ro', markersize=2)
    obs_t, = axes['FAVT'].plot(obs_validate[-1], obs_validate[1], 'ro', markersize=2,label='Observed temperature $(^\circC)$')
    axes['FACD'].plot(obs_calibrate[-1], obs_calibrate[2], 'bs', markersize=2)
    obs_do, = axes['FAVD'].plot(obs_validate[-1], obs_validate[2], 'bs', markersize=2, label='Observed dissolved oxygen $(mg/L)$')
    obs_t = axes['FAVD'].legend([obs_t, obs_do], ['Observed T $(^\circ C)$', 'Observed DO $(mg/L)$'], frameon=True, loc='lower center', fontsize=5, edgecolor='k')
    obs_t.legendPatch.set_linewidth(0.3)

    axes['WLC'].plot(obs_calibrate[-1], obs_calibrate[0], linewidth=0.75, color='r', label='Observed')
    axes['WLV'].plot(obs_validate[-1], obs_validate[0], linewidth=0.75, color='r', label='Observed')
    axes['ONC'].plot(obs_calibrate[-1], obs_calibrate[3], 'ro', markersize=2)
    axes['ONV'].plot(obs_validate[-1], obs_validate[3], 'ro', markersize=2)
    axes['ANC'].plot(obs_calibrate[-1], obs_calibrate[4], 'ro', markersize=2)
    axes['ANV'].plot(obs_validate[-1], obs_validate[4], 'ro', markersize=2)
    axes['NNC'].plot(obs_calibrate[-1], obs_calibrate[5], 'ro', markersize=2)
    axes['NNV'].plot(obs_validate[-1], obs_validate[5], 'ro', markersize=2)
    
    return


if __name__ == "__main__":
    # 率定测试
    run_model(
        exe_path="build/bin/0DReservoirModel",
        filein="test/data/0DReservoirModelCalibrate.xlsx",
        fileout="test/output/0DRMC_out.xlsx",
        params=params_dict,
        id=0,
        toggle=False,
    )

    fig, axes = create_fig()
    obs_calibrate, obs_validate = read_observation()
    visualize(axes, obs_calibrate, obs_validate)
    fig.show()
