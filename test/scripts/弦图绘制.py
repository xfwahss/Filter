from pycirclize import Circos
import pandas as pd
import numpy as np
from plotstyles.figure import Figure
from plotstyles import styles
from SALib.analyze import sobol
import matplotlib.pyplot as plt
from matplotlib.patches import Patch
from matplotlib.patches import Rectangle

styles.use("wr")


def sensitivity_analysis(df, column, v_min, v_max):
    problem = {
        "num_vars": 29,
        "names": [
            "$k_{rp20}$",
            r"$\theta_{rp}$",
            "$k_{lp20}$",
            r"$\theta_{lp}$",
            "$k_{do20}$",
            r"$\theta_{do}$",
            "$r_{nit0}$",
            "$k_{nit20}$",
            "$f_{oxmin}$",
            "$c_{oxc}^{nit}$",
            "$c_{oxo}^{nit}$",
            r"$\theta_{nit}$",
            "$T_{c}^{nit}$",
            r"$\alpha$",
            "$r_{deni0}$",
            "$k_{deni20}$",
            "$T_c^{deni}$",
            r"$\theta_{deni}$",
            "$c_{oxc}^{deni}$",
            "$c_{oxo}^{deni}$",
            r"$\beta$",
            "$B_{Ndo}$",
            "$B_{Na}$",
            "$B_{Nn}$",
            "$S_{Nrp}$",
            "$S_{Nlp}$",
            r"$\alpha_{rp}$",
            r"$\alpha_{lp}$",
            "$H$",
        ],
        "bounds": [
            [0, 0.1],
            [1, 2],
            [0, 0.1],
            [1, 2],
            [0, 0.1],
            [1, 2],
            [0, 0.1],
            [0, 0.1],
            [0, 1],
            [0, 12],
            [0, 12],
            [1, 2],
            [0, 30],
            [-1, 1],
            [0, 0.1],
            [0, 0.1],
            [0, 30],
            [1, 2],
            [0, 12],
            [0, 12],
            [-1, 2],
            [0, 0.1],
            [0, 0.1],
            [0, 0.1],
            [0, 10],
            [0, 10],
            [0, 1],
            [0, 1],
            [80, 120],
        ],
    }
    y = df.iloc[:, column].values
    y = np.clip(y, v_min, v_max)
    si = sobol.analyze(problem, y)
    ts, one_s, sec_s = si.to_df()
    row_names = problem["names"]
    col_names = problem["names"]
    sec_df = pd.DataFrame(si["S2"], index=row_names, columns=col_names)
    sec_df = sec_df.abs()
    sec_df = sec_df.fillna(0)
    sec_df[sec_df < 1e-10] = 0  # 很小的直接清除
    # print(sec_df)

    return ts, one_s, sec_df


def visilize(ax, df):
    # df 必须有列名和行名
    # Initialize Circos from matrix for plotting Chord Diagram
    # 第五大的值
    ff = np.sort(df.values, axis=None)[::-1][6]

    colors1 = plt.get_cmap("tab20")(np.linspace(0, 1, 20))
    colors2 = plt.get_cmap("tab20b")(np.linspace(0, 1, 20)[0:9])
    colors = np.vstack([colors1, colors2])
    cmaps = {n: c for n, c in zip(df.index, colors)}

    link_cmap = []
    for i, a in enumerate(df.index):
        for j, b in enumerate(df.columns):
            v = df.iat[i, j]
            if v > ff:
                link_cmap.append((a, b, "b"))
    circos = Circos.initialize_from_matrix(
        df,
        space=2,
        cmap=cmaps,
        label_kws=dict(size=0, color="none"),
        link_cmap=link_cmap,
        link_kws=dict(ec="none", lw=0.0, direction=0),
    )
    circos.plotfig(ax=ax)
    return cmaps


def plot_s(ax, x, ts, fs, width=0.15, colort="r", colorf="b"):
    width = 0.15
    x2 = x + width
    p1 = ax.bar(x, ts.iloc[:, 0], width=width, align="edge", color=colort, alpha=1)
    p2 = ax.bar(x2, fs.iloc[:, 0], width=width, align="edge", color=colorf, alpha=1)
    # ax.set_xticks(x2, ts.index, rotation=0, fontsize=7, ha="center")
    ax.set_xlim(-0.25, 29.25)
    ax.set_ylim(0, 0.6)
    ax.yaxis.set_major_formatter('{x:.2f}')
    ax.set_ylabel('Sobol Index')
    ax.set_xticklabels([])
    return p1, p2


def add_color_section(ax, cmaps):
    ax.set_xlim(-0.25, 29.25)
    ax.set_ylim(0, 1)
    ax.yaxis.set_visible(False)
    for s in ['bottom', 'right', 'left']:
        ax.spines[s].set_visible(False)
    space = 0.1
    rect = [Rectangle((i, 0), 1 - space, 1, facecolor=c, edgecolor='k', linewidth=0.5) for i, c in zip(range(29),cmaps.values())]
    for r in rect:
        ax.add_patch(r)
    ax.set_xticks(np.arange(0, 29) + 0.45, cmaps.keys())


if __name__ == "__main__":
    fig = Figure(19, 9)
    r, space, vs, top = 5.2, 0.5, 0.7, 0.5
    fig.add_axes_cm("A1", 1.5, top, 3 * r + 2 * space, 2, anchor="left upper")
    fig.add_axes_cm("LB", 1.5, 2 + top, 3 * r + 2 * space, 0.15, anchor="left upper")
    fig.add_axes_cm("B1", 1.5, 2 + top + vs, r, r, anchor="left upper", projection="polar")
    fig.add_axes_cm("B2", 1.5 + r + space, 2 + top + vs, r, r, anchor="left upper", projection="polar")
    fig.add_axes_cm("B3", 1.5 + 2 * (r + space), 2 + top + vs, r, r, anchor="left upper", projection="polar")
    fig.add_axes_cm("L", 1.5, 2 + top + 0.15, 3 * r + 2 * space, r + 0.7, anchor="left upper")
    axes = fig.axes_dict
    axes["L"].set_facecolor("none")
    axes["L"].xaxis.set_visible(False)
    axes["L"].yaxis.set_visible(False)

    data = pd.read_excel("test/data/SensitivityModelRunResults2.xlsx")
    ts, fs, ss = sensitivity_analysis(data, 4, -10000, 1)

    x = np.arange(ts.shape[0])
    width = 0.15
    space = 0.00

    plot_s(axes["A1"], x, ts, fs, colort="#dca7eb", colorf="#b7f5de")
    visilize(axes["B1"], ss)

    ts, fs, ss = sensitivity_analysis(data, 8, -10000, 1)
    plot_s(
        axes["A1"], x + 2 * width + space, ts, fs, colort="#a0d0d0", colorf="#d5aabe"
    )
    visilize(axes["B2"], ss)

    ts, fs, ss = sensitivity_analysis(data, 12, -10000, 1)
    plot_s(
        axes["A1"],
        x + 4 * width + 2 * space,
        ts,
        fs,
        colort="#eae936",
        colorf="#a7c0df",
    )
    cmaps = visilize(axes["B3"], ss)

    axes["A1"].set_title("  (a)")
    axes["L"].set_title("  (b)", y=0.80)
    add_color_section(axes["LB"], cmaps)

    fig.show()
