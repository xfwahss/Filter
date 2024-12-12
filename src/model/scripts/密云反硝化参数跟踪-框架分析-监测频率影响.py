import numpy as np
from plotstyles.fonts import global_fonts
from plotstyles.figure import Figure
from plotstyles import styles
import pandas as pd
import subprocess
import os
import model_tool
from matplotlib.patches import ConnectionPatch

styles.use("wr")


def re(c_true, c_simulate):
    return np.mean(np.abs((c_simulate - c_true) / (c_true + 1e-12))) * 100


def r2(c_true, c_simulate):
    return np.corrcoef(c_true, c_simulate)[0, 1]


def update_frequency_amplitude(
    amplitude=0.075,
    freq=1,
    file_path="./test/data/1stMiyunParameterTracker_frequency.xlsx",
):
    xls = pd.ExcelFile(file_path)
    sheet_names = xls.sheet_names  # 获取所有工作表名称
    sheets_dict = {
        sheet_name: pd.read_excel(file_path, sheet_name=sheet_name, header=None)
        for sheet_name in sheet_names
    }
    sheet_name_to_modify = "Obs"  # 要修改的工作表名称
    mask = (sheets_dict[sheet_name_to_modify].index - 1) % freq == 0
    mask[0] = True
    sheets_dict[sheet_name_to_modify].iloc[:, 1] = sheets_dict["Origin_Data"].iloc[:, 3]
    sheets_dict[sheet_name_to_modify].iloc[~mask, 1] = -999.0

    row_index = 7  # 行索引
    col_index = 1  # 列索引
    sheets_dict["Params"].iat[row_index, col_index] = amplitude
    with pd.ExcelWriter(file_path, engine="openpyxl") as writer:
        for sheet_name, df in sheets_dict.items():
            df.to_excel(writer, sheet_name=sheet_name, index=False, header=False)


def run_model(exe, toggle=True):
    cwd = os.getcwd()
    os.chdir(os.path.abspath("build"))
    if toggle:
        subprocess.run(
            [
                exe,
                "-i",
                "../test/data/1stMiyunParameterTracker_frequency.xlsx",
                "-o",
                "../test/output/1stMiyunParameterTracker_out_frequency.xlsx",
            ]
        )
    os.chdir(cwd)


def read_filter_status(path="test/output/1stMiyunParameterTracker_out_frequency.xlsx"):
    data = pd.read_excel(path, sheet_name="X")
    data_prior = pd.read_excel(path, sheet_name="prior_X")
    x = data["x"].values
    prior_x = data_prior.iloc[:, 1]
    k = data["v"].values
    dk = data["dv"].values
    ddk = data["ddv"].values
    return x, k, dk, prior_x, ddk


def read_filter_variance(
    path="test/output/1stMiyunParameterTracker_out_frequency.xlsx",
):
    data = pd.read_excel(path, sheet_name="P")
    x = data["x"].values
    k = data["v"].values
    return x, k


def read_true_value(path="test/data/1stMiyunParameterTracker_frequency.xlsx"):
    data = pd.read_excel(path, sheet_name="Origin_Data")
    c_obs = data["c_obs"]
    k = data["k"]
    c_true = data["c_true"]
    t = data["t"]
    return t, k, c_true, c_obs


def run_model_with_frequency_amplitude(frequency, amplitude):
    update_frequency_amplitude(amplitude=amplitude, freq=frequency)
    run_model("bin/1stOrderReactionParameterTracker", toggle=True)
    c_filter, k_filter, dk, prior_c, ddk = read_filter_status()
    c_variance, k_variance = read_filter_variance()
    t, k_true, c_true, c_obs = read_true_value()
    df = pd.DataFrame()
    df["t"] = t
    df["c_filter"] = c_filter
    df["c_true"] = c_true
    df["k_filter"] = k_filter
    df["k_true"] = k_true
    df["dk"] = dk
    df["prior_c"] = prior_c
    df["ddk"] = ddk
    df["c_variance"] = c_variance
    df["k_variance"] = k_variance
    return amplitude, frequency, df


def run_models(
    save_path="test/output/1stMiyunModel-Range-Freqs.xlsx",
    freqs=[1, 3, 5, 7, 15, 30],
    amplitude=0.075,
):
    sheets_dict = {}
    for freq in freqs:
        ampl, freq, df = run_model_with_frequency_amplitude(freq, amplitude)
        sheets_dict[f"amplitude-{ampl}-{freq}"] = df
    with pd.ExcelWriter(save_path, engine="openpyxl") as writer:
        for sheet_name, df in sheets_dict.items():
            df.to_excel(writer, sheet_name=sheet_name, index=False, header=True)


def plot_in_ax(
    ax_c,
    ax_k,
    amplitude,
    frequency,
    file_path="test/output/1stMiyunModel-Range-Freqs.xlsx",
    plot_true_value=False,
    color="b",
    zorder=1,
):
    freq_loc = {"1": 1, "3": 2, "5": 3, "7": 4, "15": 5, "30": 6}
    sheet_name = f"amplitude-{amplitude}-{frequency}"
    data = pd.read_excel(file_path, sheet_name=sheet_name)
    t = data["t"]
    k_true = data["k_true"]
    k_filter = data["k_filter"]
    c_true = data["c_true"]
    c_filter = data["c_filter"]

    line_ktrue, line_ctrue = 0, 0
    if plot_true_value:
        # (line_ktrue,) = ax_k.plot(
        #     xs=t, ys=[0] * t.size, zs=k_true, color="k", lw=2, zorder=8
        # )
        (line_ctrue,) = ax_c.plot(t, c_true, color="#808080", lw=2, linestyle="solid")
    (line_ktrue,) = ax_k.plot(
        xs=t,
        ys=[freq_loc[f"{frequency}"]] * t.size,
        zs=k_true,
        color="#808080",
        lw=0.5,
        zorder=zorder,
    )
    (line_kfilter,) = ax_k.plot(
        xs=t,
        ys=[freq_loc[f"{frequency}"]] * t.size,
        zs=k_filter,
        color=color,
        lw=1,
        zorder=zorder,
    )
    (line_cfilter,) = ax_c.plot(t, c_filter, color=color, lw=1, zorder=zorder)
    mape_c = re(c_true, c_filter)
    r_c = r2(k_true, k_filter)
    return t, c_true, c_filter, mape_c, r_c, line_ktrue, line_kfilter, line_ctrue, line_cfilter


def create_figure():
    fig = Figure(15, 8)
    fig.add_axes_cm("c", 1.3, 0.7, 5, 6, "left upper")
    fig.add_axes_cm("k", 6.5, 0.09, 7, 7.5, "left upper", projection="3d")
    fig.axes_dict["k"].grid(False)
    fig.axes_dict["k"].set_zlim(0, 0.005)
    fig.axes_dict["k"].set_xlim(0, 800)
    fig.axes_dict["k"].set_xlabel("Day")
    fig.axes_dict["k"].set_zlabel("k (1/d)")
    fig.axes_dict["k"].set_ylabel("Frequency (day)")
    fig.axes_dict["k"].set_yticks([1, 2, 3, 4, 5, 6], ["1", "3", "5", "7", "15", "30"])
    fig.axes_dict["c"].set_ylim(0.5, 1)
    fig.axes_dict["c"].set_ylabel("Nitrate (mg/L)")
    fig.axes_dict["c"].set_xlim(0, 800)
    fig.axes_dict["c"].set_xlabel("Day")
    fig.axes_dict["c"].set_title("  (a)", y=0.9)
    fig.axes_dict["k"].set_title("(b)", y=0.9)
    fig.axes_dict["k"].xaxis.set_pane_color((1.0, 1.0, 1.0, 1.0))
    fig.axes_dict["k"].yaxis.set_pane_color((1.0, 1.0, 1.0, 1.0))
    fig.axes_dict["k"].zaxis.set_pane_color((1.0, 1.0, 1.0, 1.0))
    return fig, fig.axes_dict


if __name__ == "__main__":
    toggle = False
    if toggle:
        run_models()
    fig, axes = create_figure()
    colors = ["#FA7F6F", "#9394E7", "#63E398", "#05B9E7", "#C76DA2", "#A9B8C6"]
    t, c_true, c_1, mapec1, rk1, k_line_ture, k_line1, c_line_true, c_line1 = plot_in_ax(
        axes["c"],
        axes["k"],
        amplitude=0.075,
        frequency=1,
        plot_true_value=True,
        color=colors[0],
        zorder=7,
    )
    t, c_true, c_2, mapec2, rk2, _, k_line3, _, c_line3 = plot_in_ax(
        axes["c"], axes["k"], amplitude=0.075, frequency=3, color=colors[1], zorder=6
    )
    t, c_true, c_3, mapec3, rk3, _, k_line5, _, c_line5 = plot_in_ax(
        axes["c"], axes["k"], amplitude=0.075, frequency=5, color=colors[2], zorder=5
    )
    t, c_true, c_4, mapec4, rk4, _, k_line7, _, c_line7 = plot_in_ax(
        axes["c"], axes["k"], amplitude=0.075, frequency=7, color=colors[3], zorder=4
    )
    t, c_true, c_5, mapec5, rk5, _, k_line15, _, c_line15 = plot_in_ax(
        axes["c"], axes["k"], amplitude=0.075, frequency=15, color=colors[4], zorder=3
    )
    t, c_true, c_6, mapec6, rk6, _, k_line30, _, c_line30 = plot_in_ax(
        axes["c"], axes["k"], amplitude=0.075, frequency=30, color=colors[5], zorder=2
    )

    c_handles = [c_line_true, c_line1, c_line3, c_line5, c_line7, c_line15, c_line30]
    c_labels = [
        "True Value",
        f"1 day: MAPE={mapec1:.2f}%",
        f"3 day: MAPE={mapec2:.2f}%",
        f"5 day: MAPE={mapec3:.2f}%",
        f"7 day: MAPE={mapec4:.2f}%",
        f"15 day: MAPE={mapec5:.2f}%",
        f"30 day: MAPE={mapec6:.2f}%",
    ]
    axes["c"].legend(c_handles, c_labels, frameon=False)
    axes["k"].text2D(
        0.1,
        0.58,
        f"1 day: r={rk1:.2f}\n3 day: r={rk2:.2f}\n5 day: r={rk3:.2f}\n7 day: r={rk4:.2f}\n15 day: r={rk5:.2f}\n30 day: r={rk6:.2f}",
        transform=axes["k"].transAxes,
    )

    axin = axes["c"].inset_axes([0.10, 0.02, 0.5, 0.30])
    fig.zone_and_linked(
        axes["c"], axin, 300, 450, t, [c_true, c_1, c_2, c_3, c_4, c_5, c_6], 'bottom'
    )
    for c, col in zip([c_1, c_2, c_3, c_4, c_5, c_6], colors):
        axin.plot(t, c, lw=1, color=col, zorder=2)
    axin.plot(t, c_true, lw=2, color='#808080', zorder=1)
    axin.xaxis.set_visible(False)
    axin.yaxis.set_visible(False)
    fig.show()
