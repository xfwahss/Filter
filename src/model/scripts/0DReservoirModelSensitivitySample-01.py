# sample and calculate the model output
from SALib.sample import sobol as sample_sobol
import pandas as pd

problem = {
    "num_vars": 29,
    "names": [
        "k_rpon",
        "theta_rpon",
        "k_lpon",
        "theta_lpon",
        "k_don",
        "theta_don",
        "rnit0",
        "knit20",
        "foxmin",
        "c_oxc_nit",
        "c_oxo_nit",
        "theta_nit",
        "T_c_nit",
        "alpha",
        "rdeni0",
        "kdeni20",
        "Tc_deni",
        "theta_deni",
        "c_oxc_deni",
        "c_oxo_deni",
        "beta",
        "b_ndo_flux",
        "b_amm_flux",
        "b_nit_flux",
        "s_nrp",
        "s_nlp",
        "alpha_rpon",
        "alpha_lpon",
        "h0",
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

param_values = sample_sobol.sample(problem, 2048, seed=100)

param_values_dict = {
    problem["names"][i]: param_values[:, i] for i in range(problem["num_vars"])
}
param_values_df = pd.DataFrame(param_values_dict)
param_values_df.to_excel("test/data/0DReservoirModelSensitivitySample.xlsx", sheet_name="params")
print("Finished!")
