from plotstyles.figure import Figure
import pandas as pd
from plotstyles.fonts import global_fonts
import model_tool
import subprocess

if __name__ == "__main__":
    ro0 = 543 * 1e-12    # 最大1e-9
    ko1 = 369 * 1e-8  # 最大1e-5

    ra0=123 * 1e-12 # 最大1e-9
    kab1 = 37399 * 1e-8 # 搜索1e-8~1
    foxmin = 35 * 0.01
    c_oxc = 38 * 0.1
    c_oxo = 58 * 0.1
    theta_a = 23 * 0.1
    T_c = 13 * 0.1

    rn0 = 2113 * 1e-12 # 最大1e-8
    knb1 = 1739863 * 1e-8  # knb1 < 0.1 最好大于 1e-8
    Tnc = -56 * 0.1
    theta_n = 29 * 0.1
    c_noxc = 102 * 0.1
    c_noxo = 70 * 0.1

    subprocess.run(["build/bin/1DReservoirModel", "-i", "test/data/1DReservoirModel_Origin.xlsx",
                    "-o", "test/output/1DReservoirModel_Origin_output.xlsx",
                    "--ro0", f"{ro0}",
                    "--ko1", f"{ko1}",
                    "--ra0", f"{ra0}",
                    "--kab1", f"{kab1}",
                    "--foxmin", f"{foxmin}",
                    "--c_oxc", f"{c_oxc}",
                    "--c_oxo", f"{c_oxo}",
                    "--theta_a", f"{theta_a}",
                    "--T_c", f"{T_c}",
                    "--rn0", f"{rn0}",
                    "--knb1", f"{knb1}",
                    "--Tnc", f"{Tnc}",
                    "--theta_n", f"{theta_n}",
                    "--c_noxc", f"{c_noxc}",
                    "--c_noxo", f"{c_noxo}"])

    simu = pd.read_excel("test/output/1DReservoirModel_Origin_output.xlsx", sheet_name="Simulation")
    obs = pd.read_excel("test/data/1DReservoirModel_Origin.xlsx", sheet_name="Res_Avg")

    t = obs["Date"].values
    wl_simu = simu.iloc[:, 0].values
    cno_simu = simu.iloc[:, 1].values
    cna_simu = simu.iloc[:, 2].values
    cnn_simu = simu.iloc[:, 3].values



    wl_obs = obs["Water_Level"].values
    Res_Cno_obs = obs["Res_Cno"].values
    Res_Cna_obs = obs["Res_Cna"].values
    Res_Cnn_obs = obs["Res_Cnn"].values

    init_step = 0
    # steps = 2193
    steps = 366
    fig = Figure(16, 14.5)
    fig.add_axes_cm("water level", 1.5, 0.5, 14, 2.5, "left upper")
    fig.add_axes_cm("Org-N", 1.5, 4., 14, 2.5, "left upper")
    fig.add_axes_cm("NH-N", 1.5, 7.5, 14, 2.5, "left upper")
    fig.add_axes_cm("NO-N", 1.5, 11., 14, 2.5, "left upper")
    axes = fig.axes_dict


    simu_range = slice(init_step,init_step + steps)
    axes["water level"].plot(t[simu_range], wl_obs[simu_range], 'r', label='Observation', lw=1)
    axes["water level"].plot(t[simu_range], wl_simu, 'b', label='Simulation', lw=1)
    axes["water level"].text(0.4, 0.1, f"RMSE={model_tool.rmse(wl_simu, wl_obs[simu_range]):.2f}", transform = axes["water level"].transAxes)
    axes["water level"].text(0.6, 0.1, f"MAPE={model_tool.mape(wl_simu, wl_obs[simu_range]):.2f}%", transform = axes["water level"].transAxes)
    axes["water level"].text(0.8, 0.1, f"NSE={model_tool.nse(wl_simu, wl_obs[simu_range]):.4f}", transform = axes["water level"].transAxes)
    axes["water level"].set_ylim(130, 160)
    axes["water level"].set_ylabel("Water level (m)")
    axes["water level"].legend(frameon=False)

    axes["Org-N"].plot(t[simu_range], Res_Cno_obs[simu_range], color="r", label="Observation", lw=0, marker='o', markersize=1)
    axes["Org-N"].plot(t[simu_range], cno_simu, color='b', label="Simulation", lw=1)
    # axes["Org-N"].set_ylim(0, 1)
    axes["Org-N"].set_ylabel("Organic nitrogen (mg/L)")
    axes["Org-N"].text(0.4, 0.8, f"RMSE={model_tool.rmse(cno_simu, Res_Cno_obs[simu_range]):.2f}", transform = axes["Org-N"].transAxes)
    axes["Org-N"].text(0.6, 0.8, f"MAPE={model_tool.mape(cno_simu, Res_Cno_obs[simu_range]):.2f}%", transform = axes["Org-N"].transAxes)
    axes["Org-N"].text(0.8, 0.8, f"NSE={model_tool.nse(cno_simu, Res_Cno_obs[simu_range]):.4f}", transform = axes["Org-N"].transAxes)
    
    axes["NH-N"].plot(t[simu_range], Res_Cna_obs[simu_range], color="r", label="Observation", lw=0, marker='o', markersize=1)
    axes["NH-N"].plot(t[simu_range], cna_simu, color='b', label="Simulation", lw=1)
    # axes["NH-N"].set_ylim(0, 1)
    axes["NH-N"].set_ylabel("Ammonia nitrogen (mg/L)")
    axes["NH-N"].text(0.4, 0.8, f"RMSE={model_tool.rmse(cna_simu, Res_Cna_obs[simu_range]):.2f}", transform = axes["NH-N"].transAxes)
    axes["NH-N"].text(0.6, 0.8, f"MAPE={model_tool.mape(cna_simu, Res_Cna_obs[simu_range]):.2f}%", transform = axes["NH-N"].transAxes)
    axes["NH-N"].text(0.8, 0.8, f"NSE={model_tool.nse(cna_simu, Res_Cna_obs[simu_range]):.4f}", transform = axes["NH-N"].transAxes)

    axes["NO-N"].plot(t[simu_range], Res_Cnn_obs[simu_range], color="r", label="Observation", lw=0, marker='o', markersize=1)
    axes["NO-N"].plot(t[simu_range], cnn_simu, color='b', label="Simulation", lw=1)
    # axes["NO-N"].set_ylim(0, 1)
    axes["NO-N"].set_ylabel("Nitrate nitrogen (mg/L)")
    axes["NO-N"].text(0.4, 0.8, f"RMSE={model_tool.rmse(cnn_simu, Res_Cnn_obs[simu_range]):.2f}", transform = axes["NO-N"].transAxes)
    axes["NO-N"].text(0.6, 0.8, f"MAPE={model_tool.mape(cnn_simu, Res_Cnn_obs[simu_range]):.2f}%", transform = axes["NO-N"].transAxes)
    axes["NO-N"].text(0.8, 0.8, f"NSE={model_tool.nse(cnn_simu, Res_Cnn_obs[simu_range]):.4f}", transform = axes["NO-N"].transAxes)

    fig.show()