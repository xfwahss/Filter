# 本脚本用于入库河流物质浓度的样条曲线插值

import pandas as pd
from plotstyles import styles
from plotstyles.figure import Figure
from scipy.interpolate import interp1d
import numpy as np
from datetime import datetime
from matplotlib.dates import DateFormatter, MonthLocator, DayLocator, YearLocator

styles.use("wr")


def interpolate(x: np.ndarray, y: np.ndarray, method="cubic"):
    intx = np.arange(0, len(x))
    xy = np.hstack([intx.reshape(-1, 1), y.reshape(-1, 1)])
    mask = ~np.isnan(xy).any(axis=1)
    xy = xy[mask]
    index_min, index_max = xy[:, 0].min(), xy[:, 0].max()
    func = interp1d(xy[:, 0], xy[:, 1], kind=method)
    int_index = range(int(index_min), int(index_max + 1), 1)
    inty = func(np.array(int_index))
    intx = x[int_index]
    return x, y, intx, inty


color_cno = "grey"
color_cna = "r"
color_cnn = "b"

def add_other_axes(name):
    ax = fig.axes_dict[name]
    ax2 = fig.add_axes(ax.get_position(), sharex=ax)
    ax3 = fig.add_axes(ax.get_position(), sharex=ax)
    ax2.set_facecolor('none')
    ax3.set_facecolor('none')
    fig.axes_dict[name+'CNA'] = ax2
    fig.axes_dict[name+'CNN'] = ax3
    ax.spines['left'].set_position(("axes", -0.16))
    ax2.spines['left'].set_position(("axes", -0.08))
    ax.spines['left'].set_color(color_cno)
    ax.tick_params(axis='y', color=color_cno)
    ax2.spines['left'].set_color(color_cna)
    ax2.tick_params(axis='y', color=color_cna)
    ax3.spines['left'].set_color(color_cnn)
    ax3.tick_params(axis='y', color=color_cnn)
    ax.yaxis.set_major_formatter("{x:.2f}")
    ax2.yaxis.set_major_formatter("{x:.2f}")
    ax3.yaxis.set_major_formatter("{x:.1f}")
    for label in ax.get_yticklabels():
        label.set_color(color_cno)
    for label in ax2.get_yticklabels():
        label.set_color(color_cna)
    for label in ax3.get_yticklabels():
        label.set_color(color_cnn)
    ax2.xaxis.set_visible(False)
    ax3.xaxis.set_visible(False)


if __name__ == "__main__":
    fig = Figure(14, 12.7)
    axes = fig.axes_dict

    leftb, tb, vspace, w, h = 3.2, 0.7, 0.2, 10, 2
    fig.add_axes_cm("Chao", leftb, tb, w, h, "left upper")
    add_other_axes("Chao")
    fig.add_axes_cm("Bai", leftb, tb + h + vspace, w, h, "left upper")
    add_other_axes("Bai")
    fig.add_axes_cm("SN", leftb, tb + 2 *(h+vspace), w, h, "left upper")
    add_other_axes("SN")
    fig.add_axes_cm("BaiDam", leftb, tb + 3 * (h+vspace), w, h, "left upper")
    add_other_axes("BaiDam")
    fig.add_axes_cm("ChaoDam", leftb, tb + 4 * (h + vspace), w, h, "left upper")
    add_other_axes("ChaoDam")

    indataset = pd.read_excel(
        "test/data/2-0维水质模型数据未插值+环境因子样条曲线插值+剔除异常值平均.xlsx",
        sheet_name="RiversIn",
    )
    outdataset = pd.read_excel(
        "test/data/2-0维水质模型数据未插值+环境因子样条曲线插值+剔除异常值平均.xlsx",
        sheet_name="RiversOut",
    )
    index = indataset["Date"].values

    chao_cna = indataset["Chaohe_Cna"].values
    chao_cnn = indataset["Chaohe_Cnn"].values
    chao_ctn = indataset["Chaohe_Ctn"].values
    chao_con = chao_ctn - chao_cna - chao_cnn
    _, _, int_x_chao_cna, int_chao_cna = interpolate(index, chao_cna, method="linear")
    _, _, int_x_chao_cnn, int_chao_cnn = interpolate(index, chao_cnn, method="linear")
    _, _, int_x_chao_ctn, int_chao_ctn = interpolate(index, chao_ctn, method="linear")
    int_chao_con = int_chao_ctn - int_chao_cna - int_chao_cnn
    print(int_chao_con.min())
    axes["ChaoCNA"].plot(index, chao_cna, marker='o', color=color_cna, markersize=1.5)
    axes["ChaoCNA"].plot(int_x_chao_cna, int_chao_cna, color=color_cna, linestyle='solid', linewidth=0.5)
    axes["ChaoCNN"].plot(index, chao_cnn, marker='s', color=color_cnn, markersize=1.5)
    axes["ChaoCNN"].plot(int_x_chao_cnn, int_chao_cnn, color=color_cnn, linestyle='solid', linewidth=0.5)
    axes["Chao"].plot(index, chao_con, marker='^', color=color_cno, markersize=1.5)
    axes["Chao"].plot(int_x_chao_cnn, int_chao_con, color=color_cno, linestyle='solid', linewidth=0.5)
    axes["Chao"].set_xlim(datetime(2014, 12, 31), datetime(2020, 12, 31))
    axes["Chao"].xaxis.set_major_formatter(DateFormatter('%Y-%m-%d'))
    axes["Chao"].xaxis.set_major_locator(YearLocator(1, 12, 31))
    axes["Chao"].set_ylim(0, 8)
    axes["ChaoCNA"].set_ylim(0, 0.4)
    axes["ChaoCNN"].set_ylim(0, 15)

    bai_cna = indataset["Baihe_Cna"].values
    bai_cnn = indataset["Baihe_Cnn"].values
    bai_ctn = indataset["Baihe_Ctn"].values
    bai_con = bai_ctn - bai_cna - bai_cnn
    _, _, int_x_bai_cna, int_bai_cna = interpolate(index, bai_cna, method="linear")
    _, _, int_x_bai_cnn, int_bai_cnn = interpolate(index, bai_cnn, method="linear")
    _, _, int_x_bai_ctn, int_bai_ctn = interpolate(index, bai_ctn, method="linear")
    int_bai_con = int_bai_ctn - int_bai_cna - int_bai_cnn
    print(int_bai_con.min())
    axes["BaiCNA"].plot(index, bai_cna, marker='o', color=color_cna, markersize=1.5)
    axes["BaiCNA"].plot(int_x_bai_cna, int_bai_cna, color=color_cna, linestyle='solid', linewidth=0.5)
    axes["BaiCNN"].plot(index, bai_cnn, marker='s', color=color_cnn, markersize=1.5)
    axes["BaiCNN"].plot(int_x_bai_cnn, int_bai_cnn, color=color_cnn, linestyle='solid', linewidth=0.5)
    axes["Bai"].plot(index, bai_con, marker='^', color=color_cno, markersize=1.5)
    axes["Bai"].plot(int_x_bai_cnn, int_bai_con, color=color_cno, linestyle='solid', linewidth=0.5)
    axes["Bai"].set_xlim(datetime(2014, 12, 31), datetime(2020, 12, 31))
    axes["Bai"].xaxis.set_major_formatter(DateFormatter('%Y-%m-%d'))
    axes["Bai"].xaxis.set_major_locator(YearLocator(1, 12, 31))
    axes["Bai"].set_ylim(0, 3)
    axes["BaiCNA"].set_ylim(0, 0.6)
    axes["BaiCNN"].set_ylim(0, 6)

    sn_cna = indataset["S_to_N_Cna"].values
    sn_cnn = indataset["S_to_N_Cnn"].values
    sn_ctn = indataset["S_to_N_Ctn"].values
    sn_con = sn_ctn - sn_cna - sn_cnn
    _, _, int_x_sn_cna, int_sn_cna = interpolate(index, sn_cna, method="linear")
    _, _, int_x_sn_cnn, int_sn_cnn = interpolate(index, sn_cnn, method="linear")
    _, _, int_x_sn_ctn, int_sn_ctn = interpolate(index, sn_ctn, method="linear")
    int_sn_con = int_sn_ctn - int_sn_cna - int_sn_cnn
    print(int_sn_con.min())
    axes["SNCNA"].plot(index, sn_cna, marker='o', color=color_cna, markersize=1.5)
    axes["SNCNA"].plot(int_x_sn_cna, int_sn_cna, color=color_cna, linestyle='solid', linewidth=0.5)
    axes["SNCNN"].plot(index, sn_cnn, marker='s', color=color_cnn, markersize=1.5)
    axes["SNCNN"].plot(int_x_sn_cnn, int_sn_cnn, color=color_cnn, linestyle='solid', linewidth=0.5)
    axes["SN"].plot(index, sn_con, marker='^', color=color_cno, markersize=1.5)
    axes["SN"].plot(int_x_sn_cnn, int_sn_con, color=color_cno, linestyle='solid', linewidth=0.5)
    axes["SN"].set_xlim(datetime(2014, 12, 31), datetime(2020, 12, 31))
    axes["SN"].xaxis.set_major_formatter(DateFormatter('%Y-%m-%d'))
    axes["SN"].xaxis.set_major_locator(YearLocator(1, 12, 31))
    axes["SN"].set_ylim(0, 0.6)
    axes["SNCNA"].set_ylim(0, 0.4)
    axes["SNCNN"].set_ylim(0, 1.5)

    baihedam_cna = outdataset["Baihe_Dam_Cna"].values
    baihedam_cnn = outdataset["Baihe_Dam_Cnn"].values
    baihedam_ctn = outdataset["Baihe_Dam_Ctn"].values
    baihedam_con = baihedam_ctn - baihedam_cna - baihedam_cnn
    _, _, int_x_baihedam_cna, int_baihedam_cna = interpolate(
        index, baihedam_cna, method="linear"
    )
    _, _, int_x_baihedam_cnn, int_baihedam_cnn = interpolate(
        index, baihedam_cnn, method="linear"
    )
    _, _, int_x_baihedam_ctn, int_baihedam_ctn = interpolate(
        index, baihedam_ctn, method="linear"
    )
    int_baihedam_con = int_baihedam_ctn - int_baihedam_cna - int_baihedam_cnn
    print(int_baihedam_con.min())
    axes["BaiDamCNA"].plot(index, baihedam_cna, marker='o', color=color_cna, markersize=1.5)
    axes["BaiDamCNA"].plot(int_x_baihedam_cna, int_baihedam_cna, color=color_cna, linestyle='solid', linewidth=0.5)
    axes["BaiDamCNN"].plot(index, baihedam_cnn,  marker='s', color=color_cnn, markersize=1.5)
    axes["BaiDamCNN"].plot(int_x_baihedam_cnn, int_baihedam_cnn, color=color_cnn, linestyle='solid', linewidth=0.5)
    axes["BaiDam"].plot(index, baihedam_con,marker='^', color=color_cno, markersize=1.5)
    axes["BaiDam"].plot(int_x_baihedam_cnn, int_baihedam_con, color=color_cno, linestyle='solid', linewidth=0.5)
    axes["BaiDam"].set_xlim(datetime(2014, 12, 31), datetime(2020, 12, 31))
    axes["BaiDam"].xaxis.set_major_formatter(DateFormatter('%Y-%m-%d'))
    axes["BaiDam"].xaxis.set_major_locator(YearLocator(1, 12, 31))
    axes["BaiDam"].set_ylim(0, 0.8)
    axes["BaiDamCNA"].set_ylim(0, 0.4)
    axes["BaiDamCNN"].set_ylim(0, 3)



    chaohedam_cna = outdataset["Chaohe_Dam_Cna"].values
    chaohedam_cnn = outdataset["Chaohe_Dam_Cnn"].values
    chaohedam_ctn = outdataset["Chaohe_Dam_Ctn"].values
    chaohedam_con = chaohedam_ctn - chaohedam_cna - chaohedam_cnn
    _, _, int_x_chaohedam_cna, int_chaohedam_cna = interpolate(
        index, chaohedam_cna, method="linear"
    )
    _, _, int_x_chaohedam_cnn, int_chaohedam_cnn = interpolate(
        index, chaohedam_cnn, method="linear"
    )
    _, _, int_x_chaohedam_ctn, int_chaohedam_ctn = interpolate(
        index, chaohedam_ctn, method="linear"
    )
    int_chaohedam_con = int_chaohedam_ctn - int_chaohedam_cna - int_chaohedam_cnn
    print(int_chaohedam_con.min())
    m1, = axes["ChaoDamCNA"].plot(index, chaohedam_cna, marker='o', color=color_cna, markersize=1.5, linewidth=0)
    l1, = axes["ChaoDamCNA"].plot(int_x_chaohedam_cna, int_chaohedam_cna, color=color_cna, linestyle='solid', linewidth=0.5)
    m2, = axes["ChaoDamCNN"].plot(index, chaohedam_cnn, marker='s', color=color_cnn, markersize=1.5, linewidth=0)
    l2, = axes["ChaoDamCNN"].plot(int_x_chaohedam_cnn, int_chaohedam_cnn, color=color_cnn, linestyle='solid', linewidth=0.5)
    m3, = axes["ChaoDam"].plot(index, chaohedam_con, marker='^', color=color_cno, markersize=1.5, linewidth=0)
    l3, = axes["ChaoDam"].plot(int_x_chaohedam_cnn, int_chaohedam_con, color=color_cno, linestyle='solid', linewidth=0.5)

    axes["ChaoDam"].set_xlim(datetime(2014, 12, 31), datetime(2020, 12, 31))
    axes["ChaoDam"].xaxis.set_major_formatter(DateFormatter('%Y-%m-%d'))
    axes["ChaoDam"].xaxis.set_major_locator(YearLocator(1, 12, 31))
    axes["ChaoDam"].set_ylim(0, 1.05)
    axes["ChaoDamCNA"].set_ylim(0, 0.6)
    axes["ChaoDamCNN"].set_ylim(0, 2)

    axes['Bai'].set_xticklabels([])
    axes['Chao'].set_xticklabels([])
    axes['SN'].set_xticklabels([])
    axes['BaiDam'].set_xticklabels([])
    axes['ChaoDam'].set_xlabel('Time')
    axes['SN'].set_ylabel('Nitrogen concentration of various component $(mg/L)$')

    axes['Bai'].set_title('  (a) Baihe river')
    axes['Chao'].set_title('  (b) Chaohe river')
    axes['SN'].set_title('  (c) Jingmi Diversion Canal')
    axes['BaiDam'].set_title('  (d) Baihe Dam')
    axes['ChaoDam'].set_title('  (e) Chaohe Dam')

    fig.legend([m3, m1, m2, l3, l1, l2], ["Organic nitrogen", "Ammonia nitrogen", "Nitrate nitrogen",
                                          "Interpolation", "Interpolation", "Interpolation"], ncols=6,
                                          frameon=False, bbox_to_anchor=(0.1, 0.92, 0.8, 0.1), loc='center',
                                          columnspacing=0.5, handlelength=1)
    # fig.savefig("test/images/出入库河流浓度插值.pdf")

    df = pd.DataFrame()
    df['Date'] = index
    df = df.set_index('Date')
    baihe_cna = pd.Series(int_bai_cna, int_x_bai_cna, name='白河氨氮')
    baihe_cnn = pd.Series(int_bai_cnn, int_x_bai_cnn, name='白河硝氮')
    baihe_ctn = pd.Series(int_bai_ctn, int_x_bai_ctn, name='白河总氮')
    df = df.join(baihe_cna)
    df = df.join(baihe_cnn)
    df = df.join(baihe_ctn)


    chaohe_cna = pd.Series(int_chao_cna, int_x_chao_cna, name='潮河氨氮')
    chaohe_cnn = pd.Series(int_chao_cnn, int_x_chao_cnn, name='潮河硝氮')
    chaohe_ctn = pd.Series(int_chao_ctn, int_x_chao_ctn, name='潮河总氮')
    df = df.join(chaohe_cna)
    df = df.join(chaohe_cnn)
    df = df.join(chaohe_ctn)

    sn_df_cna = pd.Series(int_sn_cna, int_x_sn_cna, name='京密引水渠氨氮')
    sn_df_cnn = pd.Series(int_sn_cnn, int_x_sn_cnn, name='京密引水渠硝氮')
    sn_df_ctn = pd.Series(int_sn_ctn, int_x_sn_ctn, name='京密引水渠总氮')
    df = df.join(sn_df_cna)
    df = df.join(sn_df_cnn)
    df = df.join(sn_df_ctn)


    baidam_df_cna = pd.Series(int_baihedam_cna, int_x_baihedam_cna, name='白河主坝氨氮')
    baidam_df_cnn = pd.Series(int_baihedam_cnn, int_x_baihedam_cnn, name='白河主坝硝氮')
    baidam_df_ctn = pd.Series(int_baihedam_ctn, int_x_baihedam_ctn, name='白河主坝总氮')
    df = df.join(baidam_df_cna)
    df = df.join(baidam_df_cnn)
    df = df.join(baidam_df_ctn)


    chaodam_df_cna = pd.Series(int_chaohedam_cna, int_x_chaohedam_cna, name='潮河主坝氨氮')
    chaodam_df_cnn = pd.Series(int_chaohedam_cnn, int_x_chaohedam_cnn, name='潮河主坝硝氮')
    chaodam_df_ctn = pd.Series(int_chaohedam_ctn, int_x_chaohedam_ctn, name='潮河主坝总氮')
    df = df.join(chaodam_df_cna)
    df = df.join(chaodam_df_cnn)
    df = df.join(chaodam_df_ctn)
    df.to_excel('test/data/出入库氮组分浓度插值.xlsx')

    fig.show()
