import pandas as pd
import multiprocessing as mp
from multiprocessing import Pool
import subprocess
import os
import numpy as np

def rmse(simu, obs, obs_mask):
    simu_rm_nan = simu[obs_mask]
    if (np.isnan(simu_rm_nan).sum()):
        return 10000
    v = np.sqrt(np.mean(simu_rm_nan - obs) ** 2)
    return v

def mape(simu, obs, obs_mask):
    simu_rm_nan = simu[obs_mask]
    if (np.isnan(simu_rm_nan).sum()):
        return 10000
    v = np.mean(np.abs(simu_rm_nan - obs) / obs) * 100
    return v


def nse(simu, obs, obs_mask):
    simu_rm_nan = simu[obs_mask]
    if (np.isnan(simu_rm_nan).sum()):
        return  -10000
    v = 1 - (np.sum((simu_rm_nan - obs) ** 2) / np.sum((obs - np.mean(obs)) ** 2))
    return v


def klinggupta(simu, obs, obs_mask):
    simu_rm_nan = simu[obs_mask]
    if (np.isnan(simu_rm_nan).sum()):
        return  -10000
    gamma = (np.std(simu_rm_nan) / np.mean(simu_rm_nan)) / (np.std(obs) / np.mean(obs))
    beta = np.sum(simu_rm_nan) / np.sum(obs)
    cc = np.corrcoef(obs, simu_rm_nan)[0, 1]
    v = 1.0 - np.sqrt(
        (cc - 1.0) ** 2 + (gamma - 1.0) ** 2 + (beta - 1.0) ** 2
    )
    return v

    return 1

def evaluate_model(id_create, filename_in, filename_out, params:dict, obs_cno, obs_cna, obs_cnn,
                   mask_obs_cno, mask_obs_cna, mask_obs_cnn):
    exe_list = ["build/bin/0DReservoirModel"]
    exe_list.extend(["-i", f"{filename_in}"])
    exe_list.extend(["-o", f"{filename_out}"])
    exe_list.extend(["--id", f"{id_create}"])
    exe_keys = ["--k_rpon", "--theta_rpon", "--k_lpon", "--theta_lpon", "--k_don", "--theta_don",
                "--rnit0", "--knit20", "--foxmin", "--c_oxc_nit", "--c_oxo_nit", "--theta_nit",
                "--T_c_nit", "--alpha", "--rdeni0", "--kdeni20", "--Tc_deni", "--theta_deni",
                "--c_oxc_deni", "--c_oxo_deni", "--beta", "--b_ndo_flux", "--b_amm_flux", "--b_nit_flux",
                "--s_nrp", "--s_nlp", "--alpha_rpon", "--alpha_lpon", "--h0"]
    other_terms = []
    for k, v in zip(exe_keys, params.values()):
        other_terms.extend([k, f"{v}"])
    exe_list.extend(other_terms)
    subprocess.run(exe_list, stdout=subprocess.DEVNULL)

    simu = pd.read_excel(filename_out, sheet_name="Simulation")
    simu_cno = simu.iloc[:, 1].values + simu.iloc[:, 2].values + simu.iloc[:, 3].values
    simu_cna = simu.iloc[:, 4].values
    simu_cnn = simu.iloc[:, 5].values

    cno_rmse = rmse(simu_cno, obs_cno, mask_obs_cno)
    cno_mape = mape(simu_cno, obs_cno, mask_obs_cno)
    cno_nse = nse(simu_cno, obs_cno, mask_obs_cno)
    cno_kg = klinggupta(simu_cno, obs_cno, mask_obs_cno)

    cna_rmse = rmse(simu_cna, obs_cna, mask_obs_cna)
    cna_mape = mape(simu_cna, obs_cna, mask_obs_cna)
    cna_nse = nse(simu_cna, obs_cna, mask_obs_cna)
    cna_kg = klinggupta(simu_cna, obs_cna, mask_obs_cna)

    cnn_rmse = rmse(simu_cnn, obs_cnn, mask_obs_cnn)
    cnn_mape = mape(simu_cnn, obs_cnn, mask_obs_cnn)
    cnn_nse = nse(simu_cnn, obs_cnn, mask_obs_cnn)
    cnn_kg = klinggupta(simu_cnn, obs_cnn, mask_obs_cnn)
    return [cno_rmse, cno_mape, cno_nse, cno_kg, cna_rmse, cna_mape, 
            cna_nse, cna_kg, cnn_rmse, cnn_mape, cnn_nse, cnn_kg]


if __name__ == "__main__":
    cpu_nums = int(mp.cpu_count())
    save_dir = "test/output/sa_evaluate"
    if not os.path.exists(save_dir):
        os.makedirs(save_dir, exist_ok=True)
    print("Model Start!")
    # read the parameters data
    parameters = pd.read_excel("test/data/0DReservoirModelSensitivitySample.xlsx", sheet_name="params")

    # read the observation of c_no, c_na, c_nn to reduce the consumption of IO
    obs = pd.read_excel("test/data/0DReservoirModelCalibrate.xlsx", sheet_name="ResAverage")
    cno_obs_with_nan = obs["Res_Cno"].values
    cna_obs_with_nan = obs["Res_Cna"].values
    cnn_obs_with_nan = obs["Res_Cnn"].values
    mask_cno_obs = ~np.isnan(cno_obs_with_nan)
    mask_cna_obs = ~np.isnan(cna_obs_with_nan)
    mask_cnn_obs = ~np.isnan(cnn_obs_with_nan)
    cno_obs = cno_obs_with_nan[mask_cno_obs]
    cna_obs = cna_obs_with_nan[mask_cna_obs]
    cnn_obs = cnn_obs_with_nan[mask_cnn_obs]

    batch = 120
    batch_size = 1024

    for major_index in range(0, batch):
        print(f"Calculating batch {major_index}...")
        offset = major_index * batch_size
        results = []
        results_df = pd.DataFrame(columns=['rmse_cno', 'mape_cno', 'nse_cno', 'kg_cno',
                                           'rmse_cna', 'mape_cna', 'nse_cna', 'kg_cna',
                                           'rmse_cnn', 'mape_cnn', 'nse_cnn', 'kg_cnn'])
        with Pool(cpu_nums) as pool:
            for sub_index in range(0, batch_size):
                print(f"Batch {major_index} - index {sub_index}")
                id_create = sub_index
                filename_in = 'test/data/0DReservoirModelCalibrate.xlsx'
                filename_out = f'test/temp/{id_create}0DReservoirModelCalibrate_out.xlsx'
                params = dict(parameters.iloc[offset + sub_index, 1:])
                results.append(pool.apply_async(evaluate_model, [id_create, filename_in, filename_out, params,
                                                                 cno_obs, cna_obs, cnn_obs, mask_cno_obs,
                                                                 mask_cno_obs, mask_cnn_obs]))
            for index, res in enumerate(results):
                valuesss = res.get()
                results_df.loc[index] = valuesss
        results_df.to_excel(f"{save_dir}/{major_index}SensitivityAnalysis.xlsx")
    print("End!")
