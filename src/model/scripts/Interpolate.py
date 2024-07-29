import pandas as pd
from scipy.interpolate import interp1d
from matplotlib import pyplot as plt
import numpy as np

def remover_nan(t, x):
    combine = np.hstack([t.reshape(-1, 1), x.reshape(-1, 1)])
    mask = ~np.isnan(combine).any(axis=1)
    combine = combine[mask]
    return combine[:, 0], combine[:, 1]

data = pd.read_excel("test/temp/00DReservoirModel.xlsx", sheet_name='ResAverage')
simu_range = slice(0, 400, 1)
t = np.arange(0, 400, 1)
T_discrete = data['Res_T'].values[simu_range]
t_1, T_dis_1 = remover_nan(t, T_discrete)
f = interp1d(t_1, T_dis_1, 'cubic')
T = f(t[0:380])

DO_discrete = data['Res_DO'].values[simu_range]
t_2, DO_dis_2 = remover_nan(t, DO_discrete)
f2 = interp1d(t_2, DO_dis_2, 'cubic')
DO = f2(t[0:380])
# plt.plot(t_1, T_dis_1, 'bo')
# plt.plot(t[0:380], T, 'r')
# plt.plot(t_2, DO_dis_2, 'bo')
# plt.plot(t[0:380], DO, 'r')
# plt.show()
df = pd.DataFrame({'T':T, 'DO':DO})
df.to_excel('test/temp/Interpolate.xlsx')