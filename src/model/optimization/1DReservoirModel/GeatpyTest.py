# -*- coding: utf-8 -*-
"""该目标函数存在多个欺骗性很强的局部最优点.

max f = 4*x1 + 2*x2 + x3
s.t.
2*x1 + x2 - 1 <= 0
x1 + 2*x3 - 2 <= 0
x1 + x2 + x3 - 1 == 0
0 <= x1,x2 <= 1
0 < x3 < 2
"""
import numpy as np

import geatpy as ea


class MyProblem(ea.Problem):  # 继承Problem父类

    def __init__(self):
        name = 'MyProblem'  # 初始化name（函数名称，可以随意设置）
        M = 1  # 初始化M（目标维数）
        maxormins = [-1]  # 初始化maxormins（目标最小最大化标记列表，1：最小化该目标；-1：最大化该目标）
        Dim = 3  # 初始化Dim（决策变量维数）
        varTypes = [0] * Dim  # 初始化varTypes（决策变量的类型，元素为0表示对应的变量是连续的；1表示是离散的）
        lb = [0, 0, 0]  # 决策变量下界
        ub = [1, 1, 2]  # 决策变量上界
        lbin = [1, 1, 0]  # 决策变量下边界（0表示不包含该变量的下边界，1表示包含）
        ubin = [1, 1, 0]  # 决策变量上边界（0表示不包含该变量的上边界，1表示包含）
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
        x1 = Vars[:, [0]]
        x2 = Vars[:, [1]]
        x3 = Vars[:, [2]]
        print(x1)
        exit()

        # 计算目标函数值，赋给pop种群对象的ObjV属性
        pop.ObjV = 4 * x1 + 2 * x2 + x3

        # 采用可行性法则处理约束，生成种群个体违反约束程度矩阵
        pop.CV = np.hstack(
            [2 * x1 + x2 - 1, # 第一个约束
             x1 + 2*x3 - 2, # 第二个约束
             np.abs(x1 + x2 + x3 - 1)] # 第三个约束
        )

    def calReferObjV(self):  # 设定目标数参考值（本问题目标函数参考值设定为理论最优值）
        referenceObjV = np.array([[3.5]])
        return referenceObjV

if __name__ == '__main__':
    # 实例化问题对象
    problem = MyProblem()

    # 种群设置
    Encoding = 'RI'  # 编码方式
    NIND = 5000 # 种群规模

    Field = ea.crtfld(Encoding, problem.varTypes, problem.ranges, problem.borders) # 创建区域描述器
    population = ea.Population(Encoding, Field, NIND) # 实例化种群对象(此时种群还没被真正初始化，仅仅是生成一个种群对象)

    # 算法参数设置
    myAlgorithm = ea.soea_DE_best_1_L_templet(problem, population) # 实例化一个算法模板对象
    myAlgorithm.MAXGEN = 1000 # 最大进化代数
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