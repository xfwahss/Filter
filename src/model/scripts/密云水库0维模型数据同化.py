# 同化真实数据的代码, 评估真实的数据
import pandas as pd
from plotstyles import styles
from plotstyles import figure
import subprocess
import numpy as np
import os

styles.use("wr")


class Model:
    assimilation_exe = "bin/ModelIntegrate"

    def __init__(self):
        pass

    def run(self, toggle=True):
        cwd = os.getcwd()
        os.chdir(os.path.abspath("build"))
        if toggle:
            subprocess.run(
                [
                    self.assimilation_exe,
                    "-i",
                    "../test/data/ModelIntegrate.xlsx",
                    "-o",
                    "../test/output/ModelIntegrate_out.xlsx",
                    "-m",
                    "1"
                ]
            )
        os.chdir(cwd)

    def R2(self, obs, simu):
        data = np.hstack([obs.reshape(-1, 1), simu.reshape(-1, 1)])
        mask = ~np.any(np.isnan(data), axis=1)
        data = data[mask]
        return np.corrcoef(data[:, 0], data[:, 1])[0, 1]

    def RMSE(self, obs, simu):
        data = np.hstack([obs.reshape(-1, 1), simu.reshape(-1, 1)])
        mask = ~np.any(np.isnan(data), axis=1)
        data = data[mask]
        return np.sqrt(np.mean(np.square(data[:, 0] - data[:, 1])))

    def MAPE(self, obs, simu):
        data = np.hstack([obs.reshape(-1, 1), simu.reshape(-1, 1)])
        mask = ~np.any(np.isnan(data), axis=1)
        data = data[mask]
        return np.mean(np.abs((data[:, 1] - data[:, 0])) / data[:, 0]) * 100

    def read_status(self, file_origin, file_filter):
        data_origin = pd.read_excel(file_origin, sheet_name="ResAvg")
        data_filter = pd.read_excel(file_filter, sheet_name="X")
        df_dict = {
            "t": data_origin["Date"].values,
            "wl": data_origin["Water_Level"].values,
            "cno": data_origin["Res_Cno"].values,
            "cna": data_origin["Res_Cna"].values,
            "cnn": data_origin["Res_Cnn"].values,
            "wl_filter": data_filter["wl"].values,
            "cno_filter": data_filter["c_rpon"].values
            + data_filter["c_lpon"].values
            + data_filter["c_don"].values,
            "cna_filter": data_filter["c_na"].values,
            "cnn_filter": data_filter["c_nn"].values,
        }
        return pd.DataFrame(df_dict)

    def plot_status(self, fig):
        data = self.read_status(
            "test/data/ModelIntegrate.xlsx", "test/output/ModelIntegrate_out.xlsx"
        )
        fig.add_axes_cm("wl", 1, 0.5, 12, 3, "left upper")
        fig.add_axes_cm("cno", 1, 4, 12, 3, "left upper")
        fig.add_axes_cm("cna", 1, 7.5, 12, 3, "left upper")
        fig.add_axes_cm("cnn", 1, 11, 12, 3, "left upper")
        axes = fig.axes_dict
        axes["wl"].plot(
            data["t"], data["wl"], color="r", marker="o", markersize=1, lw=0
        )
        axes["wl"].plot(data["t"], data["wl_filter"], color="b", marker="o", markersize=1, lw=0.75)

        axes["cno"].plot(
            data["t"], data["cno"], color="r", marker="o", markersize=1, lw=0
        )
        axes["cno"].plot(data["t"], data["cno_filter"], color="b", lw=0.75)
        axes["cno"].text(
            0.6,
            0.7,
            f"R={self.R2(data['cno'].values, data['cno_filter'].values):.2f}\nRMSE={self.RMSE(data['cno'].values, data['cno_filter'].values):.3f}\nMAPE={self.MAPE(data['cno'].values, data['cno_filter'].values):.2f}%",
            transform=axes["cno"].transAxes,
        )

        axes["cna"].plot(
            data["t"], data["cna"], color="r", marker="o", markersize=1, lw=0
        )
        axes["cna"].plot(data["t"], data["cna_filter"], color="b", lw=0.75)
        axes["cna"].text(
            0.8,
            0.7,
            f"R={self.R2(data['cna'].values, data['cna_filter'].values):.2f}\nRMSE={self.RMSE(data['cna'].values, data['cna_filter'].values):.3f}\nMAPE={self.MAPE(data['cna'].values, data['cna_filter'].values):.2f}%",
            transform=axes["cna"].transAxes,
        )

        axes["cnn"].plot(
            data["t"], data["cnn"], color="r", marker="o", markersize=1, lw=0
        )
        axes["cnn"].plot(data["t"], data["cnn_filter"], color="b", lw=0.75)
        axes["cnn"].text(
            0.8,
            0.7,
            f"R={self.R2(data['cnn'].values, data['cnn_filter'].values):.2f}\nRMSE={self.RMSE(data['cnn'].values, data['cnn_filter'].values):.3f}\nMAPE={self.MAPE(data['cnn'].values, data['cnn_filter'].values):.2f}%",
            transform=axes["cnn"].transAxes,
        )
        fig.show()


def taylor_plot(fig, ax, obs, simu, markers=["o", "o"], markersize=5):
    data = np.hstack([obs.reshape(-1, 1), simu.reshape(-1, 1)])
    mask = ~np.any(np.isnan(data), axis=1)
    data = data[mask]
    obs, simu = data[:, 0], data[:, 1]
    df = pd.DataFrame({"True": obs, "Simu": simu})
    fig.taylor_plot(
        ax,
        df.iloc[:, 0].values,
        df.iloc[:, 1:].values,
        Normalize=True,
        markers=markers,
        markersize=markersize,
        scale=1.5,
    )


if __name__ == "__main__":
    m = Model()
    m.run(toggle=True)
    # data = m.read_status(
    #     "test/data/ModelIntegrate.xlsx", "test/output/ModelIntegrate_out.xlsx"
    # )

    fig = figure.Figure(14, 16)
    m.plot_status(fig)
    # ax = fig.add_subplot(111)
    # ax.plot(data['t'], data['cna'], 'ro', markersize=2)
    # ax.plot(data['t'], data['cna_filter'], 'b-', lw=1)
    # taylor_plot(
    #     fig, ax, data["cnn"].values, data["cnn_filter"].values, markers=["o", "o"]
    # )
    # taylor_plot(fig, ax, cna, cna_filter, markers=["o", "v"])
    # taylor_plot(fig, ax, wl, wl_filter, markers=["o", "s"])
    # taylor_plot(fig, ax, cno, cno_filter, markers=["o", "s"])
