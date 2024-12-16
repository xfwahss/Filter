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
                ]
            )
        os.chdir(cwd)


def R2(obs, simu):
    data = np.hstack([obs.reshape(-1, 1), simu.reshape(-1, 1)])
    mask = ~np.any(np.isnan(data), axis=1)
    data = data[mask]
    return np.corrcoef(data[:, 0], data[:, 1])[0, 1]


def RMSE(obs, simu):
    data = np.hstack([obs.reshape(-1, 1), simu.reshape(-1, 1)])
    mask = ~np.any(np.isnan(data), axis=1)
    data = data[mask]
    return np.sqrt(np.mean(np.square(data[:, 0] - data[:, 1])))


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
    data_origin = pd.read_excel("test/data/ModelIntegrate.xlsx", sheet_name="ResAvg")
    data_filter = pd.read_excel("test/output/ModelIntegrate_out.xlsx", sheet_name="X")

    fig = figure.Figure()
    # ax = fig.add_subplot(211)
    # ax2 = fig.add_subplot(212)
    # t = data_origin['Date']
    wl, cno, cna, cnn = (
        data_origin["Water_Level"].values,
        data_origin["Res_Cno"].values,
        data_origin["Res_Cna"].values,
        data_origin["Res_Cnn"].values,
    )
    wl_filter, cno_filter, cna_filter, cnn_filter = (
        data_filter["wl"].values,
        data_filter["c_rpon"].values
        + data_filter["c_lpon"].values
        + data_filter["c_don"].values,
        data_filter["c_na"].values,
        data_filter["c_nn"].values,
    )
    # ax.plot(t, cnn, 'ro', markersize=2)
    # ax.plot(t, cnn_updated, 'b-', lw=1)
    # ax2.plot(t, cna, 'ro', markersize=2)
    # ax2.plot(t, cna_updated, 'b-', lw=1)
    # print(R2(cna.values, cna_updated.values))
    # print(RMSE(cna.values, cna_updated.values))
    ax = fig.add_subplot(111)
    taylor_plot(fig, ax, cnn, cnn_filter, markers=["o", "o"])
    # taylor_plot(fig, ax, cna, cna_filter, markers=["o", "v"])
    # taylor_plot(fig, ax, wl, wl_filter, markers=["o", "s"])
    # taylor_plot(fig, ax, cno, cno_filter, markers=["o", "s"])
    fig.show()
