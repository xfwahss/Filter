# -*- coding: utf-8 -*-
""" 1维模型全局优化
min rmse_cna
    rmse_cno
    rmse_cnn

s.t.
   c_oxo > c_oxc
   c_noxc > c_noxo

"""
import numpy as np
import geatpy as ea
import subprocess
import pandas as pd
from multiprocessing import Pool
import multiprocessing as mp
import shutil


def rmse(value_model:np.array, value_obs:np.array):  
    """  
    Calculate Root Mean Squared Error (RMSE)  
      
    Parameters:  
    value_obs (np.array): Array of observed values  
    value_model (np.array): Array of simulated (predicted) values  
      
    Returns:  
    rmse (float): Root Mean Squared Error  
    """  
    # 确保 obs 和 sim 的长度相同  
    assert value_obs.shape == value_model.shape, "obs and sim must have the same shape"  
    value_obs = value_obs.astype(np.float64)
    value_model = value_model.astype(np.float64)

    all_v = np.vstack([value_model, value_obs])
    has_nan = np.isnan(all_v).any(axis=0)
    filtered_v = all_v[:, ~has_nan]
    value_model = filtered_v[0, :]
    value_obs = filtered_v[1, :]
    # 计算 RMSE  

    # 设定一个最大值，当差的平方超过这个值时，就将其设为这个最大值  
    MAX_SQUARE_VALUE = 1e6  # 根据需要调整这个值  
  
    # 计算差的平方，但先限制差的绝对值  
    diff = np.abs(value_obs - value_model)  
    squared_diff = np.clip(diff ** 2, 0, MAX_SQUARE_VALUE)  
  
    # 计算RMSE  
    rmse_value = np.sqrt(np.mean(squared_diff)) 
    return rmse_value

obs = pd.read_excel("D:/Gitlocal/Filter/test/data/1DReservoirModel_Origin.xlsx", sheet_name="Res_Avg")
Res_Cno_obs = obs["Res_Cno"].values
Res_Cna_obs = obs["Res_Cna"].values
Res_Cnn_obs = obs["Res_Cnn"].values

def calc_rmse(i, C_ro0, C_ko1, C_ra0, C_kab1, C_foxmin, C_c_oxc, C_c_oxo, C_theta_a,
                C_T_c, C_rn0, C_knb1, C_Tnc, C_theta_n, C_c_noxc, C_c_noxo):
    print(i)
    shutil.copy("D:/Gitlocal/Filter/test/data/1DReservoirModel_Origin.xlsx",
                f"D:/Gitlocal/Filter/test/temp/1DReservoirModel_Origin{i}.xlsx")
    subprocess.run(["D:/Gitlocal/Filter/build/bin/1DReservoirModel", "-i", f"D:/Gitlocal/Filter/test/temp/1DReservoirModel_Origin{i}.xlsx",
                    "-o", f"D:/Gitlocal/Filter/test/temp/1DReservoirModel_Origin_output{i}.xlsx",
                    "--ro0", f"{C_ro0}",
                    "--ko1", f"{C_ko1}",
                    "--ra0", f"{C_ra0}",
                    "--kab1", f"{C_kab1}",
                    "--foxmin", f"{C_foxmin}",
                    "--c_oxc", f"{C_c_oxc}",
                    "--c_oxo", f"{C_c_oxo}",
                    "--theta_a", f"{C_theta_a}",
                    "--T_c", f"{C_T_c}",
                    "--rn0", f"{C_rn0}",
                    "--knb1", f"{C_knb1}",
                    "--Tnc", f"{C_Tnc}",
                    "--theta_n", f"{C_theta_n}",
                    "--c_noxc", f"{C_c_noxc}",
                    "--c_noxo", f"{C_c_noxo}"], stdout=subprocess.DEVNULL)
    simu = pd.read_excel(f"D:/Gitlocal/Filter/test/temp/1DReservoirModel_Origin_output{i}.xlsx", sheet_name="Simulation")
    cno_simu = simu.iloc[:, 1].values
    cna_simu = simu.iloc[:, 2].values
    cnn_simu = simu.iloc[:, 3].values

    a = rmse(cno_simu, Res_Cno_obs)
    b = rmse(cna_simu, Res_Cna_obs)
    c = rmse(cnn_simu, Res_Cnn_obs)
    return a, b, c
class Model_Train(ea.Problem):  # 继承Problem父类

    def __init__(self):
        name = 'Model_Train'  # 初始化name（函数名称，可以随意设置）
        M = 3  # 初始化M（目标维数）
        maxormins = [1, 1, 1]  # 初始化maxormins（目标最小最大化标记列表，1：最小化该目标；-1：最大化该目标）
        Dim = 15  # 初始化Dim（决策变量维数）
        varTypes = [0] * Dim  # 初始化varTypes（决策变量的类型，元素为0表示对应的变量是连续的；1表示是离散的）
        lb = [1e-12, 1e-12, 1e-12, 1e-12, 0, 0, 0, 1, -10,   1e-12, 1e-12, -10, 1, 0, 0]  # 决策变量下界
        ub = [1e-5, 1e-3, 1e-5, 1e-3, 1, 12, 12, 10, 20,     1e-5,  1e-3,  20,  10, 12, 12]  # 决策变量上界
        lbin = [1] * Dim  # 决策变量下边界（0表示不包含该变量的下边界，1表示包含）
        ubin = [1] * Dim  # 决策变量上边界（0表示不包含该变量的上边界，1表示包含）
        # 调用父类构造方法完成实例化
        ea.Problem.__init__(self,
                            name,
                            M,
                            maxormins,
                            Dim,
                            varTypes,
                            lb,
                            ub,
                            lbin,
                            ubin)

    def aimFunc(self, pop):
        Vars = pop.Phen
        ro0 = Vars[:, [0]]
        ko1 = Vars[:, [1]]

        ra0 = Vars[:, [2]]
        kab1 = Vars[:, [3]]
        foxmin = Vars[:, [4]]
        c_oxc = Vars[:, [5]]
        c_oxo = Vars[:, [6]]
        theta_a = Vars[:, [7]]
        T_c = Vars[:, [8]]

        rn0 = Vars[:, [9]]
        knb1 = Vars[:, [10]]
        Tnc = Vars[:, [11]]
        theta_n = Vars[:, [12]]
        c_noxc = Vars[:, [13]]
        c_noxo = Vars[:, [14]]

        # 计算目标函数值，赋给pop种群对象的ObjV属性
        f_cno = np.zeros_like(ra0)
        f_cna = np.zeros_like(ra0)
        f_cnn = np.zeros_like(ra0)

        core_nums = int(mp.cpu_count())
        results = []

        with Pool(core_nums) as pool:
            for i in range(f_cno.shape[0]):
                C_ro0 = ro0[i, 0]
                C_ko1 = ko1[i, 0]

                C_ra0 = ra0[i, 0]
                C_kab1 = kab1[i, 0]
                C_foxmin = foxmin[i, 0]
                C_c_oxc = c_oxc[i, 0]
                C_c_oxo = c_oxo[i, 0]
                C_theta_a = theta_a[i, 0]
                C_T_c = T_c[i, 0]

                C_rn0 = rn0[i, 0]
                C_knb1 = knb1[i, 0]
                C_Tnc = Tnc[i, 0]
                C_theta_n = theta_n[i, 0]
                C_c_noxc = c_noxc[i, 0]
                C_c_noxo = c_noxo[i, 0]
                results.append(pool.apply_async(calc_rmse, (i, C_ro0, C_ko1, C_ra0, C_kab1, C_foxmin, C_c_oxc, C_c_oxo, C_theta_a,
                                                C_T_c, C_rn0, C_knb1, C_Tnc, C_theta_n, C_c_noxc, C_c_noxo)))
                                                
            for i, res in enumerate(results):
                a, b, c = res.get()
                f_cno[i, 0], f_cna[i, 0], f_cnn[i, 0] = a, b, c

        # print(np.hstack([f_cno, f_cna, f_cnn]))

        pop.ObjV = np.hstack([f_cno, f_cna, f_cnn])

        # 采用可行性法则处理约束，生成种群个体违反约束程度矩阵
        pop.CV = np.hstack(
            [c_oxc - c_oxo,
             c_noxo - c_noxc]
        )

    def calReferObjV(self):  # 设定目标数参考值（本问题目标函数参考值设定为理论最优值）
        referenceObjV = np.array([[0],[0], [0]]).T
        return referenceObjV

if __name__ == '__main__':
    # 实例化问题对象
    problem = Model_Train()

    # 种群设置
    Encoding = 'RI'  # 编码方式
    NIND = 50 # 种群规模

    Field = ea.crtfld(Encoding, problem.varTypes, problem.ranges, problem.borders) # 创建区域描述器
    population = ea.Population(Encoding, Field, NIND) # 实例化种群对象(此时种群还没被真正初始化，仅仅是生成一个种群对象)

    # 算法参数设置
    myAlgorithm = ea.moea_NSGA2_templet(problem, population)
    myAlgorithm.MAXGEN = 20 # 最大进化代数
    myAlgorithm.mutOper.F = 0.5 # 差分进化中的参数F
    myAlgorithm.recOper.XOVR = 0.7 # 设置交叉概率
    myAlgorithm.logTras = 1 # 设置每隔多少代记录日志，若设置为0则表示不记录日志
    myAlgorithm.verbose = True # 设置是否打印输出日志信息
    myAlgorithm.drawing = 1 # 0 不绘图 1 绘制结果图 2 目标空间动画图  3 决策空间过程动画

    # 调用算法模板进行种群进化
    [BestIndi, population] = myAlgorithm.run() # 执行算法模板，得到最优个体以及最后一代种群
    BestIndi.save() # 把最优个体的信息保存到文件中
    """=================================输出结果======================="""
    print('评价次数：%s' % myAlgorithm.evalsNum)
    print('时间已过 %s 秒' % myAlgorithm.passTime)
    if BestIndi.sizes != 0:
        print('最优的目标函数值为：%s' % BestIndi.ObjV[0][0])
        print('最优的控制变量值为：')
        for i in range(BestIndi.Phen.shape[1]):
            print(BestIndi.Phen[0, i])
    else:
        print('没找到可行解。')