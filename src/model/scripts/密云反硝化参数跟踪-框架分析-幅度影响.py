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


def update_amplitude(
    amplitude, file_path="./test/data/1stMiyunParameterTracker_amplitude.xlsx"
):
    xls = pd.ExcelFile(file_path)
    sheet_names = xls.sheet_names  # 获取所有工作表名称
    sheets_dict = {
        sheet_name: pd.read_excel(file_path, sheet_name=sheet_name, header=None)
        for sheet_name in sheet_names
    }
    sheet_name_to_modify = "Params"  # 要修改的工作表名称
    row_index = 7  # 行索引
    col_index = 1  # 列索引
    sheets_dict[sheet_name_to_modify].iat[row_index, col_index] = amplitude
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
                "../test/data/1stMiyunParameterTracker_amplitude.xlsx",
                "-o",
                "../test/output/1stMiyunParameterTracker_out_amplitude.xlsx",
            ]
        )
    os.chdir(cwd)


def read_filter_status(path="test/output/1stMiyunParameterTracker_out_amplitude.xlsx"):
    data = pd.read_excel(path, sheet_name="X")
    data_prior = pd.read_excel(path, sheet_name="prior_X")
    x = data["x"].values
    prior_x = data_prior.iloc[:, 1]
    k = data["v"].values
    dk = data["dv"].values
    ddk = data["ddv"].values
    return x, k, dk, prior_x, ddk


def read_filter_variance(
    path="test/output/1stMiyunParameterTracker_out_amplitude.xlsx",
):
    data = pd.read_excel(path, sheet_name="P")
    x = data["x"].values
    k = data["v"].values
    return x, k


def read_true_value(path="test/data/1stMiyunParameterTracker_amplitude.xlsx"):
    data = pd.read_excel(path, sheet_name="Origin_Data")
    c_obs = data["c_obs"]
    k = data["k"]
    c_true = data["c_true"]
    t = data["t"]
    return t, k, c_true, c_obs


def run_model_with_amplitude(amplitude):
    update_amplitude(amplitude)
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
    return amplitude, df


def run_models(save_path, start=0, end=1, step=0.01):
    sheets_dict = {}
    for amplitude in np.arange(start, end + step, step):
        number, df = run_model_with_amplitude(amplitude)
        sheets_dict[f"amplitude-{number:.2f}"] = df
    with pd.ExcelWriter(save_path, engine="openpyxl") as writer:
        for sheet_name, df in sheets_dict.items():
            df.to_excel(writer, sheet_name=sheet_name, index=False, header=True)


def read_r_and_mape(
    file_path="./test/output/1stMiyunModel-Range-Amplitude.xlsx",
    start=0,
    end=1,
    step=0.01,
):
    amplitudes = np.arange(start, end, step)
    rs = []
    c_mapes = []
    for amplitude in amplitudes:
        sheet_name = f"amplitude-{amplitude:.2f}"
        data = pd.read_excel(file_path, sheet_name=sheet_name)
        r = r2(data["k_true"].array, data["k_filter"].array)
        c_mape = re(data["c_true"].array, data["c_filter"].array)
        rs.append(r)
        c_mapes.append(c_mape)
    return amplitudes, rs, c_mapes


def create_figure():
    fig = Figure(15.7, 12.5)
    left, top, vspace, hspace, width, height, vspace2 = 1.2, 0.5, 0.7, 0.5, 4, 3, 1
    fig.add_axes_cm(
        "MAPE",
        left,
        top + height + vspace,
        3 * width + 2 * hspace,
        height,
        "left upper",
    )
    ax = fig.axes_dict["MAPE"].twinx()
    fig.axes_dict["R"] = ax
    fig.add_axes_cm("1", left, top, width, height, "left upper")
    fig.add_axes_cm("2", left + width + hspace, top, width, height, "left upper")
    fig.add_axes_cm(
        "3", left + 2 * width + 2 * hspace, top, width, height, "left upper"
    )
    fig.add_axes_cm(
        "4", left, top + 2 * height + vspace + vspace2, width, height, "left upper"
    )
    fig.add_axes_cm(
        "5",
        left + width + hspace,
        top + 2 * height + vspace + vspace2,
        width,
        height,
        "left upper",
    )
    fig.add_axes_cm(
        "6",
        left + 2 * width + 2 * hspace,
        top + 2 * height + vspace + vspace2,
        width,
        height,
        "left upper",
    )
    return fig, fig.axes_dict


def plot_k_curve(
    ax,
    title,
    amplitude,
    ampl_rs,
    ampl_mape,
    linkedax,
    linked_dir="bottom",
    file_path="./test/output/1stMiyunModel-Range-Amplitude.xlsx",
):
    sheet_name = f"amplitude-{amplitude:.2f}"
    data = pd.read_excel(file_path, sheet_name=sheet_name)
    t = data["t"]
    k_true = data["k_true"]
    k_filter = data["k_filter"]
    (line_ktrue,) = ax.plot(t, k_true, color="k", lw=1, linestyle='dashed')
    (line_kfilter,) = ax.plot(t, k_filter, color="b", lw=1)
    ax.set_title(f"  ({title}) Amplitude={amplitude:.2f}")
    ax.set_xlim(0, 800)
    ax.ticklabel_format(style="sci", scilimits=(-3, 1), axis="y", useMathText=True)
    ax.set_ylim(0, 0.004)
    linkedax_x = amplitude
    linkedax_y = ampl_rs[f"{amplitude:.2f}"]
    mape = ampl_mape[f"{amplitude:.2f}"]
    ax.text(
        0.58, 0.77, f"MAPE = {mape:.2f}%\nr = {linkedax_y:.2f}", transform=ax.transAxes
    )
    linkedax.scatter(linkedax_x, linkedax_y, marker="o", s=12, color="k", zorder=4)
    if linked_dir == "bottom":
        ax_x, ax_y = 0.2, 0
    if linked_dir == "top":
        ax_x, ax_y = 0.8, 1

    con = ConnectionPatch(
        xyA=(ax_x, ax_y),
        xyB=(linkedax_x, linkedax_y),
        coordsA="axes fraction",
        coordsB="data",
        axesA=ax,
        axesB=linkedax,
        linewidth=0.4,
        color = '#808080'
    )
    ax.add_artist(con)
    return line_ktrue, line_kfilter


if __name__ == "__main__":
    toggle = False
    if toggle:
        run_models("./test/output/1stMiyunModel-Range-Amplitude.xlsx", 0, 1, 0.01)
    amplitudes, rs, c_mapes = read_r_and_mape()
    fig, axes = create_figure()
    (line_mape,) = axes["MAPE"].plot(amplitudes, c_mapes, color="#808080", lw=1.5)
    (line_r,) = axes["R"].plot(amplitudes, rs, color="#9ACD32", lw=1.5)
    ampli_rs = {f"{amplitude:.2f}": r for amplitude, r in zip(amplitudes, rs)}
    ampli_mape = {
        f"{amplitude:.2f}": c_mape for amplitude, c_mape in zip(amplitudes, c_mapes)
    }
    axes["MAPE"].set_ylim(-2, 30)
    axes["R"].set_ylim(0, 1)
    axes["MAPE"].set_xlim(0, 1)
    axes["MAPE"].set_xticks(np.arange(0, 1.05, 0.05))
    axes["MAPE"].set_xlabel("Amplitude")
    axes["MAPE"].set_title("  (g)", loc="left", y=0.8)
    axes["MAPE"].set_ylabel("MAPE (%)")
    axes["R"].set_ylabel("r")

    plot_k_curve(axes["1"], "a", 0.01, ampli_rs, ampli_mape, axes["R"], "bottom")
    axes["1"].set_ylabel("k (1/d)")
    plot_k_curve(axes["2"], "b", 0.03, ampli_rs, ampli_mape, axes["R"], "bottom")
    plot_k_curve(axes["3"], "c", 0.15, ampli_rs, ampli_mape, axes["R"], "bottom")
    plot_k_curve(axes["4"], "d", 0.35, ampli_rs, ampli_mape, axes["R"], "top")
    axes["4"].set_ylabel("k (1/d)")
    plot_k_curve(axes["5"], "e", 0.60, ampli_rs, ampli_mape, axes["R"], "top")
    line_ktrue, line_kfilter = plot_k_curve(
        axes["6"], "f", 0.85, ampli_rs, ampli_mape, axes["R"], "top"
    )
    # axes["6"].set_ylim(0, 0.008)
    axes["R"].legend(
        [line_mape, line_r, line_ktrue, line_kfilter],
        ["MAPE", "r", "$\mathrm{k_{true}}$", "$\mathrm{k_{filter}}$"],
        frameon=False,
        ncols = 2,
        loc ='upper right',
        columnspacing = 1
    )
    axes['4'].set_xlabel('Day')
    axes['5'].set_xlabel('Day')
    axes['6'].set_xlabel('Day')
    fig.align_ylabels_coords([axes["1"], axes["4"]], -0.15, 0.5)
    fig.align_ylabels_coords([axes['MAPE']], -0.046, 0.5)
    fig.show()
