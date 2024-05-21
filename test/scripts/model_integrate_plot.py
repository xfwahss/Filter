from matplotlib import pyplot as plt
from plotstyles.fonts import global_fonts
import pandas as pd
import utils

utils.cd_file_dir(__file__)

obs_data = pd.read_excel("../data/Miyun_Model.xlsx", sheet_name='Reservoir')
filter_data = pd.read_excel("../data/Miyun_Model_out.xlsx", sheet_name='X')
error_data = pd.read_excel("../data/Miyun_Model_out.xlsx", sheet_name='P')
flow_in_data = pd.read_excel("../data/Miyun_Model.xlsx", sheet_name='Rivers_in')
flow_out_data = pd.read_excel("../data/Miyun_Model.xlsx", sheet_name='Rivers_out')

x = obs_data['Date']


fig = plt.figure(figsize=(12, 16))
flow_in_ax = fig.add_subplot(321)
obs_flow_baihe, obs_flow_chaohe = flow_in_data['Bai_Flow'], flow_in_data['Chao_Flow']
filter_flow_baihe, filter_flow_chaohe = filter_data['Baihe_Flow'], filter_data['Chaohe_Flow']
flow_in_ax.plot(x, filter_flow_baihe, 'r-', lw=0.5)
flow_in_ax.plot(x, obs_flow_baihe, 'r--', lw=0.75)
flow_in_ax.plot(x, filter_flow_chaohe, 'b-', lw=0.5)
flow_in_ax.plot(x, obs_flow_chaohe, 'b--', lw=0.75)

fig2 = plt.figure(figsize=(16, 8))
# flow_out_ax = fig.add_subplot(3, 2, 2)
flow_out_ax = fig2.add_subplot(111)
obs_flow_baihe_dam, obs_flow_chaohe_dam = flow_out_data['Baihe_Dam_Flow'], flow_out_data['Chaohe_Dam_Flow']
filter_flow_baihe_dam, filter_flow_chaohe_dam = filter_data['Baihe_Dam_Flow'], filter_data['Chaohe_Dam_Flow']
flow_out_ax.plot(x, obs_flow_baihe_dam, 'bo', markersize=2)
flow_out_ax.plot(x, filter_flow_baihe_dam, 'b-', lw=0.5)
flow_out_ax.plot(x, obs_flow_chaohe_dam, 'ro', markersize=2)
flow_out_ax.plot(x, filter_flow_chaohe_dam, 'r-', lw=0.5)

error_ax = fig.add_subplot(323)
error_ax.plot(x, error_data['Baihe_Flow'], 'r-', label='Baihe_Flow')
error_ax.plot(x, error_data['Chaohe_Flow'], 'g-', label='Chaohe_Flow')
error_ax.plot(x, error_data['Water_Level'], 'b-', label='Water Level')
error_ax.set_ylim(0, 21)
error_ax.legend(frameon=False)



wl_ax = fig.add_subplot(324)
obs_wl = obs_data['Water_Level']
filter_wl = filter_data['Water_Level']
wl_ax.plot(x, obs_wl, 'r-', lw=0.5)
wl_ax.plot(x, filter_wl, 'b--', lw=0.75)

con_ax = fig.add_subplot(325)
con_ax.plot([0, 160], [0, 160], lw=0.5, color='grey', linestyle='dashed')
con_ax.scatter(filter_wl, obs_wl, s=1, color='r')
con_ax.scatter(filter_flow_baihe, obs_flow_baihe, s=2, color='g')
con_ax.scatter(filter_flow_chaohe, obs_flow_chaohe, s=2, color='b')
con_ax.set_aspect(1)
con_ax.set_xlim(0, 160)
con_ax.set_ylim(0, 160)
con_ax.set_xlabel("Simulation")
con_ax.set_ylabel("Observation")

plt.show()