import numpy as np
from matplotlib import pyplot as plt
from plotstyles import tools
from plotstyles.fonts import global_fonts
import pandas as pd

# 时间步数 
time_steps, dt = 100, 0.1
t_arr = [0]
for i in range(time_steps):
    t_arr.append(t_arr[i] + dt)
t_arr = np.array(t_arr)

# 变化的k值
k_base = 0.08
r_v, d_v = 0.01, 0.01
k_arr = [k_base]
for i in range(time_steps):
    if(i < 20):
        k_arr.append(k_arr[i])
    elif (i < 40):
        k_arr.append(k_arr[i] + r_v * dt)
    elif (i < 80):
        k_arr.append(k_arr[i] - d_v * dt)
    else:
        k_arr.append(k_arr[i])
k_arr = np.array(k_arr)


# 模拟函数
def simulate(c0, k_arr, dt):
    c_arr = [c0]
    for i in range(len(k_arr) - 1):
        c_arr.append(c_arr[i] - k_arr[i] * c_arr[i] * dt)
    c_arr = np.array(c_arr)
    return c_arr

def calc_re(c_true, c_simu):
    re = np.mean(np.abs(c_true - c_simu) / c_true) * 100
    return re

# 寻找最佳和最差的拟合相对误差
def search_best_worst_fit(k_min, k_max, points_num, c_comp):
    k_find = np.linspace(k_min, k_max, points_num)
    re_arr = []
    for k in k_find:
        k_simu = [k] * len(k_arr)
        c_simu = simulate(c0, k_simu, dt)
        re = calc_re(c_comp, c_simu)
        re_arr.append(re)
    re_arr = np.array(re_arr)
    re_min, re_max = re_arr.min(), re_arr.max()
    k_for_min = k_find[np.where(re_arr == re_min)]
    k_for_max = k_find[np.where(re_arr == re_max)]
    return re_min, re_max, [k_for_min[0]] * len(k_arr), [k_for_max[0]] * len(k_arr)

# 理论浓度给定
c0 = 3
c_real = simulate(c0, k_arr, dt)

np.random.seed(0)
c_obs = c_real * (1 +  np.random.normal(0, 0.05, len(c_real)))

# 给定模拟k值以及模拟浓度
k_simu = [k_base + 0.01] * len(k_arr)
c_simu = simulate(c0, k_simu, dt)

re_min, re_max, k_for_min, k_for_max = search_best_worst_fit(k_arr.min(), k_arr.max(), 100, c_real)
c_simu_min = simulate(c0, k_for_min, dt)
c_simu_max = simulate(c0, k_for_max, dt)

re_min_obs, re_max_obs, k_for_obs_min, k_for_obs_max = search_best_worst_fit(k_arr.min(), k_arr.max(), 100, c_obs)
c_simu_obs = simulate(c0, k_for_obs_min, dt)
re_min_real = calc_re(c_real, c_simu_obs)

# 绘图
fig = plt.figure(figsize=tools.cm2inch([16, 6]))
c_ax = fig.add_subplot(121)
k_ax = fig.add_subplot(122)

c_ax.plot(t_arr, c_real, label='真实值', lw=1, color='r')
c_ax.plot(t_arr, c_obs, label='观测值', lw=0, marker='o', markersize=1, color='g')
c_ax.plot(t_arr, c_simu_min, label='最佳拟合(真实)', lw=1, color='b')
c_ax.plot(t_arr, c_simu_obs, label='最佳拟合(观测)', lw=1, color='orange')
c_ax.text(0.1, 0.2, f"$RE_{{min}}^{{real}}={re_min:.1f}\\%$", fontsize=8, transform=c_ax.transAxes)
c_ax.text(0.1, 0.3, f"$RE_{{minobs}}^{{obs}}={re_min_obs:.1f}\\%$", fontsize=8, transform=c_ax.transAxes)
c_ax.legend()
c_ax.set_xlim(-1, 11)
c_ax.set_xticks([0, 2, 4, 6, 8, 10])

k_ax.plot(t_arr, k_arr, label='真实值', lw=1, color='r')
k_ax.plot(t_arr, k_for_min, label='最佳拟合值', lw=1, color='b')
k_ax.set_ylim(0, 0.20)
k_ax.set_xlim(-1, 11)
k_ax.set_xticks([0, 2, 4, 6, 8, 10])
k_ax.legend()
plt.show()

# df = pd.DataFrame({'C':c_obs})
# df.to_excel("test/data/Degradation_in.xlsx")