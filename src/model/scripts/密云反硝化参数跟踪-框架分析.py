import numpy as np
from plotstyles.fonts import global_fonts
from plotstyles.figure import Figure
from plotstyles import styles
import pandas as pd
import subprocess
import os
import model_tool

styles.use("wr")


# 由模拟生成密云水库的反硝化数据，进行参数的修正
def simulate(c0, k=0.0008, dt=1):
    c = [c0]
    for _ in range(0, 730):
        c.append(c[-1] * np.exp(-k * dt))
    return np.array(c)


def re(c_true, c_simulate):
    return np.mean(np.abs((c_simulate - c_true) / (c_true + 1e-12))) * 100


def r2(c_true, c_simulate):
    return np.corrcoef(c_true, c_simulate)[0, 1]


def rmse(c_true, c_simulate):
    return np.sqrt(np.mean((c_true - c_simulate) ** 2))


def best_simulate(ctrue, c0=1, dt=1, start=0, end=0.0041, step=0.0001):
    k_arr = np.arange(start, end, step)
    rmse_arr = []
    for k in k_arr:
        c_simu = simulate(c0, k, dt)
        rmse_arr.append(rmse(c_true, c_simu))
    return k_arr, np.array(rmse_arr)


def run_model(exe, toggle=True):
    cwd = os.getcwd()
    os.chdir(os.path.abspath("build"))
    if toggle:
        subprocess.run(
            [
                exe,
                "-i",
                "../test/data/1stMiyunParameterTracker.xlsx",
                "-o",
                "../test/output/1stMiyunParameterTracker_out.xlsx",
            ]
        )
    os.chdir(cwd)


def run_model_default(exe, toggle=True):
    cwd = os.getcwd()
    os.chdir(os.path.abspath("build"))
    if toggle:
        subprocess.run(
            [
                exe,
                "-i",
                "../test/data/1stMiyunParameterTracker_default.xlsx",
                "-o",
                "../test/output/1stMiyunParameterTracker_out_default.xlsx",
            ]
        )
    os.chdir(cwd)


def read_filter_status(path="test/output/1stMiyunParameterTracker_out.xlsx"):
    data = pd.read_excel(path, sheet_name="X")
    data_prior = pd.read_excel(path, sheet_name="prior_X")
    x = data["x"].values
    prior_x = data_prior.iloc[:, 1]
    k = data["v"].values
    dk = data["dv"].values
    ddk = data["ddv"].values
    return x, k, dk, prior_x, ddk


def read_filter_variance(path="test/output/1stMiyunParameterTracker_out.xlsx"):
    data = pd.read_excel(path, sheet_name="P")
    x = data["x"].values
    k = data["v"].values
    return x, k


def create_figure():
    fig = Figure(15, 9)
    ax1 = fig.add_axes_cm("C", 1.2, 0.5, 6, 3, "left upper")
    ax12 = fig.add_axes_cm("C_sigma", 1.2, 3.5, 6, 1, "left upper")
    ax2 = fig.add_axes_cm("K", 8.5, 0.5, 6, 3, "left upper")
    ax22 = fig.add_axes_cm("K_sigma", 8.5, 3.5, 6, 1, "left upper")
    ax3 = fig.add_axes_cm("S", 1.2, 5.0, 6, 3, "left upper")
    ax4 = fig.add_axes_cm("HisS", 8.5, 5.0, 6, 3, "left upper")
    return fig, fig.axes_dict


def read_true_value(path="test/data/1stMiyunParameterTracker.xlsx"):
    data = pd.read_excel(path, sheet_name="Origin_Data")
    c_obs = data["c_obs"]
    k = data["k"]
    c_true = data["c_true"]
    t = data["t"]
    return t, k, c_true, c_obs


if __name__ == "__main__":
    xlim = (0, 800)
    run_model("bin/1stOrderReactionParameterTracker", toggle=False)
    run_model_default("bin/1stOrderReactionParameterTracker_default", toggle=False)
    c_filter, k_filter, dk, prior_c, ddk = read_filter_status()
    c_filter_default, k_filter_default, dk_fault, prior_c_default, ddk_fault = (
        read_filter_status("test/output/1stMiyunParameterTracker_out_default.xlsx")
    )
    c_variance, k_variance = read_filter_variance()
    c_variance_default, k_variance_default = read_filter_variance(
        "test/output/1stMiyunParameterTracker_out_default.xlsx"
    )
    t, k_true, c_true, c_obs = read_true_value()

    k_arr, rmse_arr = best_simulate(c_true)
    c_simu_best = simulate(1, k=0.0008, dt=1)  # 最佳拟合
    # min_rmse = rmse(c_true, c_simu)
    # print(min_rmse)
    color_true, color_simu, color_filter_i, color_filter_p = (
        "#000000",
        "#9ACD32",
        "#0000ff",
        "#808080",
    )

    fig, axes = create_figure()
    (line_simu_best,) = axes["C"].plot(
        t, c_simu_best, lw=0.75, color=color_simu, linestyle="solid"
    )
    (line_filter_initiative,) = axes["C"].plot(t, c_filter, lw=0.75, color=color_filter_i, linestyle='solid')
    (line_true,) = axes["C"].plot(t, c_true, lw=0.75, color=color_true, linestyle='dashed')
    # (line_observation,) = axes["C"].plot(t, c_obs, "ro", markersize=0.3)
    (line_filter_passive,) = axes["C"].plot(t, c_filter_default, lw=0.75, color=color_filter_p, linestyle='solid')

    # 局部放大
    axin = axes["C"].inset_axes([0.5, 0.5, 0.3, 0.3])
    axin.plot(t, c_filter, lw=0.75, color=color_filter_i, linestyle='solid')
    axin.plot(t, c_filter_default, lw=0.75, color=color_filter_p, linestyle='solid')
    axin.plot(t, c_true, lw=0.75, color=color_true, linestyle='dashed')
    # axin.plot(t, c_obs, "ro", markersize=0.3)
    axin.xaxis.set_visible(False)
    axin.yaxis.set_visible(False)
    fig.zone_and_linked(axes["C"], axin, 290, 300, t, [c_filter, c_filter_default, c_true], linked='right', x_ratio=0.1, y_ratio=0.1)

    axes["C"].set_ylim(0.5, 1.00)
    axes["C"].set_xlim(xlim)
    axes["C"].xaxis.set_visible(False)
    axes["C"].set_ylabel("Concentration (mg/L)")
    axes["C"].set_title("(a) Nitrate    ", loc="right")
    re_obs = re(c_true, c_obs)
    re_simu = re(c_true, c_simu_best)
    re_filter_p = re(c_true, c_filter_default)
    re_filter_i = re(c_true, c_filter)
    c_handle = [
        line_true,
        # line_observation,
        line_simu_best,
        line_filter_passive,
        line_filter_initiative,
    ]
    c_labels = [
        "True value",
        # "Observation",
        f"Simulation: MAPE={re_simu:.2f}%",
        f"Filter(Passive): MAPE={re_filter_p:.2f}%",
        f"Filter(Initiative): MAPE={re_filter_i:.2f}%",
    ]
    axes["C"].legend(
        handles=c_handle, labels=c_labels, frameon=False, fontsize=6, loc="lower left"
    )
    axes["C_sigma"].plot(t, c_variance, lw=0.75, color=color_filter_i)
    axes["C_sigma"].plot(t, c_variance_default, lw=0.75, color=color_filter_p)
    axes["C_sigma"].set_xlim(xlim)
    axes["C_sigma"].set_ylim(-0.0001, 0.0015)
    axes["C_sigma"].ticklabel_format(
        style="sci", scilimits=(-3, 1), axis="y", useMathText=True
    )
    axes["C_sigma"].set_ylabel("$\sigma_c^2$")
    axes["C_sigma"].set_xticklabels([])
    axes["C_sigma"].set_title("  (c)", y=0.6)

    k_simu = np.array([0.0008] * t.shape[0])
    (line_kfilter_i,) = axes["K"].plot(t, k_filter, lw=1, color=color_filter_i, linestyle='solid')
    (line_kfilter_p,) = axes["K"].plot(t, k_filter_default, lw=1, color=color_filter_p, linestyle='solid')
    (line_k_true,) = axes["K"].plot(t, k_true,  lw=1, color=color_true, linestyle='dashed')
    (line_k_simu,) = axes["K"].plot(t, k_simu, color=color_simu, lw=1, linestyle='solid')
    axes["K"].ticklabel_format(
        style="sci", scilimits=(-3, 1), axis="y", useMathText=True
    )
    r2_k_simu = r2(k_true, k_simu)
    r2_k_filter_i = r2(k_true, k_filter)
    r2_k_filter_p = r2(k_true, k_filter_default)
    k_handles = [line_k_true, line_k_simu, line_kfilter_p, line_kfilter_i]
    k_labels = [
        "$k_{true}$",
        f"$k_{{simu}}$: r={r2_k_simu:.2f}",
        f"$k_{{filter}}^{{passive}}$: r={r2_k_filter_p:.2f}",
        f"$k_{{filter}}^{{initiative}}$: r={r2_k_filter_i:.2f}",
    ]
    axes["K"].legend(
        handles=k_handles,
        labels=k_labels,
        frameon=False,
        fontsize=6,
        ncols=2,
        columnspacing=1,
        bbox_to_anchor=(0, 1.05),
        loc="upper left",
    )
    axes["K"].set_ylim(0, 0.004)
    axes["K"].set_xlim(xlim)
    axes["K"].xaxis.set_visible(False)
    axes["K"].set_title("(b) Parameter $k$  ", loc="right", y=0.85)
    axes["K"].set_ylabel("k (1/d)")

    axes["K_sigma"].plot(t, k_variance, lw=1, color=color_filter_i)
    axes["K_sigma"].plot(t, k_variance_default, lw=1, color=color_filter_p)
    axes["K_sigma"].set_xlim(xlim)
    axes["K_sigma"].ticklabel_format(
        style="sci", scilimits=(-3, 1), axis="y", useMathText=True
    )
    axes["K_sigma"].set_ylim(-1e-8, 15e-8)
    axes["K_sigma"].set_ylabel("$\sigma_{k}^2$")
    axes["K_sigma"].set_xticklabels([])
    axes["K_sigma"].set_title("  (d)", y=0.6)

    axes["HisS"].plot(t, dk, lw=0.3, color=color_filter_i)
    axes["HisS"].ticklabel_format(
        style="sci", scilimits=(-3, 1), axis="y", useMathText=True
    )
    axes["HisS"].set_ylim(-0.0003, 0.0003)
    axes["HisS"].set_ylabel(r"$\partial k / \partial t \; \mathrm{(1/d^2)}$ ")
    axes["HisS"].set_xlim(xlim)
    axes["HisS"].set_xlabel("Day")
    axes["HisS"].set_title("  (f) Model Response R(t)")

    axes["S"].plot(t, ddk, lw=0.5, color=color_filter_i)
    axes["S"].ticklabel_format(
        style="sci", scilimits=(-1, 1), axis="y", useMathText=True
    )
    axes["S"].set_ylim(-0.02, 0.02)
    axes["S"].ticklabel_format(
        style="sci", scilimits=(-3, 1), axis="y", useMathText=True
    )
    axes["S"].set_xlim(xlim)
    axes["S"].set_ylabel("$S(t)\;\mathrm{(1/d^2)}$")
    axes["S"].set_xlabel("Day")
    axes["S"].set_title("  (e) External Stimulation S(t)")

    fig.align_ylabels_coords([axes["C"], axes["C_sigma"], axes["S"]], -0.12, 0.5)
    fig.align_ylabels_coords([axes["K"], axes["K_sigma"], axes["HisS"]], -0.10, 0.5)

    # fig2 = Figure()
    # ax = fig2.add_subplot(111)
    # ax.plot(k_arr, rmse_arr)
    fig.show()
    # fig2.show()
