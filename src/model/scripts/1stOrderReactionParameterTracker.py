import numpy as np
from plotstyles.fonts import global_fonts
from plotstyles.figure import Figure
import pandas as pd
import subprocess
import os
import model_tool

class Reaction:
    def init(self, steps=100, dt=0.1, k_base=0.08, rise_v=0.01, drop_v=0.01):
        self.steps = steps
        self.dt = dt
        # 产生实际k的基准
        self.k_base = k_base
        self.rise_v = rise_v
        self.drop_v = drop_v
        return

    def get_time_steps(self):
        t_arr = [0]
        for i in range(self.steps):
            t_arr.append(t_arr[i] + self.dt)
        return np.array(t_arr)
    
    def get_k_real(self):
        k_arr = [self.k_base]
        for i in range(self.steps):
            if(i < 0.2 * self.steps):
                k_arr.append(k_arr[i])
            elif (i < 0.4 * self.steps):
                k_arr.append(k_arr[i] + self.rise_v * self.dt)
            elif (i < 0.8 * self.steps):
                k_arr.append(k_arr[i] - self.drop_v * self.dt)
            else:
                k_arr.append(k_arr[i])
        return np.array(k_arr)
    
    def add_noise(self, true_v, mean=0, random=0.1):
        np.random.seed(0)
        return true_v * ( 1 + np.random.normal(mean, random, true_v.shape[0]))


    def simulate(self, c0, k):
        c_arr = [c0]
        for i in range(len(k) - 1):
            c_arr.append(c_arr[i] - k[i] * c_arr[i] * self.dt / 10)
        c_arr = np.array(c_arr)
        return c_arr
    
    def evaluate_re(self, c_true, c_simu):
        re = np.mean(np.abs(c_true - c_simu) / c_true) * 100
        # re = np.mean(np.abs(c_true - c_simu))
        return re
    

    # 寻找最佳和最差的拟合相对误差
    def search_best_worst_fit(self, k_min, k_max, points_num, c_comp):
        k_find = np.linspace(k_min, k_max, points_num)
        re_arr = []
        for k in k_find:
            k_simu = [k] * len(k_arr)
            c_simu = self.simulate(c0, k_simu)
            re = self.evaluate_re(c_comp, c_simu)
            re_arr.append(re)
        re_arr = np.array(re_arr)
        re_min, re_max = re_arr.min(), re_arr.max()
        k_for_min = k_find[np.where(re_arr == re_min)]
        k_for_max = k_find[np.where(re_arr == re_max)]
        return re_min, re_max, [k_for_min[0]] * len(k_arr), [k_for_max[0]] * len(k_arr)


###############滤波算法###############
def run_model(exe, toggle=True):
    cwd = os.getcwd()
    os.chdir(os.path.abspath('build'))
    if toggle:
        subprocess.run([exe])
    os.chdir(cwd)


value_nums = 200
set_dt = 10 / value_nums
def write_input(Obs, path='test/data/Degradation_in.xlsx', R=0.05):
    writer = pd.ExcelWriter(path)
    # 参数写入
    params = {
        'param_nums':6,
        'measurement_nums':value_nums,
        'dt':set_dt,
        'size':1000,
        'status_dims':4,
        'obs_dims':1,
        'tau': 0.16,
    }
    params=pd.DataFrame({'key':params.keys(), 'value':params.values()})
    params.to_excel(writer, sheet_name='Params', index=False, header=False)

    # 初始状态写入
    init_x = {
        'x': 3.26,
        'v': 2.8,
        'dv': 0,
        'ddv': 0
    }
    init_x = pd.DataFrame({'key':init_x.keys(), 'value':init_x.values()})
    init_x.to_excel(writer, sheet_name='Init_X', index=False, header=False)

    # 初始协方差
    init_P = np.array([
        [0.1, 0, 0, 0],
        [0, 0.01, 0, 0],
        [0, 0, 0.01, 0],
        [0, 0, 0, 0.01]
    ])
    init_P = pd.DataFrame(init_P)
    init_P.to_excel(writer, sheet_name='Init_P', index=False, header=False)

    # 观测矩阵
    H = np.array([
        [1, 0, 0, 0]
    ])
    H = pd.DataFrame(H)
    H.to_excel(writer, sheet_name='H', index=False, header=False)

    # 过程误差
    Q = {
        'x': 0.0,
        'v': 0.0,
        'dv': 0.00,
        'ddv': 0.00,
    }
    Q = pd.DataFrame({'key':Q.keys(), 'value':Q.values()})
    Q.to_excel(writer, sheet_name='Q', index=False, header=False)

    # 观测值写入
    Obs = pd.DataFrame(Obs)
    Obs.to_excel(writer, sheet_name='Obs')

    # 观测误差写入
    R = np.ones(shape=Obs.iloc[:,0].values.shape) * R
    R = pd.DataFrame(R)
    R.to_excel(writer, sheet_name='R')


    writer.close()

def read_status(path='test/output/Degradation_out.xlsx'):
    data = pd.read_excel(path, sheet_name='X')
    data_prior = pd.read_excel(path, sheet_name='prior_X')
    x = data['x'].values
    prior_x = data_prior.iloc[:, 1]
    k = data['v'].values
    dk = data['dv'].values
    ddk = data['ddv'].values
    return x, k, dk, prior_x, ddk

def read_variance(path='test/output/Degradation_out.xlsx'):
    data = pd.read_excel(path, sheet_name='P')
    x = data['x'].values
    k = data['v'].values
    return x, k

def calc_k_with_equation(c, dt):
    k_arr = []
    for i in range(c.shape[0] - 1):
        k = (np.log(c[i]) - np.log(c[i+1]))/dt
        k_arr.append(k)
    return np.array(k_arr)



if __name__ == '__main__':
    base_reaction = Reaction()
    base_reaction.init(steps=value_nums, dt=set_dt, k_base=2.8, rise_v=0.5, drop_v=0.5)
    t_arr = base_reaction.get_time_steps()
    k_arr = base_reaction.get_k_real()
    c0 = 3
    # 浓度真实值和观测值
    c_real = base_reaction.simulate(c0, k_arr)
    c_obs = base_reaction.add_noise(c_real, 0, 0.10)

    # 反算真实k值
    k_calc = calc_k_with_equation(c_obs, set_dt)



    ############### 数据同化部分
    write_input(Obs=c_obs, R=0.1)
    run_model("bin/1stOrderReactionParameterTracker", toggle=True)
    c_filter, k_filter, dk, prior_c, ddk = read_status()
    c_variance, k_variance = read_variance()

    ###############参数方程校正部分
    # 给定模拟k值以及模拟浓度
    k_simu = [base_reaction.k_base + 0.01] * len(k_arr)
    c_simu = base_reaction.simulate(c0, k_simu)

    # 根据真实值寻找最优估计
    re_min, re_max, k_for_min, k_for_max = base_reaction.search_best_worst_fit(k_arr.min(), k_arr.max(), 100, c_real)
    c_simu_min = base_reaction.simulate(c0, k_for_min)
    re_min_real = base_reaction.evaluate_re(c_real, c_simu_min)

    # 根据观测值寻找最优估计
    re_min_obs, re_max_obs, k_for_obs_min, k_for_obs_max = base_reaction.search_best_worst_fit(k_arr.min(), k_arr.max(), 100, c_obs)
    c_simu_obs = base_reaction.simulate(c0, k_for_obs_min)
    re_min_obs = base_reaction.evaluate_re(c_real, c_simu_obs)


    re_filter = base_reaction.evaluate_re(c_real[1:], c_filter)
    # 绘图
    fig = Figure(21, 9)
    fig.add_axes_cm('C', 1, 0.5, 6, 4, anchor='left upper')
    fig.add_axes_cm('K', 8, 0.5, 6, 4, anchor='left upper')
    fig.add_axes_cm('dk', 0.1, 0.5, 6, 4, anchor='right upper')

    fig.add_axes_cm('ddk', 0.1, 0.5, 6, 3, anchor='right bottom')
    fig.add_axes_cm('C_err', 1, 0.5, 6, 3, anchor='left bottom')
    fig.add_axes_cm('K_err', 8, 0.5, 6, 3, anchor='left bottom')
    axes = fig.axes_dict

    axes['C'].plot(t_arr, c_real, label='真实值', lw=1, color='r')
    axes['C'].plot(t_arr, c_obs, label='观测值', lw=0, marker='o', markersize=1, color='g')
    axes['C'].plot(t_arr, c_simu_min, label='最佳拟合(真实)', lw=1, color='b')
    axes['C'].plot(t_arr, c_simu_obs, label='最佳拟合(观测)', lw=1, color='orange')
    axes['C'].plot(t_arr[0:-1], c_filter, label='同化值', lw=1, color='k')
    axes['C'].plot(t_arr[0:-1], prior_c, label='先验预测', lw=1, color='k', linestyle='dashed')
    axes['C'].text(0.05, 0.1, f"$MAPE_{{min}}^{{real}}={re_min_real:.2f}\\%$", fontsize=6, transform=axes['C'].transAxes)
    axes['C'].text(0.05, 0.2, f"$MAPE_{{min}}^{{obs}}={re_min_obs:.2f}\\%$", fontsize=6, transform=axes['C'].transAxes)
    axes['C'].text(0.05, 0.3, f"$MAPE_{{min}}^{{filter}}={re_filter:.2f}\\%$", fontsize=6, transform=axes['C'].transAxes)
    axes['C'].legend(fontsize=8, frameon=False)
    # axes['C'].set_xlim(-1, 11)
    # axes['C'].set_xticks([0, 2, 4, 6, 8, 10])

    axes['K'].plot(t_arr, k_arr, label='真实值', lw=1, color='r')
    axes['K'].plot(t_arr, k_for_min, label='最佳拟合值', lw=1, color='b')
    # axes['K'].plot(t_arr[1:], k_calc, label='差分估算值', lw=0, color='g', marker='o', markersize=1)
    axes['K'].plot(t_arr[0:-1], k_filter, label='同化跟踪值', lw=1, color='k')
    axes['dk'].plot(t_arr[0:-1], dk, lw=1)
    axes['ddk'].plot(t_arr[0:-1], ddk, lw=1)
    # axes['K'].set_ylim(1, 4)
    # axes['K'].set_xlim(-1, 11)
    # axes['K'].set_xticks([0, 2, 4, 6, 8, 10])
    axes['K'].legend(fontsize=8, frameon=False)

    axes['C_err'].plot(t_arr[0:-1], c_variance, color='k', lw=1)

    axes['K_err'].plot(t_arr[0:-1], k_variance, color='k', lw=1)
    print(model_tool.nse(c_simu_min, c_real))
    print(model_tool.nse(c_simu_obs, c_real))
    print(model_tool.nse(c_filter, c_real[0:-1]))
    fig.show()