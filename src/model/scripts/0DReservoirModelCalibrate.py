import sys
sys.path.append("D:/Gitlocal/plotstyles")
from plotstyles.figure import Figure
import pandas as pd
from plotstyles.fonts import global_fonts
import model_tool
import subprocess
import time
from matplotlib import pyplot as plt
plt.rcParams["font.size"] = 7

if __name__ == "__main__":
    start_time = time.time()
    b_ndo_flux = 0.030 # 必须有释放，要不然浓度不合理，先把反应置0找一个合适的释放通量
    b_amm_flux = 0.000
    b_nit_flux = 0

    s_nrp = 0.01
    s_nlp = 0.01
    alpha_rpon = 0.5
    alpha_lpon = 0.3

    h0 = 100

    k_rpon = 0.001
    theta_rpon = 1.1
    k_lpon = 0.001
    theta_lpon = 1.1
    k_don = 0.05
    theta_don = 1.1

    rnit0 = 0.0005
    knit20 = 0.005
    foxmin = 0.5 
    c_oxc_nit = 4
    c_oxo_nit = 6.5
    theta_nit = 1.09
    T_c_nit = -0.3
    alpha = 0

    rdeni0 = 0.001
    k_deni_20 = 0.1
    Tc_deni = 15
    theta_deni = 1
    c_oxc_deni = 8
    c_oxo_deni = 2
    beta = 1


    idn = "0"
    run = True
    if (run is True):
        subprocess.run(
            [
                "build/bin/0DReservoirModel",
                "-i",
                "test/data/0DReservoirModelCalibrate.xlsx",
                "-o",
                "test/output/0DReservoirModelCalibrate_output.xlsx",
                "--id",
                f"{idn}",
                "--k_rpon",
                f"{k_rpon}",
                "--theta_rpon",
                f"{theta_rpon}",
                "--k_lpon",
                f"{k_lpon}",
                "--theta_lpon",
                f"{theta_lpon}",
                "--k_don",
                f"{k_don}",
                "--theta_don",
                f"{theta_don}",
                "--rnit0",
                f"{rnit0}",
                "--knit20",
                f"{knit20}",
                "--foxmin",
                f"{foxmin}",
                "--c_oxc_nit",
                f"{c_oxc_nit}",
                "--c_oxo_nit",
                f"{c_oxo_nit}",
                "--theta_nit",
                f"{theta_nit}",
                "--T_c_nit",
                f"{T_c_nit}",
                "--alpha",
                f"{alpha}",
                "--rdeni0",
                f"{rdeni0}",
                "--kdeni20",
                f"{k_deni_20}",
                "--Tc_deni",
                f"{Tc_deni}",
                "--theta_deni",
                f"{theta_deni}",
                "--c_oxc_deni",
                f"{c_oxc_deni}",
                "--c_oxo_deni",
                f"{c_oxo_deni}",
                "--beta",
                f"{beta}",
                "--b_ndo_flux",
                f"{b_ndo_flux}",
                "--b_amm_flux",
                f"{b_amm_flux}",
                "--b_nit_flux",
                f"{b_nit_flux}",
                "--s_nrp",
                f"{s_nrp}",
                "--s_nlp",
                f"{s_nlp}",
                "--alpha_rpon",
                f"{alpha_rpon}",
                "--alpha_lpon",
                f"{alpha_lpon}",
                "--h0",
                f"{h0}",
            ]
        )
    end_time = time.time()
    print(f"Times used:{end_time-start_time:.2f}")
    print("Plot Figure")

    simu = pd.read_excel(
        "test/output/0DReservoirModelCalibrate_output.xlsx", sheet_name="Simulation"
    )
    obs = pd.read_excel(
        f"test/temp/{idn}0DReservoirModelCalibrate.xlsx", sheet_name="ResAverage"
    )

    t = obs["Date"].values
    wl_simu = simu.iloc[:, 0].values
    c_rpon_simu = simu.iloc[:, 1].values
    c_lpon_simu = simu.iloc[:, 2].values
    c_don_simu = simu.iloc[:, 3].values
    cno_simu = simu.iloc[:, 1].values + simu.iloc[:, 2].values + simu.iloc[:, 3].values
    cna_simu = simu.iloc[:, 4].values
    cnn_simu = simu.iloc[:, 5].values
    T_simu = simu.iloc[:, 6].values
    do_simu = simu.iloc[:, 7].values

    wl_obs = obs["Water_Level"].values
    Res_Cno_obs = obs["Res_Cno"].values
    Res_Cna_obs = obs["Res_Cna"].values
    Res_Cnn_obs = obs["Res_Cnn"].values
    Res_T_obs = obs["Res_T_True"].values
    Res_DO_obs = obs["Res_DO_True"].values



    init_step = 0
    # steps = 2193
    steps = 366
    fig = Figure(10, 16)
    fig.add_axes_cm("water level", 1.5, 0.5, 8, 2, "left upper")
    fig.add_axes_cm("Org-N", 1.5, 3.0, 8, 2, "left upper")
    fig.add_axes_cm("NH-N", 1.5, 5.5, 8, 2, "left upper")
    fig.add_axes_cm("NO-N", 1.5, 8.0, 8, 2, "left upper")
    fig.add_axes_cm("T", 1.5, 10.5, 8, 2, "left upper")
    fig.add_axes_cm("DO", 1.5, 13.0, 8, 2, "left upper")
    axes = fig.axes_dict

    simu_range = slice(init_step, init_step + steps)
    axes["water level"].plot(
        t[simu_range], wl_obs[simu_range], "r", label="Observation", lw=1
    )
    axes["water level"].plot(t[simu_range], wl_simu, "b", label="Simulation", lw=1)
    axes["water level"].text(
        0.4,
        0.1,
        f"RMSE={model_tool.rmse(wl_simu, wl_obs[simu_range]):.2f}",
        transform=axes["water level"].transAxes,
    )
    axes["water level"].text(
        0.6,
        0.1,
        f"MAPE={model_tool.mape(wl_simu, wl_obs[simu_range]):.2f}%",
        transform=axes["water level"].transAxes,
    )
    axes["water level"].text(
        0.8,
        0.1,
        f"NSE={model_tool.nse(wl_simu, wl_obs[simu_range]):.4f}",
        transform=axes["water level"].transAxes,
    )
    axes["water level"].set_ylim(130, 140)
    axes["water level"].set_ylabel("Water level (m)")
    axes["water level"].legend(frameon=False)

    axes["Org-N"].plot(
        t[simu_range],
        Res_Cno_obs[simu_range],
        color="r",
        label="Observation",
        lw=0,
        marker="o",
        markersize=1,
    )
    axes["Org-N"].plot(t[simu_range], cno_simu, color="b", label="Simulation", lw=1)
    axes["Org-N"].plot(t[simu_range], c_lpon_simu, color='orange', label='labile organic nitrogen', lw=1)
    axes["Org-N"].plot(t[simu_range], c_rpon_simu, color='g', label='refactory organic nitrogen', lw=1)
    axes["Org-N"].plot(t[simu_range], c_don_simu, color='k', label='dissolved organic nitrogen', lw=1)
    # axes["Org-N"].set_ylim(0, 1)
    axes["Org-N"].set_ylabel("Organic nitrogen (mg/L)")
    axes["Org-N"].text(
        0.4,
        0.8,
        f"RMSE={model_tool.rmse(cno_simu, Res_Cno_obs[simu_range]):.2f}",
        transform=axes["Org-N"].transAxes,
    )
    axes["Org-N"].text(
        0.6,
        0.8,
        f"MAPE={model_tool.mape(cno_simu, Res_Cno_obs[simu_range]):.2f}%",
        transform=axes["Org-N"].transAxes,
    )
    axes["Org-N"].text(
        0.8,
        0.8,
        f"NSE={model_tool.nse(cno_simu, Res_Cno_obs[simu_range]):.4f}",
        transform=axes["Org-N"].transAxes,
    )
    axes["Org-N"].legend(frameon=False, fontsize=6, loc='upper left')

    axes["NH-N"].plot(
        t[simu_range],
        Res_Cna_obs[simu_range],
        color="r",
        label="Observation",
        lw=0,
        marker="o",
        markersize=1,
    )
    axes["NH-N"].plot(t[simu_range], cna_simu, color="b", label="Simulation", lw=1)
    # axes["NH-N"].set_ylim(0, 1)
    axes["NH-N"].set_ylabel("Ammonia nitrogen (mg/L)")
    axes["NH-N"].text(
        0.4,
        0.8,
        f"RMSE={model_tool.rmse(cna_simu, Res_Cna_obs[simu_range]):.2f}",
        transform=axes["NH-N"].transAxes,
    )
    axes["NH-N"].text(
        0.6,
        0.8,
        f"MAPE={model_tool.mape(cna_simu, Res_Cna_obs[simu_range]):.2f}%",
        transform=axes["NH-N"].transAxes,
    )
    axes["NH-N"].text(
        0.8,
        0.8,
        f"NSE={model_tool.nse(cna_simu, Res_Cna_obs[simu_range]):.4f}",
        transform=axes["NH-N"].transAxes,
    )

    axes["NO-N"].plot(
        t[simu_range],
        Res_Cnn_obs[simu_range],
        color="r",
        label="Observation",
        lw=0,
        marker="o",
        markersize=1,
    )
    axes["NO-N"].plot(t[simu_range], cnn_simu, color="b", label="Simulation", lw=1)
    # axes["NO-N"].set_ylim(0, 1)
    axes["NO-N"].set_ylabel("Nitrate nitrogen (mg/L)")
    axes["NO-N"].text(
        0.4,
        0.8,
        f"RMSE={model_tool.rmse(cnn_simu, Res_Cnn_obs[simu_range]):.2f}",
        transform=axes["NO-N"].transAxes,
    )
    axes["NO-N"].text(
        0.6,
        0.8,
        f"MAPE={model_tool.mape(cnn_simu, Res_Cnn_obs[simu_range]):.2f}%",
        transform=axes["NO-N"].transAxes,
    )
    axes["NO-N"].text(
        0.8,
        0.8,
        f"NSE={model_tool.nse(cnn_simu, Res_Cnn_obs[simu_range]):.4f}",
        transform=axes["NO-N"].transAxes,
    )



    axes["T"].plot(
        t[simu_range],
        Res_T_obs[simu_range],
        color="r",
        label="Observation",
        lw=0,
        marker="o",
        markersize=1,
    )
    axes["T"].plot(t[simu_range], T_simu, color="b", label="Simulation", lw=1)


    axes["DO"].plot(
        t[simu_range],
        Res_DO_obs[simu_range],
        color="r",
        label="Observation",
        lw=0,
        marker="o",
        markersize=1,
    )
    axes["DO"].plot(t[simu_range], do_simu, color="b", label="Simulation", lw=1)

    fig.show()
