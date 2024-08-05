# 本脚本用于边界条件和水库溶解氧和温度的三次样条曲线插值
# 由逐月的数据插值为逐日的数据

from scipy.interpolate import interp1d
import pandas as pd
from matplotlib import pyplot as plt
import numpy as np
from plotstyles.figure import Figure
from plotstyles import styles
from matplotlib.dates import DateFormatter, MonthLocator, DayLocator, YearLocator
styles.use('wr')

def interpolate(ax, name, x:np.ndarray, y:np.ndarray, color='grey', linewidth=0.3, marker='o', markersize=1, markercolor='grey'):
    intx = np.arange(0, len(x))
    xy = np.hstack([intx.reshape(-1, 1), y.reshape(-1, 1)])
    mask = ~np.isnan(xy).any(axis=1)
    xy = xy[mask]
    index_min, index_max = xy[:, 0].min(), xy[:, 0].max()
    func = interp1d(xy[:, 0], xy[:, 1], kind='cubic')
    int_index = range(int(index_min), int(index_max + 1), 1)
    inty = func(np.array(int_index))
    intx = x[int_index]
    ax.plot(intx, inty, lw=linewidth, color=color)
    ax.plot(x, y, color=color, lw=linewidth, marker=marker, markersize=markersize, markerfacecolor=markercolor, markeredgecolor=markercolor)
    ax.set_title(name)
    return x, y, intx, inty

def draw(ax, name, x, y, color='grey', marker='o', markersize=1, markercolor='grey'):
    ax.plot(x, y, color=color, marker=marker, markersize=markersize, markerfacecolor=markercolor, markeredgecolor=markercolor)
    ax.set_title(name)

# 计算排除异常值后的平均值并返回异常值的位置
def exclude_outliers(data):
    if np.isnan(data.values).all():
        return (np.nan, np.nan, np.isnan(data.values))
    q1 = np.percentile(data.values[~np.isnan(data.values)], 25)  
    q3 = np.percentile(data.values[~np.isnan(data.values)], 75)  
    # 计算IQR  
    iqr = q3 - q1  
    # 定义异常值的下界和上界  
    lower_bound = q1 - 1.5 * iqr  
    upper_bound = q3 + 1.5 * iqr  
    mask = (data < lower_bound) | (data > upper_bound)
    return (data[~mask].mean(skipna=True), data[~mask].std(skipna=True), mask)

if __name__ == "__main__":

    fig = Figure(14, 15)
    axes = fig.axes_dict
    fig.add_axes_cm('T', 1.5, 1, 12, 2.5, anchor='left upper')
    fig.add_axes_cm('DO', 1.5, 3.5, 12, 2.5, anchor='left upper')
    fig.add_axes_cm('OON', 1.5, 6, 12, 2.5, anchor='left upper')
    fig.add_axes_cm('NHN', 1.5, 8.5, 12, 2.5, anchor='left upper')
    fig.add_axes_cm('NON', 1.5, 11, 12, 2.5, anchor='left upper')

    sites = ['Kuxi', 'Taoli', 'Neihu', 'Henghe', 'Kudong', 'Jingou', 'BaiDam', 'ChaoDam']
    indicators = ['T', 'DO']
    data_sites = pd.read_excel("test/data/0维水质模型数据未插值.xlsx", sheet_name='Reservoir')
    index = data_sites['Date'].values

    T_index = [i + '_T' for i in sites]
    T_mean = data_sites[T_index].mean(skipna=True, axis=1).values
    for v in T_index:
        interpolate(axes['T'], 'T', index, data_sites[v].values)
    _, _, intTx, intT = interpolate(axes['T'], '  (a)', index, T_mean, color='b', markercolor='r', markersize=3, linewidth=1.5)

    DO_index = [i + '_DO' for i in sites]
    DO_mean = data_sites[DO_index].mean(skipna=True, axis=1).values
    for v in DO_index:
        interpolate(axes['DO'], '  DO', index, data_sites[v].values)
    _, _, intDOx, intDO = interpolate(axes['DO'], '  (b)', index, DO_mean, color='b', markercolor='r', markersize=3, linewidth=1.5)

    line1, = axes['T'].plot([], [], color='grey', linewidth=0.3, marker='o', markersize=1, markerfacecolor='grey', markeredgecolor='grey')
    line2, = axes['T'].plot([], [], color='b', linewidth=1.5, marker='o', markersize=3,  markerfacecolor='r', markeredgecolor='r')
    axes['DO'].legend(labels=['site-specific interpolation', 'average-based interpolation'], handles=[line1, line2], frameon=False, ncols=2)




    NHN_index = [i + '_NHN' for i in sites]
    NHN_mean = data_sites[NHN_index].mean(skipna=True, axis=1).values
    NHN_mean_EXCLUDEOUT= data_sites[NHN_index].apply(exclude_outliers, axis=1)
    nhn_mean = NHN_mean_EXCLUDEOUT.apply(func=lambda x: x[0])
    nhn_mask = NHN_mean_EXCLUDEOUT.apply(func=lambda x: x[2])

    for v in NHN_index:
        draw(axes['NHN'], 'Ammonia Nitrogen', index, data_sites[v].values)
    for v in NHN_index:
        draw(axes['NHN'], 'Ammonia Nitrogen', index, data_sites[nhn_mask][v].values, markercolor='r', marker='o')
    draw(axes['NHN'], '  (d)', index, nhn_mean, color='b', markercolor='b', markersize=3, marker='d')

    dot1, = axes['NHN'].plot([], [], linewidth=0, markerfacecolor='grey', markersize=1, marker='o', markeredgecolor='grey')
    dot2, = axes['NHN'].plot([], [], linewidth=0, markerfacecolor='r', markersize=1, marker='o', markeredgecolor='r')
    dot3, = axes['NHN'].plot([], [], linewidth=0, markerfacecolor='b', markersize=3, marker='d', markeredgecolor='b')
    axes['OON'].legend(labels=['individual value', 'outliers', 'mean value'], handles=[dot1, dot2, dot3], frameon=False, ncols=3)



    NON_index = [i + '_NON' for i in sites]
    NON_mean = data_sites[NON_index].mean(skipna=True, axis=1).values
    NON_mean_EXCLUDEOUT = data_sites[NON_index].apply(exclude_outliers, axis=1)
    non_mean = NON_mean_EXCLUDEOUT.apply(lambda x: x[0])
    non_mask = NON_mean_EXCLUDEOUT.apply(lambda x: x[2])
    for v in NON_index:
        draw(axes['NON'], 'Nitrate Nitrogen', index, data_sites[v].values)
    for v in NON_index:
        draw(axes['NON'], 'Nitrate Nitrogen', index, data_sites[NON_index][non_mask][v].values, markercolor='r', marker='o')
    # draw(axes['NON'], '  Nitrate Nitrogen', index, NON_mean, color='r', markercolor='r', markersize=5, marker='d')
    draw(axes['NON'], '  (e)', index, non_mean, color='b', markercolor='b', markersize=3, marker='d')

    TN_index = [i + '_TN' for i in sites]
    TN_mean = data_sites[TN_index].mean(skipna=True, axis=1).values
    TN_mean_EXCLUDE = data_sites[TN_index].apply(exclude_outliers, axis=1)
    tn_mean = TN_mean_EXCLUDE.apply(lambda x: x[0])
    tn_mask = TN_mean_EXCLUDE.apply(lambda x: x[2])
    for a, n, t in zip(NHN_index, NON_index, TN_index):
        org = (data_sites[t] - data_sites[a] - data_sites[n]).values
        draw(axes['OON'], '  Organic Nitrogen', index, org)
    for a, n, t in zip(NHN_index, NON_index, TN_index):
        org = (data_sites[TN_index][tn_mask][t] - data_sites[NHN_index][nhn_mask][a] - data_sites[NON_index][non_mask][n]).values
        draw(axes['OON'], '  Organic Nitrogen', index, org, color='r', marker='o', markercolor='r')
    draw(axes['OON'], '  (c)', index, tn_mean - nhn_mean - non_mean, color='b', markercolor='b', markersize=3, marker='d')

    data = pd.DataFrame()
    data['Date'] = index
    data['T_Origin'] = T_mean
    data['DO_origin'] = DO_mean
    data['Cno_exclude'] = tn_mean - nhn_mean - non_mean
    data['Cna_exclude'] = nhn_mean
    data['Cnn_exclude'] = non_mean
    # data['T_int'] = intT
    intT = pd.Series(intT, intTx, name='intT')
    data = data.set_index('Date').join(intT)
    intDO = pd.Series(intDO, intDOx, name='intDO')
    data = data.join(intDO)
    # print(intT)
    # data.to_excel("test/data/0维水质模型剔除异常值平均和样条曲线插值.xlsx")

    from datetime import date
    for ax in ['T', 'DO', 'OON', 'NHN', 'NON']:
        axes[ax].set_xlim(date(2014, 12, 31), date(2020, 12, 31))
        axes[ax].xaxis.set_major_formatter(DateFormatter('%y-%m'))
        axes[ax].xaxis.set_major_locator(YearLocator(1, 12, 31))
        if ax != 'NON':
            axes[ax].xaxis.set_visible(False)
    axes['T'].set_ylim(-5, 30)
    axes['DO'].set_ylim(0, 20)
    axes['OON'].set_ylim(0, 1.2)
    axes['NHN'].set_ylim(0, 0.45)
    axes['NON'].set_ylim(0, 2.2)

    axes['T'].set_ylabel('Temperature\n$(^\circ C$)')
    axes['DO'].set_ylabel('Dissolved oxygen\n$(mg/L)$')
    axes['OON'].set_ylabel('Organic nitrogen\n$(mg/L)$')
    axes['NHN'].set_ylabel('Ammonia nitrogen\n$(mg/L)$')
    axes['NON'].set_ylabel('Nitrate nitrogen\n$(mg/L)$')
    axes['NON'].set_xlabel('Time')

    fig.align_ylabels_coords(axes.values(), -0.06, 0.5)

    
    fig.savefig("test/images/0维水质模型环境条件插值及异常观测值剔除求平均.pdf")

    fig.show()



