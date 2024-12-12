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


def RE(c_true, c_simulate):
    return np.mean(np.abs((c_simulate - c_true) / (c_true + 1e-12))) * 100


def R2(c_true, c_simulate):
    return np.corrcoef(c_true, c_simulate)[0, 1]


class DataGen:
    """
    人工生成数据代码
    """

    def __init__(self, k20, theta_deni, T_cdeni, c_oxo_deni, c_oxc_deni, beta):
        np.random.seed(100)
        self.k20 = k20
        self.theta_deni = theta_deni
        self.T_cdeni = T_cdeni
        self.c_oxo_deni = c_oxo_deni
        self.c_oxc_deni = c_oxc_deni
        self.beta = beta

    def func_temp(self, temperature, theta, critical_temp):
        mask = temperature >= critical_temp
        res = np.zeros_like(temperature)
        res[mask] = np.power(theta, temperature[mask] - 20)
        return res

    def func_oxygen(self, oxygen, opt_oxygen, critic_oxygen, beta):
        res = np.zeros_like(oxygen)
        mask_less = oxygen < opt_oxygen
        mask_mid = np.logical_and(oxygen >= opt_oxygen, oxygen <= critic_oxygen)
        res[mask_less] = 1
        res[mask_mid] = (critic_oxygen - oxygen[mask_mid]) / (
            (critic_oxygen - opt_oxygen)
            + (np.exp(beta) - np.e) * (oxygen[mask_mid] - opt_oxygen)
        )
        return res

    def add_noise(self, c_true, re):
        return c_true * (1 + np.random.normal(0, (re / 3) ** 2, c_true.shape))

    def read_data(self, filename):
        data = pd.read_excel(filename)
        oxygen = data["DO"]
        temperature = data["T"]
        return oxygen, temperature

    def calc_parameter_k(self, filename):
        oxygen, temperature = self.read_data(filename)
        ft = self.func_temp(temperature, self.theta_deni, self.T_cdeni)
        fdo = self.func_oxygen(oxygen, self.c_oxo_deni, self.c_oxc_deni, self.beta)
        return self.k20 * ft * fdo

    def get_c_k(self, filename, c0=1.0, dt=1, re=0.015):
        k = self.calc_parameter_k(filename)
        c = [c0]
        for kt in k:
            c.append(c[-1] * np.exp(-kt * dt))
        c_true = np.array(c[:-1])
        c_obs = self.add_noise(c_true, re)
        return k, c_true, c_obs


class FilterErrors:
    def __init__(
        self,
        dg,
        file_input="./test/data/1stMiyunParameterTracker_error.xlsx",
        file_out="./test/output/1stMiyunParameterTracker_out_error.xlsx",
    ):
        self.dg = dg  # DataGen对象
        self.file_input = file_input
        self.file_out = file_out

    def update_amplitude(self, amplitude):
        xls = pd.ExcelFile(self.file_input)
        sheet_names = xls.sheet_names  # 获取所有工作表名称
        sheets_dict = {
            sheet_name: pd.read_excel(
                self.file_input, sheet_name=sheet_name, header=None
            )
            for sheet_name in sheet_names
        }
        sheet_name_to_modify = "Params"  # 要修改的工作表名称
        row_index = 7  # 行索引
        col_index = 1  # 列索引
        sheets_dict[sheet_name_to_modify].iat[row_index, col_index] = amplitude
        with pd.ExcelWriter(self.file_input, engine="openpyxl") as writer:
            for sheet_name, df in sheets_dict.items():
                df.to_excel(writer, sheet_name=sheet_name, index=False, header=False)

    def update_obs(self, re, T_DO_file="test/data/TemperatureAndDO.xlsx"):
        xls = pd.ExcelFile(self.file_input)
        sheet_names = xls.sheet_names
        sheets_dict = {
            sheet_name: pd.read_excel(
                self.file_input, sheet_name=sheet_name, header=None
            )
            for sheet_name in sheet_names
        }
        k, c_true, c_obs = self.dg.get_c_k(filename=T_DO_file, c0=1.0, dt=1, re=re)
        sheets_dict["Origin_Data"].iloc[1:, 0] = np.array(range(1, k.size + 1))
        sheets_dict["Origin_Data"].iloc[1:, 1] = k
        sheets_dict["Origin_Data"].iloc[1:, 2] = c_true
        sheets_dict["Origin_Data"].iloc[1:, 3] = c_obs
        sheets_dict["Origin_Data"].iat[0, 0] = "t"
        sheets_dict["Origin_Data"].iat[0, 1] = "k"
        sheets_dict["Origin_Data"].iat[0, 2] = "c_true"
        sheets_dict["Origin_Data"].iat[0, 3] = "c_obs"
        sheets_dict["Obs"].iloc[1:, 1] = c_obs
        sheets_dict["Obs"].iat[0, 1] = "c_obs"
        with pd.ExcelWriter(self.file_input, engine="openpyxl") as writer:
            for sheet_name, df in sheets_dict.items():
                df.to_excel(writer, sheet_name=sheet_name, index=False, header=False)

    def run_model(self, exe="bin/1stOrderReactionParameterTracker", toggle=True):
        cwd = os.getcwd()
        os.chdir(os.path.abspath("build"))
        if toggle:
            subprocess.run(
                [
                    exe,
                    "-i",
                    "../test/data/1stMiyunParameterTracker_error.xlsx",
                    "-o",
                    "../test/output/1stMiyunParameterTracker_out_error.xlsx",
                ]
            )
        os.chdir(cwd)

    def read_filter_status(self):
        data = pd.read_excel(self.file_out, sheet_name="X")
        data_prior = pd.read_excel(self.file_out, sheet_name="prior_X")
        x = data["x"].values
        prior_x = data_prior.iloc[:, 1]
        k = data["v"].values
        dk = data["dv"].values
        ddk = data["ddv"].values
        return x, k, dk, prior_x, ddk

    def read_filter_variance(self):
        data = pd.read_excel(self.file_out, sheet_name="P")
        x = data["x"].values
        k = data["v"].values
        return x, k

    def read_true_value(self):
        data = pd.read_excel(self.file_input, sheet_name="Origin_Data")
        c_obs = data["c_obs"]
        k = data["k"]
        c_true = data["c_true"]
        t = data["t"]
        return t, k, c_true, c_obs

    def run_model_with_re(self, re):
        self.update_obs(re)
        self.run_model(toggle=True)
        c_filter, k_filter, dk, prior_c, ddk = self.read_filter_status()
        c_variance, k_variance = self.read_filter_variance()
        t, k_true, c_true, c_obs = self.read_true_value()
        df = pd.DataFrame()
        df["t"] = t
        df["c_filter"] = c_filter
        df["c_obs"]  = c_obs
        df["c_true"] = c_true
        df["k_filter"] = k_filter
        df["k_true"] = k_true
        df["dk"] = dk
        df["prior_c"] = prior_c
        df["ddk"] = ddk
        df["c_variance"] = c_variance
        df["k_variance"] = k_variance
        return re, df

    def run_models(
        self,
        save_path="./test/output/1stMiyunModel-Range-Error.xlsx",
        res=[0.01, 0.02, 0.05, 0.10, 0.15, 0.20, 0.25, 0.30],
    ):
        sheets_dict = {}
        for re in res:
            number, df = self.run_model_with_re(re)
            sheets_dict[f"re-{number:.2f}"] = df
        with pd.ExcelWriter(save_path, engine="openpyxl") as writer:
            for sheet_name, df in sheets_dict.items():
                df.to_excel(writer, sheet_name=sheet_name, index=False, header=True)


def create_figure():
    fig = Figure(15.7, 7)
    fig.add_axes_cm('c', 1, 1, 6, 4)
    fig.add_axes_cm('k', 8, 1, 6, 4)
    return fig, fig.axes_dict


if __name__ == "__main__":
    fig, axes = create_figure()
    dg = DataGen(
        k20=0.0038, theta_deni=1.11, T_cdeni=-2, c_oxo_deni=3.0, c_oxc_deni=10.0, beta=1
    )
    fe = FilterErrors(dg)
    # plt.plot(range(k.size), c_obs, "ro", markersize=2)
    re, df = fe.run_model_with_re(0.05)
    fe.update_amplitude(0.175)
    axes['k'].plot(df['t'], df['k_true'], lw=1, color='b')
    axes['k'].plot(df['t'], df['k_filter'], lw=0.5, color='r')
    axes['c'].plot(df['t'], df['c_true'], lw=1, color='b')
    axes['c'].plot(df['t'], df['c_filter'], lw=0.5, color='r')
    axes['c'].plot(df['t'], df['c_obs'], 'go', markersize=2)
    print(RE(df['c_true'].array, df['c_filter'].array))
    # print(RE(df['c_true'].array, df['c_obs'].array))
    print(R2(df['k_true'].array, df['k_filter'].array))
    fig.show()
