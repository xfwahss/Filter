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
    "--rnit0": 0.0026, #0.0026
    "--knit20": 0.038, # 0.038
    "--foxmin": 0.1,
    "--c_oxc_nit": 4.5,
    "--c_oxo_nit": 15,
    "--theta_nit": 1.018,
    "--T_c_nit": 4.5,
    "--alpha": 0.0,
    # 反硝化过程参数
    "--b_nit_flux": 0.00,
    "--rdeni0": 0.0033, # 0.0033
    "--kdeni20": 0.021,#0.021
    "--Tc_deni": -2,
    "--theta_deni": 1.11,
    "--c_oxc_deni": 10.0,
    "--c_oxo_deni": 3.0,
    "--beta": 1,
    # 其他参数
    "--h0": 122.0,
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

def r2(obs, simu):
    obs, simu = skip_na(obs, simu)
    return np.corrcoef(obs, simu)[0, 1]

def read_observation(name):
    obs_dataset = pd.read_excel(
        "test/data/predict/6-0ResModel-Predict-{name}.xlsx".format(name=name),
        sheet_name="ResAverage",
    )
    obs_range = slice(1, 368, 1)
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
    obs = [
        obs_wl[obs_range].values,
        obs_T_init[obs_range].values,
        obs_DO_init[obs_range].values,
        obs_Con[obs_range].values,
        obs_Can[obs_range].values,
        obs_Cnn[obs_range].values,
        obs_T[obs_range].values,
        obs_DO[obs_range].values,
        index[obs_range].values,
    ]
    return obs


def read_simulation(name):
    simu_c = pd.read_excel("test/output/6-0ResModel-Predict-{name}-OUT.xlsx".format(name=name), sheet_name="Simulation")
    calibrate_range = slice(1, 368, 1)
    wl_simu = simu_c.iloc[:, 0].values
    c_rpon = simu_c.iloc[:, 1].values
    c_lpon = simu_c.iloc[:, 2].values
    c_on = simu_c.iloc[:, 3].values + c_rpon + c_lpon
    c_na = simu_c.iloc[:, 4].values
    c_nn = simu_c.iloc[:, 5].values
    simu = [
        wl_simu[calibrate_range],
        c_rpon[calibrate_range],
        c_lpon[calibrate_range],
        c_on[calibrate_range],
        c_na[calibrate_range],
        c_nn[calibrate_range],
    ]
    return simu

def vplot(ax, start_date, end_date, data_obs, data_simu, ftype):
    x = pd.date_range(start=start_date, end=end_date)
    if ftype == 'WL':
        obs_wl = data_obs[0][:x.size]
        simu_wl = data_simu[0][:x.size]
        ax.plot(x, obs_wl, 'r-')
        ax.plot(x, simu_wl, 'b-')
        rmsev = rmse(obs_wl, simu_wl)
        mapev = mape(obs_wl, simu_wl)
        r2v = r2(obs_wl, simu_wl)
        ax.text(0.1, 0.5, fr'RMSE={rmsev:.2f}, MAPE={mapev:.2f}%, $\rho$={r2v:.2f}', transform=ax.transAxes)
        ax.set_ylim(130, 150)
        ax.set_ylabel('WL (m)')
        ax.xaxis.set_visible(False)
    if ftype == 'ON':
        obs_on = data_obs[3][:x.size]
        simu_on = data_simu[3][:x.size]
        ax.plot(x, obs_on, 'ro')
        ax.plot(x, simu_on, 'b-')
        rmsev = rmse(obs_on, simu_on)
        mapev = mape(obs_on, simu_on)
        r2v = r2(obs_on, simu_on)
        ax.text(0.1, 0.5, fr'RMSE={rmsev:.2f}, MAPE={mapev:.2f}%, $\rho$={r2v:.2f}', transform=ax.transAxes)
        ax.set_ylim(0, 0.5)
        ax.set_ylabel('ON (mg/L)')
        ax.xaxis.set_visible(False)
    
    if ftype == 'AN':
        obs_an = data_obs[4][:x.size]
        simu_an = data_simu[4][:x.size]
        ax.plot(x, data_obs[4][:x.size], 'ro')
        ax.plot(x, data_simu[4][:x.size], 'b-')
        rmsev = rmse(obs_an, simu_an)
        mapev = mape(obs_an, simu_an)
        r2v = r2(obs_an, simu_an)
        ax.text(0.1, 0.5, fr'RMSE={rmsev:.2f}, MAPE={mapev:.2f}%, $\rho$={r2v:.2f}', transform=ax.transAxes)
        ax.set_ylim(0, 0.5)
        ax.set_ylabel('AN (mg/L)')
        ax.xaxis.set_visible(False)
    
    if ftype == 'NN':
        obs_nn = data_obs[5][:x.size]
        simu_nn = data_simu[5][:x.size]
        ax.plot(x, data_obs[5][:x.size], 'ro')
        ax.plot(x, data_simu[5][:x.size], 'b-')
        rmsev = rmse(obs_nn, simu_nn)
        mapev = mape(obs_nn, simu_nn)
        r2v = r2(obs_nn, simu_nn)
        ax.text(0.1, 0.5, fr'RMSE={rmsev:.2f}, MAPE={mapev:.2f}%, $\rho$={r2v:.2f}', transform=ax.transAxes)
        ax.set_ylim(0., 2)
        ax.set_ylabel('NN (mg/L)')
        ax.set_xlabel('Date')



if __name__ == "__main__":
    # 率定测试
    run_model(
        exe_path="build/bin/0DReservoirModel",
        filein="test/data/6-0ResModel-Predict.xlsx",
        fileout="test/output/6-0ResModel-PredictOUT.xlsx",
        params=params_dict,
        id=0,
        toggle=False
    )
    run_model(
        exe_path="build/bin/0DReservoirModel",
        filein="test/data/predict/6-0ResModel-Predict-2017.xlsx",
        fileout="test/output/6-0ResModel-Predict-2017-OUT.xlsx",
        params=params_dict,
        id=0,
        toggle=False
    )

    run_model(
        exe_path="build/bin/0DReservoirModel",
        filein="test/data/predict/6-0ResModel-Predict-2018.xlsx",
        fileout="test/output/6-0ResModel-Predict-2018-OUT.xlsx",
        params=params_dict,
        id=0,
        toggle=False
    )

    run_model(
        exe_path="build/bin/0DReservoirModel",
        filein="test/data/predict/6-0ResModel-Predict-2019.xlsx",
        fileout="test/output/6-0ResModel-Predict-2019-OUT.xlsx",
        params=params_dict,
        id=0,
        toggle=False
    )


    run_model(
        exe_path="build/bin/0DReservoirModel",
        filein="test/data/predict/6-0ResModel-Predict-2020.xlsx",
        fileout="test/output/6-0ResModel-Predict-2020-OUT.xlsx",
        params=params_dict,
        id=0,
        toggle=False
    )


    fig = figure.Figure(10, 10)
    ax1 = fig.add_subplot(411)
    ax2 = fig.add_subplot(412)
    ax3 = fig.add_subplot(413)
    ax4 = fig.add_subplot(414)
    data_obs = read_observation(2017)
    data_simu = read_simulation(2017)
    vplot(ax1, '2017-01-01', '2017-12-31', data_obs, data_simu, 'WL')
    vplot(ax2, '2017-01-01', '2017-12-31', data_obs, data_simu, 'ON')
    vplot(ax3, '2017-01-01', '2017-12-31', data_obs, data_simu, 'AN')
    vplot(ax4, '2017-01-01', '2017-12-31', data_obs, data_simu, 'NN')
    fig.show()


    fig2 = figure.Figure(10, 10)
    ax11 = fig2.add_subplot(411)
    ax21 = fig2.add_subplot(412)
    ax31 = fig2.add_subplot(413)
    ax41 = fig2.add_subplot(414)
    data_obs = read_observation(2018)
    data_simu = read_simulation(2018)
    vplot(ax11, '2018-01-01', '2018-12-31', data_obs, data_simu, 'WL')
    vplot(ax21, '2018-01-01', '2018-12-31', data_obs, data_simu, 'ON')
    vplot(ax31, '2018-01-01', '2018-12-31', data_obs, data_simu, 'AN')
    vplot(ax41, '2018-01-01', '2018-12-31', data_obs, data_simu, 'NN')
    fig2.show()


    fig3 = figure.Figure(10, 10)
    ax13 = fig3.add_subplot(411)
    ax23 = fig3.add_subplot(412)
    ax33 = fig3.add_subplot(413)
    ax43 = fig3.add_subplot(414)
    data_obs = read_observation(2019)
    data_simu = read_simulation(2019)
    vplot(ax13, '2019-01-01', '2019-12-31', data_obs, data_simu, 'WL')
    vplot(ax23, '2019-01-01', '2019-12-31', data_obs, data_simu, 'ON')
    vplot(ax33, '2019-01-01', '2019-12-31', data_obs, data_simu, 'AN')
    vplot(ax43, '2019-01-01', '2019-12-31', data_obs, data_simu, 'NN')
    fig3.show()


    fig4 = figure.Figure(10, 10)
    ax14 = fig4.add_subplot(411)
    ax24 = fig4.add_subplot(412)
    ax34 = fig4.add_subplot(413)
    ax44 = fig4.add_subplot(414)
    data_obs = read_observation(2020)
    data_simu = read_simulation(2020)
    vplot(ax14, '2020-01-01', '2020-12-30', data_obs, data_simu, 'WL')
    vplot(ax24, '2020-01-01', '2020-12-30', data_obs, data_simu, 'ON')
    vplot(ax34, '2020-01-01', '2020-12-30', data_obs, data_simu, 'AN')
    vplot(ax44, '2020-01-01', '2020-12-30', data_obs, data_simu, 'NN')
    fig4.show()
